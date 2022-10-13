#include "../file.h"
#include "../strutf8.h"
#include "../strutf16.h"
#include "../alloc.h"

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
            opendir((cchar_ptr) list->list[list->current].path.bytes)
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

void cake_list_files_recursive(const char *path, Cake_List_String_UTF8 *files, Cake_List_String_UTF8 *folders, ListFileFilter filter, void *args) {
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
            if(utf8Path->bytes[utf8Path->size - 1] != FILE_SEPARATOR)
                cake_strutf8_add_char_array(utf8Path, FILE_SEPARATOR_CHAR_STR);
            #ifdef CAKE_WINDOWS
            cake_strutf8_add_wchar_array(utf8Path, snapshots.list[snapshots.current - 1].dataw.cFileName);
            #else
            cake_strutf8_add_char_array(utf8Path, ep->d_name);
            #endif

            #ifdef CAKE_UNIX
            stat((cchar_ptr) utf8Path->bytes, &epStat);
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
                        cake_list_strutf8_add_char_array(folders, (cchar_ptr) utf8Path->bytes);
                    cake_list_filesnapshot_add(utf8Path, &snapshots);
                }
            }else {
                if(filter != NULL && !filter(utf8Path, args)) goto list_files_ignore;
                if(files != NULL)
                    cake_list_strutf8_add_char_array(files, (cchar_ptr) utf8Path->bytes);
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
                dest->bytes = (uchar *) realloc(dest->bytes, (dest->size + bytesRead) * sizeof(uchar));
                memcpy(&dest->bytes[dest->size], buffer, bytesRead * sizeof(uchar));
                dest->size += bytesRead;
                break;
            }
        }
    }
    close(fd);
    free(buffer);
    dest->bytes = (uchar *) realloc(dest->bytes, dest->size * sizeof(uchar) + sizeof(uchar));
    dest->bytes[dest->size] = '\0';
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
            dest->bytes = (uchar *) realloc(dest->bytes, (dest->size + bytesRead) * sizeof(uchar));
            memcpy(&dest->bytes[dest->size], buffer, bytesRead * sizeof(uchar));
            dest->size += bytesRead;
        }
    }
    CloseHandle(hFile);
    free(buffer);
    dest->bytes = (uchar *) realloc(dest->bytes, dest->size * sizeof(uchar) + sizeof(uchar));
    dest->bytes[dest->size] = '\0';
    dest->length = cake_strutf8_length(dest);
    #endif

    return cake_true;
}

#ifdef CAKE_WINDOWS
cake_bool cake_file_exists(const char *filename) {
    Cake_String_UTF16 name16;
    cake_create_strutf16(&name16);
    cake_char_array_to_strutf16(filename, &name16);

    cake_bool res = (GetFileAttributesW(name16.characteres) != INVALID_FILE_ATTRIBUTES);
    free(name16.characteres);
    return res;
}
#endif


cake_bool cake_mkdirs(const char *filepath) {
    #ifdef CAKE_WINDOWS
    Cake_String_UTF16 pathCopy;
    cake_create_strutf16(&pathCopy);
    cake_char_array_to_strutf16(filepath, &pathCopy);

    wchar_t *p = NULL;
    wchar_t slash;
    #else
    Cake_String_UTF8 *pathCopy = cake_strutf8(filepath);

    uchar *p = NULL;
    uchar slash;
    #endif

    // On enlève le dernier slash si y en a un
    while(
        #ifdef CAKE_WINDOWS
        pathCopy.characteres[pathCopy.length - 1] == FILE_SEPARATOR ||
        pathCopy.characteres[pathCopy.length - 1] == FILE_SEPARATOR_REVERSE
        #else
        pathCopy->bytes[pathCopy->size - 1] == FILE_SEPARATOR ||
        pathCopy->bytes[pathCopy->size - 1] == FILE_SEPARATOR_REVERSE
         #endif
    ) {
        if(
            #ifdef CAKE_WINDOWS
            pathCopy.length == 1
            #else
            pathCopy->size == 1
            #endif
        )
            return cake_false;
        
        #ifdef CAKE_WINDOWS
        pathCopy.length--;
        pathCopy.characteres[pathCopy.length] = L'\0';
        #else
        (pathCopy->size)--;
        pathCopy->bytes[pathCopy->size] = '\0';
        #endif
    }

    #ifdef CAKE_WINDOWS
    DWORD fAttribute = GetFileAttributesW(pathCopy.characteres);
    #endif

    if(
        #ifdef CAKE_WINDOWS
        (fAttribute != INVALID_FILE_ATTRIBUTES && (fAttribute & FILE_ATTRIBUTE_DIRECTORY))
        #else
        cake_file_exists(filepath)
        #endif
    ) {
        #ifdef CAKE_WINDOWS
        free(pathCopy.characteres);
        #else
        cake_free_strutf8(pathCopy);
        #endif
        return cake_false;
    }

    if(
        #ifdef CAKE_WINDOWS
        pathCopy.characteres[1] == L':' && (pathCopy.characteres[2] == FILE_SEPARATOR || pathCopy.characteres[2] == FILE_SEPARATOR_REVERSE)
        #else
        pathCopy->bytes[1] == ':' && (pathCopy->bytes[2] == FILE_SEPARATOR || pathCopy->bytes[2] == FILE_SEPARATOR_REVERSE)
        #endif
    )
        #ifdef CAKE_WINDOWS
        p = pathCopy.characteres + 3;
        #else
        p = pathCopy->bytes + 3;
        #endif
    else
        #ifdef CAKE_WINDOWS
        p = pathCopy.characteres + 1;
        #else
        p = pathCopy->bytes + 1;
        #endif

    for(; *p; p++) {
        if(*p == FILE_SEPARATOR || *p == FILE_SEPARATOR_REVERSE) {
            slash = *p;
            *p = CAKE_CHAR('\0');
            #ifdef CAKE_WINDOWS
            CreateDirectoryW(pathCopy.characteres, NULL);
            #else
            if(mkdir((cchar_ptr) pathCopy->bytes, 0777) == -1) {
                if(errno != EEXIST) {
                    cake_free_strutf8(pathCopy);
                    return cake_false;
                }
            }
            #endif
            *p = slash;
        }
    }
    #ifdef CAKE_WINDOWS
    if(!CreateDirectoryW(pathCopy.characteres, NULL)) {
        free(pathCopy.characteres);
        return cake_false;
    }
    #else
    if(mkdir((cchar_ptr) pathCopy->bytes, 0777) == -1) {
        if(errno == EEXIST) {
            cake_free_strutf8(pathCopy);
            return cake_false;
        }
    }
    #endif

    #ifdef CAKE_WINDOWS
    free(pathCopy.characteres);
    #else
    cake_free_strutf8(pathCopy);
    #endif
    return cake_true;
}

