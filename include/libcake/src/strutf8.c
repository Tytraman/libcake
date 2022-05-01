#include "../strutf8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>

#ifdef CAKE_WINDOWS
#include <windows.h>
#endif

void __cake_strutf8_replace(Cake_String_UTF8 *utf, const uchar *replacement, cake_byte appendMode, uchar *ptr, ulonglong *oldLength, ulonglong *replacementLength, ulonglong *diff, ulonglong *internalIndex) {
    ulonglong tempIndex = ptr - utf->bytes;
    if(appendMode == 1) {
        // On diminue la longueur
        memmove(ptr + *replacementLength, utf->bytes + *internalIndex, (utf->data.length - *internalIndex) * sizeof(uchar));
        utf->data.length -= *diff;
        utf->bytes = (uchar *) realloc(utf->bytes, utf->data.length * sizeof(uchar) + sizeof(uchar));
        utf->bytes[utf->data.length] = '\0';
    }else if(appendMode == 2) {
        // On augmente la longueur
        utf->bytes = (uchar *) realloc(utf->bytes, (utf->data.length + *diff) * sizeof(uchar) + sizeof(uchar));
        ptr = &utf->bytes[tempIndex];
        memmove(ptr + *replacementLength, ptr + *oldLength, (utf->data.length - tempIndex - *oldLength) * sizeof(uchar) + sizeof(uchar));
        utf->data.length += *diff;
        utf->bytes[utf->data.length] = '\0';
    }
    // Copie de la chaîne de remplacement vers son emplacement dans la chaîne destination
    memcpy(&utf->bytes[tempIndex], replacement, *replacementLength * sizeof(uchar));
}

cake_byte __cake_strutf8_replace_diff(ulonglong *oldLength, ulonglong *replacementLength, ulonglong *diff) {
    cake_byte appendMode = 0;
    *diff = 0;
    if(*oldLength > *replacementLength) {
        appendMode = 1;
        *diff = *oldLength - *replacementLength;
    }else if(*replacementLength > *oldLength) {
        appendMode = 2;
        *diff = *replacementLength - *oldLength;
    }
    return appendMode;
}

void cake_create_strutf8(Cake_String_UTF8 *utf) {
    utf->data.length = 0;
    utf->length = 0;
    utf->bytes = NULL;
}

Cake_String_UTF8 *cake_strutf8(const char *value) {
    Cake_String_UTF8 *utf = (Cake_String_UTF8 *) malloc(sizeof(Cake_String_UTF8));

    utf->data.length = cake_str_count(value);
    utf->bytes = (uchar *) malloc(utf->data.length * sizeof(uchar) + sizeof(uchar));
    memcpy(utf->bytes, value, utf->data.length * sizeof(uchar));
    utf->bytes[utf->data.length] = '\0';
    utf->length = cake_strutf8_length(utf);

    return utf;
}

void cake_strutf8_copy(Cake_String_UTF8 *dest, Cake_String_UTF8 *src) {
    dest->data.length = src->data.length;
    dest->length = src->length;
    if(src->data.length == 0) {
        dest->bytes = NULL;
        return;
    }
    dest->bytes = (uchar *) realloc(dest->bytes, src->data.length * sizeof(uchar) + sizeof(uchar));
    memcpy(dest->bytes, src->bytes, src->data.length * sizeof(uchar));
    dest->bytes[dest->data.length] = STR_NULL_END;
}

void cake_clear_strutf8(Cake_String_UTF8 *utf) {
    free(utf->bytes);
    cake_create_strutf8(utf);
}

ulonglong cake_strutf8_length(Cake_String_UTF8 *utf) {
    ulonglong length = 0;
    ulonglong i;
    for(i = 0; i < utf->data.length; i++) {
        if((utf->bytes[i] & 0b10000000) == 0)
            length++;
        else if((utf->bytes[i] & 0b11000000) == 192) {
            i++;
            while(i < utf->data.length && (utf->bytes[i] & 0b11000000) == 128)
                i++;
            length++;
            i--;
        }
    }
    return length;
}

void cake_strutf8_to_utf16(Cake_String_UTF8 *src, Cake_String_UTF16 *dest) {
    uchar *arrayEnd = &src->bytes[src->data.length - 1];
    uchar *pStart = src->bytes, *startEncode, *endEncode;

    while(arrayEnd > pStart && *arrayEnd == '\0')
        arrayEnd--;

    dest->length = 0;

    int bytes;
    ushort value;
    while(pStart <= arrayEnd) {
        cake_strutf8_index_by_index(pStart, arrayEnd, 0L, &startEncode, &endEncode, &bytes);
        value = cake_strutf8_decode(startEncode, bytes);
        cake_strutf16_add_char(dest, value);
        pStart = endEncode;
    }
}

void cake_strutf16_to_strutf8(Cake_String_UTF16 *src, Cake_String_UTF8 *dest) {
    dest->length = 0;
    dest->data.length = 0;
    ulonglong i;
    for(i = 0; i < src->length; i++)
        cake_strutf8_add_wchar(dest, src->characteres[i]);
    cake_strutf8_add_wchar(dest, L'\0');
}

