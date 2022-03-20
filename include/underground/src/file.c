#include "../file.h"
#include "../strutf8.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef CAKE_UNIX
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#endif


void cake_create_filelist(Cake_FileList *list) {
    list->list = NULL;
    list->length = 0;
}

void cake_clear_filelist(Cake_FileList *list) {
    unsigned long long i;
    for(i = 0; i < list->length; i++)
        free(list->list[i].bytes);
    free(list->list);
}

void cake_add_file_element(Cake_String_UTF8 *element, Cake_FileList *dest) {
    dest->list = (Cake_String_UTF8 *) realloc(dest->list, (dest->length + 1) * sizeof(Cake_String_UTF8));

    cake_create_strutf8(&dest->list[dest->length]);
    cake_strutf8_copy(&dest->list[dest->length], element);

    (dest->length)++;
}

void cake_create_list_filesnapshot(Cake_List_FileSnapshot *list) {
    list->list = NULL;
    list->current = 0;
    list->length = 0;
}

cake_bool cake_list_filesnapshot_add(Cake_String_UTF8 *newPath, Cake_List_FileSnapshot *list) {
    if(list->current == list->length) {
        (list->length)++;
        list->list = (Cake_FileSnapshot *) realloc(list->list, list->length * sizeof(Cake_FileSnapshot));
    }

    cake_strutf8_copy(&list->list[list->current].path, newPath);

    #ifdef CAKE_WINDOWS
    Cake_String_UTF16 utf16Path;
    cake_create_strutf16(&utf16Path);
    cake_strutf8_to_utf16(&list->list[list->current].path, &utf16Path);
    cake_strutf16_add_wchar_array(&utf16Path, FILE_SEPARATOR_STR L"*");
    #endif

    if(
        (
            list->list[list->current].dir =
            #ifdef CAKE_WINDOWS
            FindFirstFileW(utf16Path.characteres, &list->list[list->current].dataw)
            #else
            opendir(list->list[list->current].path.bytes)
            #endif
        )
        ==
        #ifdef CAKE_WINDOWS
        INVALID_HANDLE_VALUE
        #else
        NULL
        #endif
    ) {
        #ifdef CAKE_WINDOWS
        free(utf16Path.characteres);
        #endif
        free(list->list[list->current].path.bytes);
        return cake_false;
    }

    #ifdef CAKE_WINDOWS
    free(utf16Path.characteres);
    #endif

    (list->current)++;
    return cake_true;
}

void cake_list_filesnapshot_remove_last(Cake_List_FileSnapshot *list) {
    (list->current)--;
    free(list->list[list->current].path.bytes);
    #ifdef CAKE_WINDOWS
    FindClose(list->list[list->current].dir);
    #else
    closedir(list->list[list->current].dir);
    #endif
}

unsigned long long cake_list_files_recursive(cake_char *path, Cake_FileList *files, Cake_FileList *folders, ListFileFilter filter) {
    Cake_List_FileSnapshot snapshots;
    cake_create_list_filesnapshot(&snapshots);

    Cake_String_UTF8 utf8Path;

    #ifdef CAKE_WINDOWS
    cake_wchar_array_to_strutf8(path, &utf8Path);
    #else
    cake_create_strutf8(&utf8Path);
    cake_char_array_to_strutf8(path, &utf8Path);
    #endif

    if(!cake_list_filesnapshot_add(&utf8Path, &snapshots)) {
        free(snapshots.list);
        free(utf8Path.bytes);
        return 0;
    }

    free(utf8Path.bytes);

    #ifdef CAKE_UNIX
    struct dirent *ep;
    struct stat epStat;
    #endif

    unsigned long long number = 0;

    while(1) {
        if(
            #ifdef CAKE_WINDOWS
            FindNextFileW(snapshots.list[snapshots.current - 1].dir, &snapshots.list[snapshots.current - 1].dataw)
            #else
            (ep = readdir(snapshots.list[snapshots.current - 1].dir)) != NULL
            #endif
        ){
            cake_create_strutf8(&utf8Path);
            cake_strutf8_copy(&utf8Path, &snapshots.list[snapshots.current - 1].path);
            if(utf8Path.bytes[utf8Path.data.length - 1] != FILE_SEPARATOR)
                cake_strutf8_add_char_array(&utf8Path, "\\");

            #ifdef CAKE_WINDOWS
            cake_strutf8_add_wchar_array(&utf8Path, snapshots.list[snapshots.current - 1].dataw.cFileName);
            #else
            cake_strutf8_add_char_array(&utf8Path, ep->d_name);
            #endif

            #ifdef CAKE_UNIX
            stat(utf8Path.bytes, &epStat);
            #endif

            // Si c'est un dossier
            if(
                #ifdef CAKE_WINDOWS
                snapshots.list[snapshots.current - 1].dataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
                #else
                epStat.st_mode & S_IFDIR
                #endif
            ) {
                if(
                    #ifdef CAKE_WINDOWS
                    wcscmp(snapshots.list[snapshots.current - 1].dataw.cFileName, L".") != 0 && wcscmp(snapshots.list[snapshots.current - 1].dataw.cFileName, L"..") != 0
                    #else
                    strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0
                    #endif
                ) {
                    if(folders != NULL)
                        cake_add_file_element(&utf8Path, folders);
                    cake_list_filesnapshot_add(&utf8Path, &snapshots);
                }
            }else {
                if(filter != NULL && !filter(&utf8Path)) goto list_files_ignore;
                number++;
                if(files != NULL)
                    cake_add_file_element(&utf8Path, files);
                else
                    printf("%s\n", utf8Path.bytes);
list_files_ignore: ;
            }
            free(utf8Path.bytes);
        }else {
            cake_list_filesnapshot_remove_last(&snapshots);
            if(snapshots.current == 0)
                break;
        }
    }

    free(snapshots.list);
    return number;
}

cake_bool file_mem_copy(
    #ifdef CAKE_UNIX
    char *filename,
    #else
    wchar_t *filename,
    #endif
    Cake_String_UTF8 *dest,
    ushort buffSize
) {
    #ifdef CAKE_UNIX
    int fd = open(filename, O_RDONLY);
    if(fd == -1)
        return cake_false;
    uchar *buffer = (uchar *) malloc(buffSize * sizeof(uchar));

    ssize_t bytesRead;
    cake_bool loop = cake_true;
    while(loop) {
        bytesRead = read(fd, buffer, buffSize);
        switch(bytesRead) {
            case -1:{
                close(fd);
                free(buffer);
                return cake_false;
            }
            case 0:{
                loop = cake_false;
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
    dest->length = cake_strutf8_length(dest);
    #else
    HANDLE hFile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
        return cake_false;

    uchar *buffer = (uchar *) malloc(buffSize * sizeof(uchar));
    cake_bool loop = cake_true;
    cake_bool result;
    DWORD bytesRead;
    while(loop) {
        result = ReadFile(hFile, buffer, buffSize, &bytesRead, NULL);
        if(!result) {
            CloseHandle(hFile);
            free(buffer);
            return cake_false;
        }
        if(bytesRead == 0)
            loop = cake_false;
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
    dest->length = cake_strutf8_length(dest);
    #endif

    return cake_true;
}

