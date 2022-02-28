#include "../../pikapi.h"

#ifdef PIKA_WINDOWS
pika_byte change_directory(const uchar *directory) {
    String_UTF16 name;
    create_strutf16(&name);
    char_array_to_strutf16(directory, &name);
    pika_bool ret = SetCurrentDirectoryW(name.characteres);
    free(name.characteres);
    return ret;
}
#endif