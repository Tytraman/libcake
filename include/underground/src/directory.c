#include "../directory.h"
#include "../utf16.h"

#ifdef CAKE_WINDOWS
cake_byte cake_change_directory(const uchar *directory) {
    Cake_String_UTF16 name;
    cake_create_strutf16(&name);
    cake_char_array_to_strutf16(directory, &name);
    cake_bool ret = SetCurrentDirectoryW(name.characteres);
    free(name.characteres);
    return ret;
}
#else

#endif