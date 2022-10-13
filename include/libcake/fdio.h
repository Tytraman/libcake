#ifndef __CAKE_FDIO_H__
#define __CAKE_FDIO_H__

#include "def.h"
#include "strutf8.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CAKE_FDIO_PTR(var) &var

#ifdef CAKE_WINDOWS

#define CAKE_FDIO_ERROR_READ  FALSE
#define CAKE_FDIO_ERROR_WRITE FALSE

#define CAKE_FDIO_ERROR_OPEN INVALID_HANDLE_VALUE

#define CAKE_FDIO_ACCESS_READ  GENERIC_READ
#define CAKE_FDIO_ACCESS_WRITE GENERIC_WRITE
#define CAKE_FDIO_ACCESS_READ_WRITE GENERIC_READ | GENERIC_WRITE
#define CAKE_FDIO_ACCESS_APPEND FILE_APPEND_DATA

#define CAKE_FDIO_SHARE_READ   FILE_SHARE_READ
#define CAKE_FDIO_SHARE_WRITE  FILE_SHARE_WRITE
#define CAKE_FDIO_SHARE_DELETE FILE_SHARE_DELETE

#define CAKE_FDIO_OPEN_CREATE_ALWAYS        CREATE_ALWAYS
#define CAKE_FDIO_OPEN_CREATE_IF_NOT_EXISTS CREATE_NEW
#define CAKE_FDIO_OPEN_ALWAYS               OPEN_ALWAYS
#define CAKE_FDIO_OPEN_IF_EXISTS            OPEN_EXISTING
#define CAKE_FDIO_OPEN_TRUNC_IF_EXISTS      TRUNCATE_EXISTING

#define CAKE_FDIO_ATTRIBUTE_NORMAL         FILE_ATTRIBUTE_NORMAL
#define CAKE_FDIO_ATTRIBUTE_HIDDEN         FILE_ATTRIBUTE_HIDDEN
#define CAKE_FDIO_ATTRIBUTE_READ_ONLY      FILE_ATTRIBUTE_READONLY
#define CAKE_FDIO_ATTRIBUTE_SYSTEM         FILE_ATTRIBUTE_SYSTEM
#define CAKE_FDIO_ATTRIBUTE_CASE_SENSITIVE FILE_FLAG_POSIX_SEMANTICS

typedef DWORD cake_fdio_mode;


#define __cake_fdio_read(fd, numberOfBytesToRead, bytesRead, buffer) ReadFile(fd, buffer, numberOfBytesToRead, bytesRead, NULL)
#define cake_fdio_read(fd, numberOfBytesToRead, bytesRead, buffer) __cake_fdio_read(fd, numberOfBytesToRead, CAKE_FDIO_PTR(bytesRead), buffer)

#define __cake_fdio_write(fd, numberOfBytesToWrite, bytesWritten, buffer) WriteFile(fd, buffer, numberOfBytesToWrite, bytesWritten, NULL)
#define cake_fdio_write(fd, numberOfBytesToWrite, bytesWritten, buffer) __cake_fdio_write(fd, numberOfBytesToWrite, CAKE_FDIO_PTR(bytesWritten), buffer)
#define cake_fdio_write_no_ret(fd, numberOfBytesToWrite, buffer) WriteFile(fd, buffer, numberOfBytesToWrite, NULL, NULL)



/*
        Ouvre un fichier et retourne son FileDescriptor.

        En cas d'erreur, CAKE_FDIO_ERROR_OPEN est retourné.
*/
cake_fd cake_fdio_open_file(const char *filename, ulong desiredAccess, ulong shareMode, ulong openMode, ulong attributes);

// Ferme un FileDescriptor, peut-être utilisé sur un cake_fd.
#define cake_fdio_close(fd) CloseHandle(fd)

#define CAKE_FDIO_CREATE_FAILS FALSE
#define cake_fdio_create_dir(filename) CreateDirectoryW(filename, NULL)

#else
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CAKE_FDIO_ERROR_READ  -1
#define CAKE_FDIO_ERROR_WRITE -1

#define CAKE_FDIO_ERROR_OPEN -1