void cake_wchar_array_to_strutf8(const wchar_t *src, Cake_String_UTF8 *dest) {
    dest->length = 0;
    dest->data.length = 0;
    ulonglong length = wcslen(src);
    ulonglong i;
    for(i = 0; i < length; ++i)
        cake_strutf8_add_wchar(dest, src[i]);
}

ulonglong cake_strutf8_index_by_index(const uchar *pArrayStart, uchar *pArrayEnd, ulonglong utfIndex, uchar **pStart, uchar **pEnd, int *bytes) {
    ulonglong internalIndex = 0;
    ulonglong currentUtfIndex = 0;
    uchar *saveStart;
    ulonglong saveInternalIndex;

    while(pArrayStart <= pArrayEnd) {
        // Si c'est un caractère ASCII
        if((*pArrayStart & 0b10000000) == 0) {
            if(currentUtfIndex == utfIndex) {
                *pStart = (uchar *) pArrayStart;
                *pEnd = (uchar *) (pArrayStart + 1);
                if(bytes) *bytes = 1;
                return internalIndex;
            }
            pArrayStart++;
            currentUtfIndex++;
            internalIndex++;
        // Si c'est le début d'un caractère encodé UTF-8
        }else if((*pArrayStart & 0b11000000) == 192) {
            saveStart = (uchar *) pArrayStart;
            saveInternalIndex = internalIndex;
            pArrayStart++;
            // Tant que c'est un octet UTF-8
            while(pArrayStart <= pArrayEnd && (*pArrayStart & 0b11000000) == 128) {
                pArrayStart++;
                internalIndex++;
            }
            if(currentUtfIndex == utfIndex) {
                *pStart = saveStart;
                *pEnd = (uchar *) pArrayStart;
                if(bytes) *bytes = pArrayStart - saveStart;
                return saveInternalIndex;
            }
            currentUtfIndex++;
            internalIndex++;
        }else {
            pArrayStart++;
        }
    }
    return internalIndex;
}

ulonglong cake_strutf8_index_by_index_reverse(Cake_String_UTF8 *utf, ulonglong utfIndex, cake_byte *bytes) {
    if(utfIndex > utf->length - 1)
        return 0;
    ulonglong internalIndex = utf->data.length - 1;
    ulonglong currentUtfIndex = utf->length - 1;
    cake_byte b;
    while(1) {
        // Si c'est un caractère ASCII
        if((utf->bytes[internalIndex] & 0x80) == 0) {
            if(currentUtfIndex == utfIndex) {
                if(bytes)
                    *bytes = 1;
                return internalIndex;
            }
            internalIndex--;
            currentUtfIndex--;
        }else {
            b = 1;
            // Tant que c'est un octet UTF-8
            while((utf->bytes[internalIndex] & 0xC0) == 0x80) {
                b++;
                if(internalIndex == 0)
                    break;
                internalIndex--;
            }
            if(currentUtfIndex == utfIndex) {
                if(bytes)
                    *bytes = b;
                return internalIndex;
            }
            if(internalIndex == 0)
                break;
            internalIndex--;
            currentUtfIndex--;
        }
    }

    return internalIndex;
}

int cake_strutf8_decode(const uchar *src, char bytes) {
    if(bytes < 1 || bytes > 6) return -1;
    if(bytes == 1) return (int) src[0];
    int i;
    uint value = 0, temp;
    uchar mask;
    char moving = 0;
    switch(bytes) {
        case 2:
            mask = 0b00011111;
            break;
        case 3:
            mask = 0b00001111;
            break;
        case 4:
            mask = 0b00000111;
            break;
        case 5:
            mask = 0b00000011;
            break;
        case 6:
            mask = 0b00000001;
            break;
    }
    for(i = bytes - 1; i > 0; i--) {
        temp = src[i] & 0b00111111;
        temp = temp << moving;
        value = value | temp;
        moving += 6;
    }
    temp = src[0] & mask;
    temp = temp << moving;
    value = value | temp;
    return value;
}

ulonglong cake_str_count(const char *str) {
    ulonglong i = 0;
    while(str[i] != '\0') i++;
    return i;
}

cake_bool cake_strutf8_add_char_array(Cake_String_UTF8 *dest, const char *str) {
    ulonglong strLength = cake_str_count(str);
    void *ptr = realloc(dest->bytes, (dest->data.length + strLength) * sizeof(uchar) + sizeof(uchar));
    if(ptr == NULL)
        return cake_false;
    dest->bytes = (uchar *) ptr;
    memcpy(dest->bytes + dest->data.length, str, strLength * sizeof(uchar));
    dest->data.length += strLength;
    dest->bytes[dest->data.length] = '\0';
    dest->length = cake_strutf8_length(dest);
    return cake_true;
}

cake_bool cake_char_array_to_strutf8(const char *src, Cake_String_UTF8 *dest) {
    ulonglong length = cake_str_count(src);
    void *ptr = realloc(dest->bytes, dest->data.length * sizeof(uchar) + sizeof(uchar));
    if(ptr == NULL)
        return cake_false;
    dest->data.length = length;
    dest->bytes = (uchar *) ptr;
    memcpy(dest->bytes, src, dest->data.length * sizeof(uchar));
    dest->bytes[dest->data.length] = '\0';
    dest->length = cake_strutf8_length(dest);
    return cake_true;
}

