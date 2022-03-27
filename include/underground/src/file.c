#include "../file.h"
#include "../strutf8.h"
#include "../strutf16.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef CAKE_UNIX
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#endif


void cake_create_list_filesnapshot(Cake_List_FileSnapshot *list) {
    list->list = NULL;
    list->current = 0;
    list->length = 0;
}

cake_bool cake_list_filesnapshot_add(Cake_String_UTF8 *newPath, Cake_List_FileSnapshot *list) {
    if(list->current == list->length) {
        (list->length)++;
        list->list = (Cake_FileSnapshot *) realloc(list->list, list->length * sizeof(Cake_FileSnapshot));
        cake_create_strutf8(&list->list[list->current].path);
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
    list->list[list->current].path.bytes = NULL;
    #ifdef CAKE_WINDOWS
    FindClose(list->list[list->current].dir);
    #else
    closedir(list->list[list->current].dir);
    #endif
}

void cake_list_files_recursive(const uchar *path, Cake_List_String_UTF8 *files, Cake_List_String_UTF8 *folders, ListFileFilter filter) {
    Cake_List_FileSnapshot snapshots;
    cake_create_list_filesnapshot(&snapshots);

    Cake_String_UTF8 *utf8Path = cake_strutf8(path);

    if(!cake_list_filesnapshot_add(utf8Path, &snapshots)) {
        free(snapshots.list);
        cake_free_strutf8(utf8Path);
        return;
    }

    #ifdef CAKE_UNIX
    struct dirent *ep;
    struct stat epStat;
    #endif

    while(1) {
        // Si on trouve quelque chose
        if(
            #ifdef CAKE_WINDOWS
            FindNextFileW(snapshots.list[snapshots.current - 1].dir, &snapshots.list[snapshots.current - 1].dataw)
            #else
            (ep = readdir(snapshots.list[snapshots.current - 1].dir)) != NULL
            #endif
        ){
            cake_strutf8_copy(utf8Path, &snapshots.list[snapshots.current - 1].path);
            if(utf8Path->bytes[utf8Path->data.length - 1] != FILE_SEPARATOR)
                cake_strutf8_add_char_array(utf8Path, FILE_SEPARATOR_CHAR_STR);
            #ifdef CAKE_WINDOWS
            cake_strutf8_add_wchar_array(utf8Path, snapshots.list[snapshots.current - 1].dataw.cFileName);
            #else
            cake_strutf8_add_char_array(utf8Path, ep->d_name);
            #endif

            #ifdef CAKE_UNIX
            stat(utf8Path->bytes, &epStat);
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
                        cake_list_strutf8_add_char_array(folders, utf8Path->bytes);
                    cake_list_filesnapshot_add(utf8Path, &snapshots);
                }
            }else {
                if(filter != NULL && !filter(utf8Path)) goto list_files_ignore;
                if(files != NULL)
                    cake_list_strutf8_add_char_array(files, utf8Path->bytes);
                else
                    printf("%s\n", utf8Path->bytes);
list_files_ignore: ;
            }
        }else {
            cake_list_filesnapshot_remove_last(&snapshots);
            if(snapshots.current == 0)
                break;
        }
    }
    cake_free_strutf8(utf8Path);

    free(snapshots.list);
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

#ifdef CAKE_WINDOWS
cake_bool cake_file_exists(const uchar *filename) {
    Cake_String_UTF16 name16;
    cake_create_strutf16(&name16);
    cake_char_array_to_strutf16(filename, &name16);

    cake_bool res = (GetFileAttributesW(name16.characteres) != INVALID_FILE_ATTRIBUTES);
    free(name16.characteres);
    return res;
}
#endif


// TODO: portage Linux
cake_bool cake_mkdirs(const uchar *filepath) {
    Cake_String_UTF16 pathCopy;
    cake_create_strutf16(&pathCopy);
    cake_char_array_to_strutf16(filepath, &pathCopy);

    wchar_t *p = NULL;
    wchar_t slash;

    // On enlève le dernier slash si y en a un
    while(
        pathCopy.characteres[pathCopy.length - 1] == FILE_SEPARATOR ||
        pathCopy.characteres[pathCopy.length - 1] == FILE_SEPARATOR_REVERSE
    ) {
        if(pathCopy.length == 1)
            return cake_false;
        pathCopy.length--;
        pathCopy.characteres[pathCopy.length] = L'\0';
    }

    #ifdef CAKE_WINDOWS
    DWORD fAttribute = GetFileAttributesW(pathCopy.characteres);
    #endif

    if(
        #ifdef CAKE_WINDOWS
        (fAttribute != INVALID_FILE_ATTRIBUTES && (fAttribute & FILE_ATTRIBUTE_DIRECTORY))
        #else

        #endif
    ) {
        free(pathCopy.characteres);
        return cake_false;
    }

    if(pathCopy.characteres[1] == L':' && (pathCopy.characteres[2] == L'\\' || pathCopy.characteres[2] == L'/'))
        p = pathCopy.characteres + 3;
    else
        p = pathCopy.characteres + 1;

    for(; *p; p++) {
        if(*p == L'/' || *p == L'\\') {
            slash = *p;
            *p = L'\0';
            CreateDirectoryW(pathCopy.characteres, NULL);
            *p = slash;
        }
    }
    if(!CreateDirectoryW(pathCopy.characteres, NULL)) {
        free(pathCopy.characteres);
        return cake_false;
    }
    free(pathCopy.characteres);
    return cake_true;
}

#ifdef CAKE_WINDOWS
cake_bool cake_delete_file(const uchar *filename) {
    Cake_String_UTF16 name16;
    cake_create_strutf16(&name16);
    cake_char_array_to_strutf16(filename, &name16);
    cake_bool ret = DeleteFileW(name16.characteres);
    free(name16.characteres);
    return ret;
}
#endif
