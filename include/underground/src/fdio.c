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