uchar cake_strutf8_add_wchar(Cake_String_UTF8 *dest, wchar_t value) {
    wchar_t val[2] = { value, STR_NULL_END };
    uchar preshot = cake_strutf8_wchar_array_calc_size(val);
    dest->bytes = (uchar *) realloc(dest->bytes, (dest->data.length + preshot) * sizeof(uchar) + sizeof(uchar));
    cake_strutf8_wchar_to_byte_ext(value, &dest->bytes, &dest->data.length);
    dest->bytes[dest->data.length] = '\0';
    (dest->length)++;
    return preshot;
}

void cake_strutf8_add_wchar_array(Cake_String_UTF8 *dest, const wchar_t *str) {
    size_t length = wcslen(str);
    size_t i;
    ulonglong preshotSize = cake_strutf8_wchar_array_calc_size(str);

    dest->bytes = (uchar *) realloc(dest->bytes, (dest->data.length + preshotSize) * sizeof(uchar) + sizeof(uchar));

    for(i = 0; i < length; i++)
        cake_strutf8_wchar_to_byte_ext(str[i], &dest->bytes, &dest->data.length);

    dest->bytes[dest->data.length] = '\0';
    (dest->length) += length;
}

ulonglong cake_strutf8_wchar_array_calc_size(const wchar_t *str) {
    size_t length = wcslen(str);
    size_t i;

    ulonglong size = 0;

    for(i = 0; i < length; i++) {
        if(str[i] <= 126) size += 1;
        else if(str[i] <= 2047) size += 2;
        else if(str[i] <= 65535) size += 3;
    }

    return size;
}

uchar cake_strutf8_wchar_to_byte(wchar_t value, uchar **buffer) {
    char bytes = 0;
    if(value <= 126) bytes = 1;
    else if(value <= 2047) bytes = 2;
    else if(value <= 65535) bytes = 3;
    //else if(value <= 1114111) bytes = 4;
    else return 0;
    *buffer = (uchar *) malloc(bytes * sizeof(uchar));
    if(bytes == 1) {
        (*buffer)[0] = value & 0b01111111;
    }else {
        int i;
        for(i = bytes - 1; i > 0; i--) {
            (*buffer)[i] = (value & 0b00111111) | 0b10000000;
            value = value >> 6;
        }
        switch(bytes) {
            case 2:
                (*buffer)[0] = (value & 0b00011111) | 0b11000000;
                break;
            case 3:
                (*buffer)[0] = (value & 0b00001111) | 0b11100000;
                break;
            //case 4:
            //    (*buffer)[0] = (value & 0b00000111) | 0b11110000;
            //    break;
        }
    }
    return bytes;
}

void cake_strutf8_wchar_to_byte_ext(wchar_t value, uchar **buffer, ulonglong *index) {
    char bytes = 0;
    if(value <= 126) bytes = 1;
    else if(value <= 2047) bytes = 2;
    else if(value <= 65535) bytes = 3;
    else return;
    if(bytes == 1) {
        (*buffer)[*index] = value & 0b01111111;
    }else {
        int i;
        for(i = bytes - 1; i > 0; i--) {
            (*buffer)[*index + i] = (value & 0b00111111) | 0b10000000;
            value = value >> 6;
        }
        switch(bytes) {
            case 2:
                (*buffer)[*index] = (value & 0b00011111) | 0b11000000;
                break;
            case 3:
                (*buffer)[*index] = (value & 0b00001111) | 0b11100000;
                break;
        }
    }
    (*index) += bytes;
}

uchar *cake_strutf8_search_from_end(Cake_String_UTF8 *utf, const char *research, ulonglong *internalIndex) {
    if(*internalIndex > utf->data.length)
        return NULL;
    ulonglong length = cake_str_count(research);
    if(*internalIndex > utf->data.length - length)
        *internalIndex = utf->data.length - length;
    uchar *found = NULL;

    ulonglong j, k;

    while(1) {
        if(utf->bytes[*internalIndex] == research[0]) {
            k = *internalIndex + 1;
            found = &utf->bytes[*internalIndex];
            for(j = 1; j < length; j++) {
                if(utf->bytes[k++] != research[j]) {
                    found = NULL;
                    break;
                }
            }
            if(found) {
                if(*internalIndex != 0)
                    (*internalIndex)--;
                break;
            }
        }
        if(*internalIndex == 0)
            break;
        (*internalIndex)--;
    }
    return found;
}

cake_bool cake_strutf8_end_with(Cake_String_UTF8 *utf, const char *str) {
    ulonglong length = cake_str_count(str);
    if(length > utf->data.length) return cake_false;

    ulonglong i, j = 0;
    for(i = utf->data.length - length; i < utf->data.length; i++) {
        if(utf->bytes[i] != str[j])
            return cake_false;
        j++;
    }
    return cake_true;
}

cake_bool cake_strutf8_start_with(Cake_String_UTF8 *utf, const char *research) {
    ulonglong length = cake_str_count(research);
    if(length > utf->data.length) return cake_false;

    ulonglong i;
    for(i = 0; i < length; i++)
        if(utf->bytes[i] != research[i])
            return cake_false;

    return cake_true;
}

