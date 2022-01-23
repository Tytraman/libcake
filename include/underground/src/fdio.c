#include "../fdio.h"

#include "../utf16.h"
#include "../utf8.h"

#include <stdlib.h>

#ifdef PIKA_WINDOWS
pika_fd fdio_open_file(const uchar *filename, fdio_mode desiredAccess, fdio_mode shareMode, fdio_mode openMode, fdio_mode attributes) {
    String_UTF8 *utf8 = strutf8(filename);
    String_UTF16 utf16;
    create_strutf16(&utf16);
    strutf8_to_utf16(utf8, &utf16);
    free_strutf8(utf8);

    pika_fd fd = CreateFileW(
        utf16.characteres,
        desiredAccess,
        shareMode,
        NULL,
        openMode,
        attributes,
        NULL
    );

    free(utf16.characteres);

    return fd;
}
#endif

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