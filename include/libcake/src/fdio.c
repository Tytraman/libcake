#include "../fdio.h"

#include "../strutf16.h"
#include "../strutf8.h"

#include <stdio.h>
#include <stdlib.h>

char cake_fdio_compare_time(cake_fd fd, cake_fd compareTo, cake_byte mode) {
    #ifdef CAKE_WINDOWS
    FILETIME time1, time2;
    switch(mode) {
        case CAKE_FDIO_COMPARE_CREATION_TIME:
            GetFileTime(fd, &time1, NULL, NULL);
            GetFileTime(compareTo, &time2, NULL, NULL);
            break;
        case CAKE_FDIO_COMPARE_ACCESS_TIME:
            GetFileTime(fd, NULL, &time1, NULL);
            GetFileTime(compareTo, NULL, &time2, NULL);
            break;
        default:
            GetFileTime(fd, NULL, NULL, &time1);
            GetFileTime(compareTo, NULL, NULL, &time2);
            break;
    }
    return CompareFileTime(&time1, &time2);
    #else
    struct stat time1, time2;

    fstat(fd, &time1);
    fstat(compareTo, &time2);

    struct timespec *timex1, *timex2;

    switch(mode) {
        default:
            return CAKE_FDIO_EQUAL;
        case CAKE_FDIO_COMPARE_ACCESS_TIME:
            timex1 = &time1.st_atim;
            timex2 = &time2.st_atim;
            break;
        case CAKE_FDIO_COMPARE_LAST_WRITE_TIME:
            timex1 = &time1.st_mtim;
            timex2 = &time2.st_mtim;
            break;
    }

    if(timex1->tv_sec > timex2->tv_sec)
        return CAKE_FDIO_NEWER;
    else if(timex1->tv_sec < timex2->tv_sec)
        return CAKE_FDIO_OLDER;
    else {
        if(timex1->tv_nsec > timex2->tv_nsec)
            return CAKE_FDIO_NEWER;
        else if(timex1->tv_nsec < timex2->tv_nsec)
            return CAKE_FDIO_OLDER;
        else
            return CAKE_FDIO_EQUAL;
    }

    #endif
}

void cake_fdio_mem_copy_strutf8(Cake_String_UTF8 *dest, cake_fd fd, ushort buffSize) {
    uchar *buffer = (uchar *) malloc(buffSize * sizeof(uchar));
    cake_size bytesRead;
    while(1) {
        cake_fdio_read(fd, buffSize, bytesRead, buffer);
        if(bytesRead == CAKE_FDIO_ERROR_READ || bytesRead == 0)
            break;
        dest->bytes = (uchar *) realloc(dest->bytes, (dest->data.length + bytesRead) * sizeof(uchar) + sizeof(uchar));
        memcpy(&dest->bytes[dest->data.length], buffer, bytesRead * sizeof(uchar));
        (dest->data.length) += bytesRead;
    }
    dest->bytes[dest->data.length] = '\0';
    dest->length = cake_strutf8_length(dest);
    free(buffer);
}

void cake_fdio_mem_copy(uchar **dest, ulonglong *destLength, cake_fd fd, ushort buffSize) {
    cake_size bytesRead;
    uchar *buffer = (uchar *) malloc(buffSize * sizeof(uchar));
    *destLength = 0;
    while(cake_fdio_read(fd, buffSize, bytesRead, buffer) != CAKE_FDIO_ERROR_READ && bytesRead != 0) {
        *dest = (uchar *) realloc(*dest, (*destLength + bytesRead) * sizeof(uchar) + sizeof(uchar));
        memcpy(&(*dest)[*destLength], buffer, bytesRead * sizeof(uchar));
        (*destLength) += bytesRead;
        (*dest)[*destLength] = '\0';
    }
    free(buffer);
}

#ifdef CAKE_WINDOWS
cake_fd cake_fdio_open_file(const char *filename, ulong desiredAccess, ulong shareMode, ulong openMode, ulong attributes) {
    Cake_String_UTF16 name;
    cake_create_strutf16(&name);
    cake_char_array_to_strutf16(filename, &name);
    cake_fd fd = CreateFileW(name.characteres, desiredAccess, shareMode, NULL, openMode, attributes, NULL);
    free(name.characteres);
    return fd;
}
#endif