uchar *cake_strutf8_search_from_start(Cake_String_UTF8 *utf, const char *research, ulonglong *internalIndex) {
    ulonglong length = cake_str_count(research);
    if(length > utf->data.length - *internalIndex) return NULL;

    ulonglong j = 0;

    uchar *ptr;

    for(; *internalIndex < utf->data.length; ++(*internalIndex)) {
        if(utf->bytes[*internalIndex] == research[j]) {
            if(j == 0)
                ptr = &utf->bytes[*internalIndex];
            j++;
            if(j == length) {
                ++(*internalIndex);
                return ptr;
            }
        }else
            j = 0;
    }

    return NULL;
}

uchar *cake_str_search_array(const char *str, const char *value) {
    ulonglong baseLength = cake_str_count(str);
    ulonglong length = cake_str_count(value);
    if(length > baseLength) return NULL;

    ulonglong i, j = 0;

    uchar *ptr;

    for(i = 0; i < baseLength; ++i) {
        if(str[i] == value[j]) {
            if(j == 0)
                ptr = (uchar *) &str[i];
            j++;
            if(j == length) {
                ++i;
                return ptr;
            }
        }else
            j = 0;
    }

    return NULL;
}

Cake_List_String_UTF8 *cake_strutf8_split(Cake_String_UTF8 *utf, const char *delim) {
    Cake_List_String_UTF8 *list = cake_list_strutf8();

    if(cake_str_count(delim) == 0)
        return list;
    ulonglong internalIndex = 0;
    ulonglong lastInternalIndex = internalIndex;
    uchar *ptr;
    uchar temp;

    while((ptr = cake_strutf8_search_from_start(utf, delim, &internalIndex)) != NULL) {
        temp = *ptr;
        *ptr = '\0';
        cake_list_strutf8_add_char_array(list, (const char *) &utf->bytes[lastInternalIndex]);
        *ptr = temp;
        lastInternalIndex = internalIndex;
    }
    cake_list_strutf8_add_char_array(list, (const char *) &utf->bytes[lastInternalIndex]);
    return list;
}

cake_bool cake_str_starts_with(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? cake_false : memcmp(pre, str, lenpre * sizeof(char)) == 0;
}

cake_bool cake_strutf8_remove_index(Cake_String_UTF8 *utf, ulonglong index) {
    if(utf->length < index)
        return cake_false;

    uchar *pStart = NULL;
    uchar *pEnd   = NULL;
    int bytes;
    cake_strutf8_index_by_index(utf->bytes, &utf->bytes[utf->data.length - 1], index, &pStart, &pEnd, &bytes);
    ulonglong length = &utf->bytes[utf->data.length] - pEnd;

    if(length > 0)
        memmove(pStart, pEnd, length * sizeof(uchar));
    utf->data.length -= bytes;

    void *ptr = realloc(utf->bytes, utf->data.length * sizeof(uchar) + sizeof(uchar));
    if(ptr != NULL)
        utf->bytes = (uchar *) ptr;

    utf->bytes[utf->data.length] = '\0';
    (utf->length)--;
    return cake_true;
}

uchar cake_strutf8_insert_wchar(Cake_String_UTF8 *utf, ulonglong index, wchar_t value) {
    if(index >= utf->length)
        return 0;
    
    uchar *valueUtf;
    uchar bytesNeeded = cake_strutf8_wchar_to_byte(value, &valueUtf);
    if(bytesNeeded == 0)
        return 0;

    // On augmente la taille du buffer
    void *ptr = realloc(utf->bytes, (utf->data.length + bytesNeeded) * sizeof(uchar) + sizeof(uchar));
    if(ptr == NULL) {
        free(valueUtf);
        return 0;
    }
    utf->bytes = (uchar *) ptr;

    uchar *pStart = NULL;
    uchar *pEnd   = NULL;
    int bytes;
    ulonglong internalIndex = cake_strutf8_index_by_index(utf->bytes, &utf->bytes[utf->data.length - 1], index, &pStart, &pEnd, &bytes);

    // On déplace vers la droite les données
    ulonglong length = utf->data.length - internalIndex;
    memmove(pStart + bytesNeeded, pStart, length * sizeof(uchar));

    // On copie les octets convertis
    memcpy(pStart, valueUtf, bytesNeeded * sizeof(uchar));
    
    (utf->data.length) += bytesNeeded;
    utf->bytes[utf->data.length] = '\0';
    (utf->length)++;

    free(valueUtf);
    return bytesNeeded;
}

cake_bool cake_strutf8_insert_char_array(Cake_String_UTF8 *utf, ulonglong index, const char *str) {
    if(index >= utf->length)
        return cake_false;
    
    ulonglong insertLength = cake_str_count(str);
    void *ptr = realloc(utf->bytes, (utf->data.length + insertLength) * sizeof(uchar) + sizeof(uchar));
    if(ptr == NULL)
        return cake_false;
    utf->bytes = (uchar *) ptr;

    uchar *pStart, *pEnd;
    int bytes;
    // Index interne auquel insérer la chaîne.
    ulonglong internalIndex = cake_strutf8_index_by_index(utf->bytes, &utf->bytes[utf->data.length - 1], index, &pStart, &pEnd, &bytes);
    
    // On déplace les octets vers la droite.
    memmove(utf->bytes + internalIndex + insertLength, utf->bytes + internalIndex, (utf->data.length - internalIndex) * sizeof(uchar));
    // On copie la chaîne à insérer.
    memcpy(utf->bytes + internalIndex, str, insertLength * sizeof(uchar));
    utf->data.length += insertLength;
    utf->bytes[utf->data.length] = '\0';

    utf->length = cake_strutf8_length(utf);

    return cake_true;
}

