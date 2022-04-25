#ifndef __CAKE_FILE_H__
#define __CAKE_FILE_H__

#include "def.h"
#include "strutf8.h"

#ifdef CAKE_WINDOWS
#include <windows.h>

typedef HANDLE cake_dir;

#define cake_close_dir(x) FindClose(x)

cake_bool cake_file_exists(const char *filename);
cake_bool cake_delete_file(const char *filename);
cake_bool cake_delete_folder(const char *pathname);

#else
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

typedef DIR *cake_dir;

#define cake_close_dir(x) closedir(x)
#define cake_file_exists(filename) (access(filename, F_OK) == 0)
#define cake_delete_file(filename) unlink(filename)
#define cake_delete_folder(pathname) rmdir(pathname)
#endif

typedef struct cake_filesnapshot {
    cake_dir dir;
    #ifdef CAKE_WINDOWS
    WIN32_FIND_DATAW dataw;
    #endif
    Cake_String_UTF8 path;
} Cake_FileSnapshot;

typedef struct cake_list_filesnapshot {
    Cake_FileSnapshot *list;
    ulonglong current;
    ulonglong length;
} Cake_List_FileSnapshot;

typedef cake_bool (*ListFileFilter)(Cake_String_UTF8 *filename, void *args);

void cake_create_list_filesnapshot(Cake_List_FileSnapshot *list);
cake_bool cake_list_filesnapshot_add(Cake_String_UTF8 *newPath, Cake_List_FileSnapshot *list);
void cake_list_filesnapshot_remove_last(Cake_List_FileSnapshot *list);

void cake_list_files_recursive(const char *path, Cake_List_String_UTF8 *files, Cake_List_String_UTF8 *folders, ListFileFilter filter, void *args);


cake_bool cake_file_mem_copy(
    #ifdef CAKE_UNIX
    char *filename,
    #else
    wchar_t *filename,
    #endif
    Cake_String_UTF8 *dest,
    ushort buffSize
);

/**
 * @brief Crée tous les dossiers du chemin passé.
 * 
 * @param filepath Chemin final voulu.
 * @return `cake_true` si tous les dossiers ont été créés.
 */
cake_bool cake_mkdirs(const char *filepath);

#endif