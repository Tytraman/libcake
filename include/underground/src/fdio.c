#include "../fdio.h"

#include "../utf16.h"
#include "../utf8.h"

#include <stdlib.h>

char fdio_compare_time(pika_fd fd, pika_fd compareTo, pika_byte mode) {
    #ifdef PIKA_WINDOWS
    FILETIME time1, time2;
    switch(mode) {
        case FDIO_COMPARE_CREATION_TIME:
            GetFileTime(fd, &time1, NULL, NULL);
            GetFileTime(compareTo, &time2, NULL, NULL);
            break;
        case FDIO_COMPARE_ACCESS_TIME:
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
            return FDIO_EQUAL;
        case FDIO_COMPARE_ACCESS_TIME:
            timex1 = &time1.st_atim;
            timex2 = &time2.st_atim;
            break;
        case FDIO_COMPARE_LAST_WRITE_TIME:
            timex1 = &time1.st_mtim;
            timex2 = &time2.st_mtim;
            break;
    }

    if(timex1->tv_sec > timex2->tv_sec)
        return FDIO_NEWER;
    else if(timex1->tv_sec < timex2->tv_sec)
        return FDIO_OLDER;
    else {
        if(timex1->tv_nsec > timex2->tv_nsec)
            return FDIO_NEWER;
        else if(timex1->tv_nsec < timex2->tv_nsec)
            return FDIO_OLDER;
        else
            return FDIO_EQUAL;
    }

    #endif
}

void fdio_mem_copy_strutf8(String_UTF8 *dest, pika_fd fd, ushort buffSize) {
    pika_size bytesRead;
    uchar *buffer = (uchar *) malloc(buffSize * sizeof(uchar));

    while(fdio_read(fd, buffSize, bytesRead, buffer) != FDIO_ERROR_READ && bytesRead != 0) {
        dest->bytes = (uchar *) realloc(dest->bytes, (dest->data.length + bytesRead) * sizeof(uchar) + sizeof(uchar));
        memcpy(&dest->bytes[dest->data.length], buffer, bytesRead * sizeof(uchar));
        dest->data.length += bytesRead;
        dest->bytes[dest->data.length] = '\0';
    }
    dest->length = strutf8_length(dest);

    free(buffer);
}

void fdio_mem_copy(uchar **dest, ulonglong *destLength, pika_fd fd, ushort buffSize) {
    pika_size bytesRead;
    uchar *buffer = (uchar *) malloc(buffSize * sizeof(uchar));
    *destLength = 0;
    while(fdio_read(fd, buffSize, bytesRead, buffer) != FDIO_ERROR_READ && bytesRead != 0) {
        *dest = (uchar *) realloc(*dest, (*destLength + bytesRead) * sizeof(uchar) + sizeof(uchar));
        memcpy(&(*dest)[*destLength], buffer, bytesRead * sizeof(uchar));
        (*destLength) += bytesRead;
        (*dest)[*destLength] = '\0';
    }
    free(buffer);
}