cake_bool cake_strutf8_equals(const Cake_String_UTF8 *utf, const char *compare) {
    if(cake_str_count(compare) != utf->data.length)
        return cake_false;

    return memcmp(utf->bytes, compare, utf->data.length) == 0;
}

ulonglong cake_strutf8_replace_all(Cake_String_UTF8 *utf, const char *old, const char *replacement) {
    ulonglong number            = 0;
    ulonglong oldLength         = cake_str_count(old);
    ulonglong replacementLength = cake_str_count(replacement);
    ulonglong diff              = 0;
    cake_byte appendMode = __cake_strutf8_replace_diff(&oldLength, &replacementLength, &diff);

    ulonglong internalIndex = 0;
    uchar *ptr;

    // Tant qu'on trouve une occurence
    while((ptr = cake_strutf8_search_from_start(utf, old, &internalIndex)) != NULL) {
        __cake_strutf8_replace(utf, (const uchar *) replacement, appendMode, ptr, &oldLength, &replacementLength, &diff, &internalIndex);
        number++;
    }
    if(number > 0)
        utf->length = cake_strutf8_length(utf);
    return number;
}

cake_bool cake_strutf8_replace_start(Cake_String_UTF8 *utf, const char *old, const char *replacement) {
    if(utf->data.length == 0 || utf->bytes == NULL)
        return cake_false;
    if(!cake_strutf8_start_with(utf, old))
        return cake_false;
    
    ulonglong oldLength = cake_str_count(old);
    ulonglong replacementLength = cake_str_count(replacement);
    ulonglong diff;
    cake_byte appendMode = __cake_strutf8_replace_diff(&oldLength, &replacementLength, &diff);
    uchar *ptr = utf->bytes;
    ulonglong internalIndex = oldLength;
    __cake_strutf8_replace(utf, (const uchar *) replacement, appendMode, ptr, &oldLength, &replacementLength, &diff, &internalIndex);
    utf->length = cake_strutf8_length(utf);
    return cake_true;
}

cake_bool cake_strutf8_replace_end(Cake_String_UTF8 *utf, const char *old, const char *replacement) {
    if(utf->data.length == 0 || utf->bytes == NULL)
        return cake_false;
    if(!cake_strutf8_end_with(utf, old))
        return cake_false;
    
    ulonglong oldLength = cake_str_count(old);
    ulonglong replacementLength = cake_str_count(replacement);
    ulonglong internalIndex = utf->data.length - oldLength;
    uchar *ptr = &utf->bytes[internalIndex];

    ulonglong diff;
    cake_byte appendMode = __cake_strutf8_replace_diff(&oldLength, &replacementLength, &diff);
    
    __cake_strutf8_replace(utf, (const uchar *) replacement, appendMode, ptr, &oldLength, &replacementLength, &diff, &internalIndex);
    utf->length = cake_strutf8_length(utf);
    
    return cake_true;
}

cake_bool cake_strutf8_replace_from_start(Cake_String_UTF8 *utf, const char *old, const char *replacement) {
    if(utf->data.length == 0 || utf->bytes == NULL)
        return cake_false;
    ulonglong internalIndex = 0;
    uchar *ptr = cake_strutf8_search_from_start(utf, old, &internalIndex);
    if(ptr == NULL)
        return cake_false;
    
    ulonglong oldLength = cake_str_count(old);
    ulonglong replacementLength = cake_str_count(replacement);
    ulonglong diff;
    cake_byte appendMode = __cake_strutf8_replace_diff(&oldLength, &replacementLength, &diff);
    __cake_strutf8_replace(utf, (const uchar *) replacement, appendMode, ptr, &oldLength, &replacementLength, &diff, &internalIndex);
    utf->length = cake_strutf8_length(utf);
    return cake_true;
}

cake_bool cake_strutf8_replace_from_end(Cake_String_UTF8 *utf, const char *old, const char *replacement) {
    if(utf->data.length == 0 || utf->bytes == NULL)
        return cake_false;
    ulonglong internalIndex = utf->data.length - 1;
    uchar *ptr = cake_strutf8_search_from_end(utf, old, &internalIndex);
    if(ptr == NULL)
        return cake_false;

    ulonglong oldLength = cake_str_count(old);
    internalIndex += oldLength + 1;
    ulonglong replacementLength = cake_str_count(replacement);
    ulonglong diff;
    cake_byte appendMode = __cake_strutf8_replace_diff(&oldLength, &replacementLength, &diff);
    __cake_strutf8_replace(utf, (const uchar *) replacement, appendMode, ptr, &oldLength, &replacementLength, &diff, &internalIndex);
    utf->length = cake_strutf8_length(utf);
    return cake_true;
}


