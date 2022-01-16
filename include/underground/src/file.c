#include "../file.h"
#include "../utf8.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef PIKA_UNIX
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#endif


void create_filelist(FileList *list) {
    list->list = NULL;
    list->length = 0;
}

void clear_filelist(FileList *list) {
    unsigned long long i;
    for(i = 0; i < list->length; i++)
        free(list->list[i].bytes);
    free(list->list);
}

void add_file_element(String_UTF8 *element, FileList *dest) {
    dest->list = (String_UTF8 *) realloc(dest->list, (dest->length + 1) * sizeof(String_UTF8));

    create_strutf8(&dest->list[dest->length]);
    strutf8_copy(&dest->list[dest->length], element);

    (dest->length)++;
}

void create_list_filesnapshot(ListFileSnapshot *list) {
    list->list = NULL;
    list->current = 0;
    list->length = 0;
}

pika_bool list_filesnapshot_add(String_UTF8 *newPath, ListFileSnapshot *list) {
    if(list->current == list->length) {
        (list->length)++;
        list->list = (FileSnapshot *) realloc(list->list, list->length * sizeof(FileSnapshot));
    }

    strutf8_copy(&list->list[list->current].path, newPath);

    #ifdef PIKA_WINDOWS
    String_UTF16 utf16Path;
    create_strutf16(&utf16Path);
    strutf8_to_utf16(&list->list[list->current].path, &utf16Path);
    strutf16_add_wchar_array(&utf16Path, FILE_SEPARATOR_STR L"*");
    #endif

    if(
        (
            list->list[list->current].dir =
            #ifdef PIKA_WINDOWS
            FindFirstFileW(utf16Path.characteres, &list->list[list->current].dataw)
            #else
            opendir(list->list[list->current].path.bytes)
            #endif
        )
        ==
        #ifdef PIKA_WINDOWS
        INVALID_HANDLE_VALUE
        #else
        NULL
        #endif
    ) {
        #ifdef PIKA_WINDOWS
        free(utf16Path.characteres);
        #endif
        free(list->list[list->current].path.bytes);
        return pika_false;
    }

    #ifdef PIKA_WINDOWS
    free(utf16Path.characteres);
    #endif

    (list->current)++;
    return pika_true;
}

void list_filesnapshot_remove_last(ListFileSnapshot *list) {
    (list->current)--;
    free(list->list[list->current].path.bytes);
    #ifdef PIKA_WINDOWS
    FindClose(list->list[list->current].dir);
    #else
    closedir(list->list[list->current].dir);
    #endif
}