#ifdef CAKE_WINDOWS
cake_bool cake_delete_file(const char *filename) {
    Cake_String_UTF16 name16;
    cake_create_strutf16(&name16);
    cake_char_array_to_strutf16(filename, &name16);
    cake_bool ret = DeleteFileW(name16.characteres);
    free(name16.characteres);
    return ret;
}

cake_bool cake_delete_folder(const char *pathname) {
    Cake_String_UTF16 name16;
    cake_create_strutf16(&name16);
    cake_char_array_to_strutf16(pathname, &name16);
    cake_bool ret = RemoveDirectoryW(name16.characteres);
    free(name16.characteres);
    return ret;
}
#endif


cake_bool __cake_open_file(Cake_File *dest, const char *filename, cake_mask accessMode, cake_mask shareMode, cake_mask openMode, cake_mask attributes) {
    dest->filename = cake_strutf8(filename);
    if(dest->filename == NULL)
        return cake_false;

    // TODO: Portage Linux
#ifdef CAKE_WINDOWS
    Cake_String_UTF16 utf = { 0 };
    cake_strutf8_to_utf16(dest->filename, &utf);

    dest->fd = CreateFileW(
        utf.characteres,
        accessMode,
        shareMode,
        NULL,
        openMode,
        attributes,
        NULL
    );
    free(utf.characteres);

    if(dest->fd == INVALID_HANDLE_VALUE) {
        cake_free_strutf8(dest->filename);
        return cake_false;
    }
#else

#endif
    dest->accessMode = accessMode;
    dest->shareMode = shareMode;
    return cake_true;
}

void cake_close_file(Cake_File *file) {
    // TODO: Portage Linux
#ifdef CAKE_WINDOWS
    CloseHandle(file->fd);
#else

#endif
    cake_free_strutf8(file->filename);
}

cake_bool cake_folder_watcher_start(Cake_FolderWatcher *watcher) {
    // TODO: Portage Linux
    
#ifdef CAKE_WINDOWS
    // C'est dans cette structure que sera stocké le nom du fichier modifié
    FILE_NOTIFY_INFORMATION *info = (FILE_NOTIFY_INFORMATION *) cake_new(1024 * sizeof(*info));     // Cette structure est un peu bizarre et je ne sais pas vraiment pourquoi j'ai mis 1024
    if(info == NULL)
        return cake_false;

    cake_bool ret = cake_false;
    DWORD bytesReturned;

    // ReadDirectoryChanges est une fonction bloquante qui attend une modification dans le dossier spécifié
    if(ReadDirectoryChangesW(watcher->folder.fd, info, 1024 * sizeof(*info), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE, &bytesReturned, NULL, NULL)) {
        // S'il n'y a pas de callback, il n'y a pas vraiment d'intérêt à utiliser cette fonction
        if(watcher->callback != NULL) {
            // Conversion du nom de fichier modifié qui est en UTF-16 en UTF-8
            Cake_String_UTF8 utf = { 0 };
            cake_wchar_array_to_strutf8_len(&utf, info[0].FileName, info[0].FileNameLength);

            // On appelle le callback
            ret = watcher->callback(&utf);

            free(utf.bytes);
        }
    }
#else

#endif
    cake_free(info);
    return ret;
}