ulonglong cake_strutf8_remove_all(Cake_String_UTF8 *utf, const char *value) {
    ulonglong number = 0;
    ulonglong length = cake_str_count(value);

    ulonglong internalIndex = 0;
    void *p;
    uchar *ptr;
    while((ptr = cake_strutf8_search_from_start(utf, value, &internalIndex)) != NULL) {
        memmove(ptr, utf->bytes + internalIndex, (utf->data.length - internalIndex) * sizeof(uchar));
        internalIndex -= length;
        number++;
    }
    // Si des choses ont été supprimées de la chaîne, on redimenssionne le buffer
    if(number > 0) {
        utf->data.length -= length * number;
        utf->bytes[utf->data.length] = '\0';
        p = realloc(utf->bytes, utf->data.length * sizeof(uchar) + sizeof(uchar));
        if(p != NULL)
            utf->bytes = (uchar *) p;
        utf->length = cake_strutf8_length(utf);
    }

    return number;
}

cake_bool cake_strutf8_remove_start(Cake_String_UTF8 *utf, const char *value) {
    if(!cake_strutf8_start_with(utf, value))
        return cake_false;
    ulonglong length = cake_str_count(value);

    utf->data.length -= length;
    memmove(utf->bytes, utf->bytes + length, utf->data.length * sizeof(uchar));
    void *ptr = realloc(utf->bytes, utf->data.length * sizeof(uchar) + sizeof(uchar));
    if(ptr != NULL)
        utf->bytes = (uchar *) ptr;
    utf->bytes[utf->data.length] = '\0';

    utf->length = cake_strutf8_length(utf);

    return cake_true;
}

void cake_free_strutf8(Cake_String_UTF8 *utf) {
    free(utf->bytes);
    free(utf);
}

void cake_strutf8_reverse(Cake_String_UTF8 *utf) {
    uchar *buffer = (uchar *) malloc(utf->data.length * sizeof(uchar) + sizeof(uchar));
    buffer[utf->data.length] = '\0';

    ulonglong index, internalIndexReverse = utf->data.length - 1;
    int bytes;
    uchar *pStart, *pEnd;
    for(index = 0; index < utf->length; ++index) {
        cake_strutf8_index_by_index(utf->bytes, &utf->bytes[utf->data.length - 1], index, &pStart, &pEnd, &bytes);

        while(pStart < pEnd) {
            buffer[internalIndexReverse - (pEnd - pStart - 1)] = *pStart;
            pStart++;
        }

        internalIndexReverse -= bytes;
    }

    free(utf->bytes);
    utf->bytes = buffer;
}

Cake_List_String_UTF8 *cake_list_strutf8() {
    Cake_List_String_UTF8 *list = (Cake_List_String_UTF8 *) malloc(sizeof(*list));
    list->data.length = 0;
    list->list = NULL;
    return list;
}

cake_bool cake_list_strutf8_add_char_array(Cake_List_String_UTF8 *list, const char *str) {
    void *ptr = realloc(list->list, (list->data.length + 1) * sizeof(*list->list));
    if(ptr == NULL)
        return cake_false;
    list->list = (Cake_String_UTF8 **) ptr;
    (list->data.length)++;
    list->list[list->data.length - 1] = cake_strutf8(str);
    return cake_true;
}

void cake_free_list_strutf8(Cake_List_String_UTF8 *list) {
    ulonglong i;
    for(i = 0; i < list->data.length; ++i)
        cake_free_strutf8(list->list[i]);
    free(list->list);
    free(list);
}

ulonglong cake_str_search(const char *str, char value, uchar **ptr) {
    ulonglong i;

    for(i = 0; i < cake_str_count(str); ++i)
        if(str[i] == value) {
            *ptr = (uchar *) &str[i];
            break;
        }

    return i;
}

void cake_strutf8_to_lower(Cake_String_UTF8 *utf) {
    ulonglong i;
    int bytes;
    uchar *start, *end;
    int code;
    ulonglong tempInd;
    for(i = 0; i < utf->length; ++i) {
        cake_strutf8_index_by_index(utf->bytes, &utf->bytes[utf->data.length - 1], i, &start, &end, &bytes);
        code = cake_strutf8_decode(start, bytes);
        // La liste des caractères pris en charge recouvrent les "principaux caractères"
        switch(bytes) {
            default: goto strutf8_to_lower_ignore;
            case 1:
                if(code > 64 && code < 91)
                    code += 32;
                break;
            case 2:
                if(
                    (code > 191 && code < 215) ||
                    (code > 215 && code < 223)
                )
                    code += 32;
                else if(code > 255 && code < 311 && ((code & 1) == 0))
                    code++;
                else if(code > 312 && code < 328 && (code & 1))
                    code++;
                else if(code > 329 && code < 375 && ((code & 1) == 0))
                    code++;
                else if(code > 376 && code < 382 && code & 1)
                    code++;
                else {
                    switch(code) {
                        default: break;
                        case 376:
                            code = 255;
                            break;
                    }
                }
                break;
        }
        tempInd = 0;
        cake_strutf8_wchar_to_byte_ext(code, &start, &tempInd);
strutf8_to_lower_ignore: ;
    }
}