#define CAKE_FDIO_ACCESS_READ  O_RDONLY
#define CAKE_FDIO_ACCESS_WRITE O_WRONLY
#define CAKE_FDIO_ACCESS_READ_WRITE O_RDWR
#define CAKE_FDIO_ACCESS_APPEND (O_WRONLY | O_APPEND)

#define CAKE_FDIO_SHARE_READ   0
#define CAKE_FDIO_SHARE_WRITE  0
#define CAKE_FDIO_SHARE_DELETE 0

#define CAKE_FDIO_OPEN_CREATE_ALWAYS        O_CREAT | O_TRUNC
#define CAKE_FDIO_OPEN_CREATE_IF_NOT_EXISTS O_CREAT
#define CAKE_FDIO_OPEN_ALWAYS               O_CREAT
#define CAKE_FDIO_OPEN_IF_EXISTS            0
#define CAKE_FDIO_OPEN_TRUNC_IF_EXISTS      O_TRUNC

#define CAKE_FDIO_ATTRIBUTE_NORMAL         0
#define CAKE_FDIO_ATTRIBUTE_HIDDEN         0
#define CAKE_FDIO_ATTRIBUTE_READ_ONLY      0
#define CAKE_FDIO_ATTRIBUTE_SYSTEM         0
#define CAKE_FDIO_ATTRIBUTE_CASE_SENSITIVE 0

typedef int cake_fd;
typedef int cake_fdio_mode;

#define cake_fdio_read(fd, numberOfBytesToRead, bytesRead, buffer) (bytesRead = read(fd, buffer, numberOfBytesToRead))
#define cake_fdio_write(fd, numberOfBytesToWrite, bytesWritten, buffer) (bytesWritten = write(fd, buffer, numberOfBytesToWrite))

#define cake_fdio_read_no_ret(fd, numberOfBytesToRead, buffer) read(fd, buffer, numberOfBytesToRead)
#define cake_fdio_write_no_ret(fd, numberOfBytesToWrite, buffer) write(fd, buffer, numberOfBytesToWrite)

/*
        Ouvre un fichier et retourne son FileDescriptor.

        En cas d'erreur, CAKE_FDIO_ERROR_OPEN est retourné.
*/
#define cake_fdio_open_file(filename, desiredAccess, shareMode, openMode, attributes) open(filename, desiredAccess | shareMode | openMode | attributes, 0777)

// Ferme un FileDescriptor, peut-être utilisé sur un cake_fd.
#define cake_fdio_close(fd) close(fd)


#define CAKE_FDIO_CREATE_FAILS -1
#define cake_fdio_create_dir(filename) mkdir(filename, 0777)


#endif

#define CAKE_FDIO_OLDER -1
#define CAKE_FDIO_EQUAL 0
#define CAKE_FDIO_NEWER 1
#define CAKE_FDIO_UNDEFINED -2

#define CAKE_FDIO_COMPARE_CREATION_TIME   0
#define CAKE_FDIO_COMPARE_ACCESS_TIME     1
#define CAKE_FDIO_COMPARE_LAST_WRITE_TIME 2

/*
        Compare le temps entre 2 fichiers.

        mode permet de spécifier quelle valeur de temps comparer :
        - CAKE_FDIO_COMPARE_CREATION_TIME
        - CAKE_FDIO_COMPARE_ACCESS_TIME
        - CAKE_FDIO_COMPARE_LAST_WRITE_TIME

        Retourne :
        - FDIO_OLDER : si fd est plus vieux que compareTo
        - CAKE_FDIO_EQUAL : si les 2 fichiers ont le même temps
        - CAKE_FDIO_NEWER : si fd est plus récent que compareTo
        - FDIO_UNDEFINED

        Note :
        Sous Unix, la date de création des fichiers n'est pas stockés, sauf sur certains OS.
        De ce fait, CAKE_FDIO_COMPARE_CREATION_TIME sous Unix retournera toujours FDIO_UNDEFINED.
*/
char cake_fdio_compare_time(cake_fd fd, cake_fd compareTo, cake_byte mode);

void cake_fdio_mem_copy_strutf8(Cake_String_UTF8 *dest, cake_fd fd, ushort buffSize);
void cake_fdio_mem_copy(uchar **dest, ulonglong *destLength, cake_fd fd, ushort buffSize);

#ifdef __cplusplus
}
#endif

#endif