unsigned long long list_files_recursive(pika_char *path, FileList *files, FileList *folders, ListFileFilter filter) {
    ListFileSnapshot snapshots;
    create_list_filesnapshot(&snapshots);

    String_UTF8 utf8Path;

    #ifdef PIKA_WINDOWS
    wchar_array_to_strutf8(path, &utf8Path);
    #else
    create_strutf8(&utf8Path);
    array_char_to_strutf8(path, &utf8Path);
    #endif

    if(!list_filesnapshot_add(&utf8Path, &snapshots)) {
        free(snapshots.list);
        free(utf8Path.bytes);
        return 0;
    }

    free(utf8Path.bytes);

    #ifdef PIKA_UNIX
    struct dirent *ep;
    struct stat epStat;
    #endif

    unsigned long long number = 0;

    while(1) {
        if(
            #ifdef PIKA_WINDOWS
            FindNextFileW(snapshots.list[snapshots.current - 1].dir, &snapshots.list[snapshots.current - 1].dataw)
            #else
            (ep = readdir(snapshots.list[snapshots.current - 1].dir)) != NULL
            #endif
        ){
            create_strutf8(&utf8Path);
            strutf8_copy(&utf8Path, &snapshots.list[snapshots.current - 1].path);
            if(utf8Path.bytes[utf8Path.data.length - 1] != FILE_SEPARATOR)
                strutf8_add_char_array(&utf8Path, "\\");

            #ifdef PIKA_WINDOWS
            strutf8_add_wchar_array(&utf8Path, snapshots.list[snapshots.current - 1].dataw.cFileName);
            #else
            strutf8_add_char_array(&utf8Path, ep->d_name);
            #endif

            #ifdef PIKA_UNIX
            stat(utf8Path.bytes, &epStat);
            #endif

            // Si c'est un dossier
            if(
                #ifdef PIKA_WINDOWS
                snapshots.list[snapshots.current - 1].dataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
                #else
                epStat.st_mode & S_IFDIR
                #endif
            ) {
                if(
                    #ifdef PIKA_WINDOWS
                    wcscmp(snapshots.list[snapshots.current - 1].dataw.cFileName, L".") != 0 && wcscmp(snapshots.list[snapshots.current - 1].dataw.cFileName, L"..") != 0
                    #else
                    strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0
                    #endif
                ) {
                    if(folders != NULL)
                        add_file_element(&utf8Path, folders);
                    list_filesnapshot_add(&utf8Path, &snapshots);
                }
            }else {
                if(filter != NULL && !filter(&utf8Path)) goto list_files_ignore;
                number++;
                if(files != NULL)
                    add_file_element(&utf8Path, files);
                else
                    printf("%s\n", utf8Path.bytes);
list_files_ignore: ;
            }
            free(utf8Path.bytes);
        }else {
            list_filesnapshot_remove_last(&snapshots);
            if(snapshots.current == 0)
                break;
        }
    }

    free(snapshots.list);
    return number;
}

pika_bool file_mem_copy(
    #ifdef PIKA_UNIX
    char *filename,
    #else
    wchar_t *filename,
    #endif
    String_UTF8 *dest,
    ushort buffSize
) {
    #ifdef PIKA_UNIX
    int fd = open(filename, O_RDONLY);
    if(fd == -1)
        return pika_false;
    uchar *buffer = (uchar *) malloc(buffSize * sizeof(uchar));

    ssize_t bytesRead;
    pika_bool loop = pika_true;
    while(loop) {
        bytesRead = read(fd, buffer, buffSize);
        switch(bytesRead) {
            case -1:{
                close(fd);
                free(buffer);
                return pika_false;
            }
            case 0:{
                loop = pika_false;
                break;
            }
            default:{
                dest->bytes = (uchar *) realloc(dest->bytes, (dest->data.length + bytesRead) * sizeof(uchar));
                memcpy(&dest->bytes[dest->data.length], buffer, bytesRead * sizeof(uchar));
                dest->data.length += bytesRead;
                break;
            }
        }
    }
    close(fd);
    free(buffer);
    dest->bytes = (uchar *) realloc(dest->bytes, dest->data.length * sizeof(uchar) + sizeof(uchar));
    dest->bytes[dest->data.length] = '\0';
    dest->length = strutf8_length(dest);
    #else
    HANDLE hFile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
        return pika_false;

    uchar *buffer = (uchar *) malloc(buffSize * sizeof(uchar));
    pika_bool loop = pika_true;
    pika_bool result;
    DWORD bytesRead;
    while(loop) {
        result = ReadFile(hFile, buffer, buffSize, &bytesRead, NULL);
        if(!result) {
            CloseHandle(hFile);
            free(buffer);
            return pika_false;
        }
        if(bytesRead == 0)
            loop = pika_false;
        else {
            dest->bytes = (uchar *) realloc(dest->bytes, (dest->data.length + bytesRead) * sizeof(uchar));
            memcpy(&dest->bytes[dest->data.length], buffer, bytesRead * sizeof(uchar));
            dest->data.length += bytesRead;
        }
    }
    CloseHandle(hFile);
    free(buffer);
    dest->bytes = (uchar *) realloc(dest->bytes, dest->data.length * sizeof(uchar) + sizeof(uchar));
    dest->bytes[dest->data.length] = '\0';
    dest->length = strutf8_length(dest);
    #endif

    return pika_true;
}