ulonglong cake_strutf8_to_ulonglong(Cake_String_UTF8 *utf) {
    ulonglong value = 0;
    ulonglong pow = 1;
    ulonglong i = utf->length - 1;
    ulonglong internalIndex;
    cake_byte bytes;
    int code;
    while(1) {
        internalIndex = cake_strutf8_index_by_index_reverse(utf, i, &bytes);
        code = cake_strutf8_decode(&utf->bytes[internalIndex], bytes);
        if(code >= '0' && code <= '9') {
            value += (code - '0') * pow;
            pow *= 10;
        }
        if(i == 0)
            break;
        i--;
    }


    return value;
}

void cake_ulonglong_to_char_array(ulonglong value, char *buffer) {
    if(value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    cake_byte length = 0;
    ulonglong copy = value;
    while(copy != 0) {
        copy /= 10;
        length++;
    }
    buffer[length] = '\0';
    while(value != 0) {
        buffer[length - 1] = value % 10 + '0';
        value /= 10;
        length--;
    }
}

char *cake_ulonglong_to_char_array_dyn(ulonglong value) {
    char *buffer = NULL;
    if(value == 0) {
        buffer = (char *) malloc(2 * sizeof(char));
        buffer[0] = '0';
        buffer[1] = '\0';
        return buffer;
    }
    uchar length = 0;
    ulonglong copy = value;
    while(copy != 0) {
        copy /= 10;
        length++;
    }
    buffer = (char *) malloc(length * sizeof(char) + sizeof(char));
    buffer[length] = '\0';
    while(value != 0) {
        buffer[length - 1] = value % 10 + '0';
        value /= 10;
        length--;
    }
    return buffer;
}

Cake_String_UTF8_Pair *cake_strutf8_pair(const char *key, const char *value) {
    Cake_String_UTF8_Pair *pair = (Cake_String_UTF8_Pair *) malloc(sizeof(Cake_String_UTF8_Pair));
    pair->key   = cake_strutf8(key);
    pair->value = cake_strutf8(value);
    return pair;
}

void cake_free_strutf8_pair(Cake_String_UTF8_Pair *pair) {
    cake_free_strutf8(pair->key);
    cake_free_strutf8(pair->value);
    free(pair);
}

uint cake_str_hex_to_uint(const uchar *str) {
    uint value = 0;
    const uchar *c = str;
    if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
        c += 2;

    while(*c != '\0') {
        if(*c >= '0' && *c <= '9') {
            value <<= 4;
            value += *c - '0';
        }else if((*c >= 'A' && *c <= 'F') || (*c >= 'a' && *c <= 'f')) {
            value <<= 4;
            value += (*c & 0x07) + 9;
        }
        c++;
    }

    return value;
}

// TODO: refaire cake_strutf8_decode_url
void cake_strutf8_decode_url(Cake_String_UTF8 *utf) {
    cake_strutf8_replace_all(utf, "+", " ");
    uchar *begin   = utf->bytes;
    uchar *current = utf->bytes;
    uchar recovery;
    while(*current != '\0') {
        if(*current == '%') {
            begin = current;
            while(1) {
                if((current + 3) > &utf->bytes[utf->data.length - 1]) {
                    current = &utf->bytes[utf->data.length];
                    break;
                }else {
                    current += 3;
                    if(*current != '%')
                        break;
                }
            }
            recovery = *current;
            *current = '\0';
            uint dec = cake_str_hex_to_uint(begin);
            *current = recovery;
            uchar *buffer;
            uchar bytes = cake_strutf8_dec_to_char(dec, &buffer);
            char diff = bytes - (current - begin);
            ulonglong saveIndex = begin - utf->bytes + bytes;
            // On copie le caractère converti
            memcpy(begin, buffer, bytes * sizeof(uchar));

            // On déplace le reste vers la gauche
            memcpy(begin + bytes, current, (&utf->bytes[utf->data.length] - current) * sizeof(uchar));
            utf->data.length += diff;
            utf->bytes = (uchar *) realloc(utf->bytes, utf->data.length * sizeof(uchar) + sizeof(uchar));
            utf->bytes[utf->data.length] = '\0';
            current = &utf->bytes[saveIndex];
            free(buffer);
        }
        current++;
        if(current > &utf->bytes[utf->data.length - 1])
            break;
    }
    utf->length = cake_strutf8_length(utf);
}

uchar cake_strutf8_dec_to_char(uint value, uchar **buffer) {
    uchar bytes;
    if(value < 256) bytes = 1;
    else if(value < 65536) bytes = 2;
    else bytes = 3;
    *buffer = (uchar *) malloc(bytes * sizeof(uchar));

    uint mask = 0xFF, reverse = 0;
    uchar i;
    for(i = 0; i < bytes; ++i) {
        (*buffer)[bytes - 1 - i] = ((value & mask) >> reverse);
        mask <<= 8;
        reverse += 8;
    }

    return bytes;
}

void cake_str_dec_to_hexchar(uchar value, uchar dest[2]) {
    uchar val1 = (value & 0x0F);
    uchar val2 = ((value & 0xF0) >> 4);
    if(val1 > 9)
        dest[1] = val1 + 'a' - 10;
    else
        dest[1] = val1 + '0';
    if(val2 > 9)
        dest[0] = val2 + 'a' - 10;
    else
        dest[0] = val2 + '0';
}

void __strutf8_remove_from_to_internal(Cake_String_UTF8 *utf, ulonglong fromIndex, ulonglong toIndex) {
    ulonglong diff = toIndex - fromIndex;
    memmove(utf->bytes + fromIndex, utf->bytes + toIndex, ((utf->bytes + utf->data.length) - (utf->bytes + toIndex)) * sizeof(uchar));
    (utf->data.length) -= diff;
    void *ptr = realloc(utf->bytes, utf->data.length * sizeof(uchar) + sizeof(uchar));
    if(ptr != NULL)
        utf->bytes = (uchar *) ptr;
    utf->bytes[utf->data.length] = '\0';

    utf->length = cake_strutf8_length(utf);
}

cake_bool cake_strutf8_remove_from_to(Cake_String_UTF8 *utf, ulonglong fromIndex, ulonglong toIndex) {
    if(fromIndex > utf->length || toIndex > utf->length || fromIndex >= toIndex)
        return cake_false;

    uchar *pFromStart, *pFromEnd;
    uchar *pToStart, *pToEnd;
    ulonglong internalFromIndex = cake_strutf8_index_by_index(utf->bytes, &utf->bytes[utf->data.length - 1], fromIndex, &pFromStart, &pFromEnd, NULL);
    ulonglong internalToIndex   = cake_strutf8_index_by_index(utf->bytes, &utf->bytes[utf->data.length - 1], toIndex, &pToStart, &pToEnd, NULL);
    
    __strutf8_remove_from_to_internal(utf, internalFromIndex, internalToIndex);
    return cake_true;
}

cake_bool cake_strutf8_remove_from_to_internal(Cake_String_UTF8 *utf, ulonglong fromIndex, ulonglong toIndex) {
    if(fromIndex > utf->data.length || toIndex > utf->data.length || fromIndex >= toIndex)
        return cake_false;

    __strutf8_remove_from_to_internal(utf, fromIndex, toIndex);
    return cake_true;
}


cake_bool cake_str_equals(const char *str1, const char *str2) {
    ulonglong length1 = cake_str_count(str1);
    ulonglong length2 = cake_str_count(str2);
    if(length1 != length2)
        return cake_false;

    return memcmp(str1, str2, length1) == 0;
}

ulonglong cake_str_number_of(const char *str, char value) {
    ulonglong number = 0;
    while(*str != '\0') {
        if(*str == value)
            number++;
        str++;
    }
    return number;
}

void cake_strutf8_vector_delete_callback(void *args) {
    Cake_String_UTF8 *utf = (Cake_String_UTF8 *) args;
    free(utf->bytes);
}

void cake_strutf8_vector_delete_callback_ptr(void *args) {
    Cake_String_UTF8 *utf = *((Cake_String_UTF8 **) args);
    cake_free_strutf8(utf);
}

Cake_String_UTF8 *cake_strutf8_substring(Cake_String_UTF8 *from, ulonglong startIndex, ulonglong endIndex) {
    if(from->data.length == 0 || from->bytes == NULL || endIndex > from->length || startIndex >= endIndex)
        return NULL;
    uchar *pStartStart, *pStartEnd;
    uchar *pEndStart, *pEndEnd;
    int bytes;
    cake_strutf8_index_by_index(from->bytes, &from->bytes[from->data.length - 1], startIndex, &pStartStart, &pStartEnd, &bytes);
    cake_strutf8_index_by_index(from->bytes, &from->bytes[from->data.length - 1], endIndex, &pEndStart, &pEndEnd, &bytes);

    uchar temp = *pEndStart;
    *pEndStart = '\0';
    Cake_String_UTF8 *utf = cake_strutf8((const char *) pStartStart);
    *pEndStart = temp;
    return utf;
}

cake_bool cake_str_contains(const char *str, char value) {
    while(*str != '\0')
        if(*str++ == value)
            return cake_true;
    return cake_false;
}

cake_bool cake_list_strutf8_remove(Cake_List_String_UTF8 *list, ulonglong index) {
    if(index >= list->data.length)
        return cake_false;
    if(list->data.length > 1) {
        cake_free_strutf8(list->list[index]);
        (list->data.length)--;
        memmove(list->list + index, list->list + index + 1, (list->data.length - index) * sizeof(*list->list));
        void *ptr = realloc(list->list, list->data.length * sizeof(*list->list));
        if(ptr != NULL)
            list->list = (Cake_String_UTF8 **) ptr;
    }else {
        cake_free_strutf8(list->list[0]);
        free(list->list);
        list->list = NULL;
        list->data.length = 0;
    }
    return cake_true;
}

cake_bool cake_list_strutf8_insert(Cake_List_String_UTF8 *list, ulonglong index, const char *str) {
    if(index > list->data.length)
        return cake_false;

    void *ptr = realloc(list->list, (list->data.length + 1) * sizeof(*list->list));
    if(ptr == NULL)
        return cake_false;
    list->list = (Cake_String_UTF8 **) ptr;
    memmove(list->list + index + 1, list->list + index, (list->data.length - index) * sizeof(*list->list));
    (list->data.length)++;
    list->list[index] = cake_strutf8(str);

    return cake_true;
}
