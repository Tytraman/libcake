#ifndef __CAKE_FILE_H__
#define __CAKE_FILE_H__

#include "def.h"
#include "utf8.h"

#ifdef CAKE_WINDOWS
#include <windows.h>

typedef HANDLE cake_dir;

#define cake_close_dir(x) FindClose(x)
#define cake_file_exists(filename) (GetFileAttributesW(filename) != INVALID_FILE_ATTRIBUTES)

#else
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

typedef DIR *cake_dir;

#define cake_close_dir(x) closedir(x)
#define cake_file_exists(filename) (access(filename, F_OK) == 0)
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

typedef struct cake_filelist {
    ulonglong length;
    Cake_String_UTF8 *list;
} Cake_FileList;

typedef cake_bool (*ListFileFilter)(Cake_String_UTF8 *filename);

void cake_create_list_filesnapshot(Cake_List_FileSnapshot *list);
cake_bool cake_list_filesnapshot_add(Cake_String_UTF8 *newPath, Cake_List_FileSnapshot *list);
void cake_list_filesnapshot_remove_last(Cake_List_FileSnapshot *list);

void cake_create_filelist(Cake_FileList *list);
void cake_clear_filelist(Cake_FileList *list);
void cake_add_file_element(Cake_String_UTF8 *element, Cake_FileList *dest);

ulonglong cake_list_files_recursive(cake_char *path, Cake_FileList *files, Cake_FileList *folders, ListFileFilter filter);


cake_bool cake_file_mem_copy(
    #ifdef CAKE_UNIX
    char *filename,
    #else
    wchar_t *filename,
    #endif
    Cake_String_UTF8 *dest,
    ushort buffSize
);

#endif