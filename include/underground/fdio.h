#ifndef __PIKA_FDIO_H__
#define __PIKA_FDIO_H__

#include "def.h"

#ifdef PIKA_WINDOWS

#define FDIO_ERROR_READ FALSE

#define FDIO_ERROR_OPEN INVALID_HANDLE_VALUE

#define FDIO_ACCESS_READ  GENERIC_READ
#define FDIO_ACCESS_WRITE GENERIC_WRITE

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

#define FDIO_PTR(var) &var

// TODO: portage Linux

#define __fdio_read(fd, numberOfBytesToRead, bytesRead, buffer) ReadFile(fd, buffer, numberOfBytesToRead, bytesRead, NULL)
#define fdio_read(fd, numberOfBytesToRead, bytesRead, buffer) __fdio_read(fd, numberOfBytesToRead, FDIO_PTR(bytesRead), buffer)

#define fdio_write(fd, numberOfBytesToWrite, pBytesWritten, buffer) WriteFile(fd, buffer, numberOfBytesToWrite, pBytesWritten, NULL)


/*
        Ouvre un fichier et retourne son FileDescriptor.

        En cas d'erreur, FDIO_ERROR_OPEN est retourné.
*/
pika_fd fdio_open_file(const uchar *filename, fdio_mode desiredAccess, fdio_mode shareMode, fdio_mode openMode, fdio_mode attributes);


// Ferme un FileDescriptor, peut-être utilisé sur un pika_fd.
#define fdio_close(fd) CloseHandle(fd)

#else

#define FDIO_ERROR_READ -1

#define FDIO_ERROR_OPEN -1

typedef int pika_fd;

#endif



#endif