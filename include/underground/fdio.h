#ifndef __PIKA_FDIO_H__
#define __PIKA_FDIO_H__

#include "def.h"

#define FDIO_PTR(var) &var

#ifdef PIKA_WINDOWS

#define FDIO_ERROR_READ FALSE

#define FDIO_ERROR_OPEN INVALID_HANDLE_VALUE

#define FDIO_ACCESS_READ  GENERIC_READ
#define FDIO_ACCESS_WRITE GENERIC_WRITE
#define FDIO_ACCESS_READ_WRITE GENERIC_READ | GENERIC_WRITE
#define FDIO_ACCESS_APPEND FILE_APPEND_DATA

#define FDIO_SHARE_READ   FILE_SHARE_READ
#define FDIO_SHARE_WRITE  FILE_SHARE_WRITE
#define FDIO_SHARE_DELETE FILE_SHARE_DELETE

#define FDIO_OPEN_CREATE_ALWAYS        CREATE_ALWAYS
#define FDIO_OPEN_CREATE_IF_NOT_EXISTS CREATE_NEW
#define FDIO_OPEN_ALWAYS               OPEN_ALWAYS
#define FDIO_OPEN_IF_EXISTS            OPEN_EXISTING
#define FDIO_OPEN_TRUNC_IF_EXISTS      TRUNCATE_EXISTING

#define FDIO_ATTRIBUTE_NORMAL         FILE_ATTRIBUTE_NORMAL
#define FDIO_ATTRIBUTE_HIDDEN         FILE_ATTRIBUTE_HIDDEN
#define FDIO_ATTRIBUTE_READ_ONLY      FILE_ATTRIBUTE_READONLY
#define FDIO_ATTRIBUTE_SYSTEM         FILE_ATTRIBUTE_SYSTEM
#define FDIO_ATTRIBUTE_CASE_SENSITIVE FILE_FLAG_POSIX_SEMANTICS

typedef DWORD fdio_mode;

/*
        Lorsqu'un tableau de pika_fd est créé, exemple : pika_fd fd[2]

        fd[0] = stream de lecture
        fd[1] = stream d'écriture
*/
typedef HANDLE pika_fd;


#define __fdio_read(fd, numberOfBytesToRead, bytesRead, buffer) ReadFile(fd, buffer, numberOfBytesToRead, bytesRead, NULL)
#define fdio_read(fd, numberOfBytesToRead, bytesRead, buffer) __fdio_read(fd, numberOfBytesToRead, FDIO_PTR(bytesRead), buffer)

#define __fdio_write(fd, numberOfBytesToWrite, bytesWritten, buffer) WriteFile(fd, buffer, numberOfBytesToWrite, bytesWritten, NULL)
#define fdio_write(fd, numberOfBytesToWrite, bytesWritten, buffer) __fdio_write(fd, numberOfBytesToWrite, FDIO_PTR(bytesWritten), buffer)


/*
        Ouvre un fichier et retourne son FileDescriptor.

        En cas d'erreur, FDIO_ERROR_OPEN est retourné.
*/
pika_fd fdio_open_file(const uchar *filename, fdio_mode desiredAccess, fdio_mode shareMode, fdio_mode openMode, fdio_mode attributes);


// Ferme un FileDescriptor, peut-être utilisé sur un pika_fd.
#define fdio_close(fd) CloseHandle(fd)

#else
#include <fcntl.h>
#include <unistd.h>

#define FDIO_ERROR_READ -1

#define FDIO_ERROR_OPEN -1

#define FDIO_ACCESS_READ  O_RDONLY
#define FDIO_ACCESS_WRITE O_WRONLY
#define FDIO_ACCESS_READ_WRITE O_RDWR
#define FDIO_ACCESS_APPEND O_APPEND

#define FDIO_SHARE_READ   0
#define FDIO_SHARE_WRITE  0
#define FDIO_SHARE_DELETE 0

#define FDIO_OPEN_CREATE_ALWAYS        O_EXCL
#define FDIO_OPEN_CREATE_IF_NOT_EXISTS O_CREAT
#define FDIO_OPEN_ALWAYS               0
#define FDIO_OPEN_IF_EXISTS            0
#define FDIO_OPEN_TRUNC_IF_EXISTS      O_TRUNC

#define FDIO_ATTRIBUTE_NORMAL         0
#define FDIO_ATTRIBUTE_HIDDEN         0
#define FDIO_ATTRIBUTE_READ_ONLY      0
#define FDIO_ATTRIBUTE_SYSTEM         0
#define FDIO_ATTRIBUTE_CASE_SENSITIVE 0

typedef int pika_fd;
typedef int fdio_mode;

#define fdio_read(fd, numberOfBytesToRead, bytesRead, buffer) (bytesRead = read(fd, buffer, numberOfBytesToRead))
#define fdio_write(fd, numberOfBytesToWrite, bytesWritten, buffer) (bytesWritten = write(fd, buffer, numberOfBytesToWrite))

/*
        Ouvre un fichier et retourne son FileDescriptor.

        En cas d'erreur, FDIO_ERROR_OPEN est retourné.
*/
#define fdio_open_file(filename, desiredAccess, shareMode, openMode, attributes) open(filename, desiredAccess | shareMode | openMode | attributes)

// Ferme un FileDescriptor, peut-être utilisé sur un pika_fd.
#define fdio_close(fd) close(fd)

#endif



#endif