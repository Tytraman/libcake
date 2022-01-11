#ifndef __PIKA_FILE_H__
#define __PIKA_FILE_H__

#include "def.h"
#include "utf8.h"

#ifdef PIKA_WINDOWS
#include <windows.h>

typedef HANDLE pika_dir;

#define close_dir(x) FindClose(x)
#define file_exists(filename) (GetFileAttributesW(filename) != INVALID_FILE_ATTRIBUTES)
#else
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

typedef DIR *pika_dir;

#define close_dir(x) closedir(x)
#define file_exists(filename) (access(filename, F_OK) == 0)
#endif

typedef struct FileSnapshot {
    pika_dir dir;
    #ifdef PIKA_WINDOWS
    WIN32_FIND_DATAW dataw;
    #endif
    String_UTF8 path;
} FileSnapshot;

typedef struct ListFileSnapshot {
    FileSnapshot *list;
    unsigned long long current;
    unsigned long long length;
} ListFileSnapshot;

typedef struct FileList {
    unsigned long long length;
    String_UTF8 *list;
} FileList;

typedef pika_bool (*ListFileFilter)(String_UTF8 *filename);

void create_list_filesnapshot(ListFileSnapshot *list);
pika_bool list_filesnapshot_add(String_UTF8 *newPath, ListFileSnapshot *list);
void list_filesnapshot_remove_last(ListFileSnapshot *list);

void create_filelist(FileList *list);
void clear_filelist(FileList *list);
void add_file_element(String_UTF8 *element, FileList *dest);

unsigned long long list_files_recursive(pika_char *path, FileList *files, FileList *folders, ListFileFilter filter);


#endif