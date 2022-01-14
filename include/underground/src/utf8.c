#include "../utf8.h"
#include "../memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifdef PIKA_WINDOWS
#include <windows.h>
#endif

void create_strutf8(String_UTF8 *utf) {
    utf->data.length = 0;
    utf->data.byteSize = sizeof(unsigned char);
    utf->length = 0;
    utf->bytes = NULL;
}

void strutf8_copy(String_UTF8 *dest, String_UTF8 *src) {
    dest->data.length = src->data.length;
    dest->data.byteSize = src->data.byteSize;
    dest->length = src->length;
    if(src->data.length == 0) {
        dest->bytes = NULL;
        return;
    }
    dest->bytes = (unsigned char *) malloc(src->data.length * src->data.byteSize + src->data.byteSize);
    memcpy(dest->bytes, src->bytes, src->data.length * src->data.byteSize);
    dest->bytes[dest->data.length] = STR_NULL_END;
}

void clear_strutf8(String_UTF8 *utf) {
    free(utf->bytes);
    create_strutf8(utf);
}

unsigned long long strutf8_length(String_UTF8 *utf) {
    unsigned long long length = 0UL;
    unsigned long long i;
    for(i = 0UL; i < utf->data.length; i++) {
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
    if(utf->bytes[utf->data.length - 1] == '\0')
        length--;
    return length;
}

void strutf8_to_utf16(String_UTF8 *src, String_UTF16 *dest) {
    unsigned char *arrayEnd = &src->bytes[src->data.length - 1];
    unsigned char *pStart = src->bytes, *startEncode, *endEncode;

    while(arrayEnd > pStart && *arrayEnd == '\0')
        arrayEnd--;

    clear_strutf16(dest);
    int bytes;
    unsigned short value;
    while(pStart <= arrayEnd) {
        strutf8_index_by_index(pStart, arrayEnd, 0L, &startEncode, &endEncode, &bytes);
        value = strutf8_decode(startEncode, bytes);
        strutf16_add_char(dest, value);
        pStart = endEncode;
    }
}

void strutf16_to_strutf8(String_UTF16 *src, String_UTF8 *dest) {
    create_strutf8(dest);
    unsigned long long i;
    for(i = 0; i < src->length; i++)
        strutf8_add_wchar(dest, src->characteres[i]);
    strutf8_add_wchar(dest, L'\0');
}

void wchar_array_to_strutf8(const wchar_t *src, String_UTF8 *dest) {
    create_strutf8(dest);
    unsigned long long length = wcslen(src);
    unsigned long long i;
    for(i = 0; i < length; ++i)
        strutf8_add_wchar(dest, src[i]);
}

ulonglong strutf8_index_by_index(unsigned char *pArrayStart, unsigned char *pArrayEnd, unsigned long long utfIndex, unsigned char **pStart, unsigned char **pEnd, int *bytes) {
    ulonglong internalIndex = 0;
    ulonglong currentUtfIndex = 0UL;
    while(pArrayStart <= pArrayEnd) {
        // Si c'est un caractère ASCII
        if((*pArrayStart & 0b10000000) == 0) {
            if(currentUtfIndex == utfIndex) {
                *pStart = pArrayStart;
                *pEnd = pArrayStart + 1;
                if(bytes) *bytes = 1;
                return internalIndex;
            }
            pArrayStart++;
            currentUtfIndex++;
            internalIndex++;
        // Si c'est le début d'un caractère encodé UTF-8
        }else if((*pArrayStart & 0b11000000) == 192) {
            uchar *saveStart = pArrayStart;
            ulonglong saveInternalIndex = internalIndex;
            pArrayStart++;
            // Tant que c'est un octet UTF-8
            while(pArrayStart <= pArrayEnd && (*pArrayStart & 0b11000000) == 128) {
                pArrayStart++;
                internalIndex++;
            }
            if(currentUtfIndex == utfIndex) {
                *pStart = saveStart;
                *pEnd = pArrayStart;
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

int strutf8_decode(const unsigned char *src, char bytes) {
    if(bytes < 1 || bytes > 6) return -1;
    if(bytes == 1) return (int) src[0];
    int i, j;
    unsigned int value = 0, temp;
    unsigned char mask;
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

unsigned long long str_count(char *str) {
    unsigned long long i = 0;
    while(str[i] != '\0') i++;
    return i;
}

void strutf8_add_array_char(String_UTF8 *dest, char *str) {
    unsigned long long strLength = str_count(str);
    dest->bytes = (unsigned char *) realloc(dest->bytes, (dest->data.length + strLength) * dest->data.byteSize + dest->data.byteSize);
    memcpy(&dest->bytes[dest->data.length], str, strLength * dest->data.byteSize);
    dest->data.length += strLength;
    dest->bytes[dest->data.length] = '\0';
    dest->length = strutf8_length(dest);
}

void array_char_to_strutf8(char *src, String_UTF8 *dest) {
    dest->data.length = str_count(src);
    free(dest->bytes);
    dest->bytes = (unsigned char *) malloc(dest->data.length * dest->data.byteSize + dest->data.byteSize);
    memcpy(dest->bytes, src, dest->data.length * dest->data.byteSize);
    dest->bytes[dest->data.length] = '\0';
    dest->length = strutf8_length(dest);
}

int8 strutf8_add_wchar(String_UTF8 *dest, wchar_t value) {
    wchar_t val[2] = { value, STR_NULL_END };
    int8 preshot = strutf8_wchar_array_calc_size(val);
    dest->bytes = (uchar *) realloc(dest->bytes, (dest->data.length + preshot) * dest->data.byteSize + dest->data.byteSize);
    strutf8_wchar_to_byte_ext(value, &dest->bytes, &dest->data.length);
    dest->bytes[dest->data.length] = '\0';
    (dest->length)++;
    return preshot;
}

char strutf8_add_wchar_array(String_UTF8 *dest, wchar_t *str) {
    size_t length = wcslen(str);
    size_t i;
    unsigned long long preshotSize = strutf8_wchar_array_calc_size(str);

    dest->bytes = (unsigned char *) realloc(dest->bytes, (dest->data.length + preshotSize) * dest->data.byteSize + dest->data.byteSize);

    for(i = 0; i < length; i++)
        strutf8_wchar_to_byte_ext(str[i], &dest->bytes, &dest->data.length);

    dest->bytes[dest->data.length] = '\0';
    (dest->length) += length;
}

void strutf8_add_char(String_UTF8 *dest, char value, pika_bool increaseLength) {
    (dest->data.length)++;
    dest->bytes = (unsigned char *) realloc(dest->bytes, dest->data.length * dest->data.byteSize + dest->data.byteSize);
    dest->bytes[dest->data.length - 1] = value;
    dest->bytes[dest->data.length ] = '\0';
    if(increaseLength)
        (dest->length)++;
}

unsigned long long strutf8_wchar_array_calc_size(wchar_t *str) {
    size_t length = wcslen(str);
    size_t i;

    unsigned long long size = 0;

    for(i = 0; i < length; i++) {
        if(str[i] <= 126) size += 1;
        else if(str[i] <= 2047) size += 2;
        else if(str[i] <= 65535) size += 3;
    }

    return size;
}

char strutf8_wchar_to_byte(wchar_t value, unsigned char **buffer) {
    char bytes = 0;
    if(value <= 126) bytes = 1;
    else if(value <= 2047) bytes = 2;
    else if(value <= 65535) bytes = 3;
    //else if(value <= 1114111) bytes = 4;
    else return -1;
    *buffer = malloc(bytes * sizeof(unsigned char));
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

void strutf8_wchar_to_byte_ext(wchar_t value, unsigned char **buffer, unsigned long long *index) {
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

unsigned char *strutf8_search_from_end(String_UTF8 *utf, unsigned char *research) {
    size_t length = str_count(research);
    unsigned char *found = NULL;

    unsigned long long i, j, k;
    for(i = utf->data.length - length; ; i--) {
        if(utf->bytes[i] == research[0]) {
            k = i + 1;
            found = &utf->bytes[i];
            for(j = 1; j < length; j++) {
                if(utf->bytes[k++] != research[j]) {
                    found = NULL;
                    break;
                }
            }
            if(found)
                break;
        }
        if(i == 0)
            break;
    }

    return found;
}

pika_bool strutf8_end_with(String_UTF8 *utf, unsigned char *str) {
    unsigned long long length = str_count(str);
    if(length > utf->data.length) return pika_false;

    unsigned long long i, j = 0;
    for(i = utf->data.length - length; i < utf->data.length; i++) {
        if(utf->bytes[i] != str[j])
            return pika_false;
        j++;
    }
    return pika_true;
}

pika_bool strutf8_start_with(String_UTF8 *utf, unsigned char *research) {
    unsigned long long length = str_count(research);
    if(length > utf->data.length) return pika_false;

    unsigned long long i;
    for(i = 0; i < length; i++)
        if(utf->bytes[i] != research[i])
            return pika_false;

    return pika_true;
}

uchar *strutf8_search(String_UTF8 *utf, uchar *research, ulonglong *internalIndex) {
    unsigned long long length = str_count(research);
    if(length > utf->data.length - *internalIndex) return NULL;

    unsigned long long j = 0;

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

unsigned long long strutf8_split_ptr(String_UTF8 *utf, String_UTF8 **dest, unsigned char *delim) {
    *dest = NULL;
    unsigned long long length = 0;
    unsigned long long internalIndex = 0;
    unsigned long long lastInternalIndex = internalIndex;
    unsigned char *ptr;
    while((ptr = strutf8_search(utf, delim, &internalIndex)) != NULL) {
        *dest = (String_UTF8 *) realloc(*dest, (length + 1) * sizeof(String_UTF8));
        (*dest)[length].bytes = ptr;
        (*dest)[length].data.byteSize = utf->data.byteSize;
        (*dest)[length].data.length = internalIndex - lastInternalIndex;
        (*dest)[length].length = strutf8_length(&(*dest)[length]);
        ++length;
    }
    return length;
}

pika_bool str_starts_with(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? pika_false : memcmp(pre, str, lenpre * sizeof(char)) == 0;
}

pika_bool strutf8_remove_index(String_UTF8 *utf, ulonglong index) {
    if(utf->length < index)
        return pika_false;

    uchar *pStart = NULL;
    uchar *pEnd   = NULL;
    int bytes;
    strutf8_index_by_index(utf->bytes, &utf->bytes[utf->data.length - 1], index, &pStart, &pEnd, &bytes);
    ulonglong length = &utf->bytes[utf->data.length] - pEnd;

    if(length > 0)
        memcpy(pStart, pEnd, length * utf->data.byteSize);
    (utf->data.length) -= bytes;
    utf->bytes = (uchar *) realloc(utf->bytes, utf->data.length * utf->data.byteSize + utf->data.byteSize);
    utf->bytes[utf->data.length] = '\0';
    (utf->length)--;
    return pika_true;
}

int8 strutf8_insert_wchar(String_UTF8 *utf, ulonglong index, wchar_t value) {
    if(index > utf->length)
        return -1;
    
    uchar *valueUtf;
    char bytesNeeded = strutf8_wchar_to_byte(value, &valueUtf);
    if(bytesNeeded == -1)
        return -1;

    // On augmente la taille du buffer
    utf->bytes = (uchar *) realloc(utf->bytes, (utf->data.length + bytesNeeded) * utf->data.byteSize + utf->data.byteSize);

    uchar *pStart = NULL;
    uchar *pEnd   = NULL;
    int bytes;
    ulonglong internalIndex = strutf8_index_by_index(utf->bytes, &utf->bytes[utf->data.length - 1], index, &pStart, &pEnd, &bytes);

    // On déplace vers la droite les données
    ulonglong length = utf->data.length - internalIndex;
    memcpy(pStart + bytesNeeded, pStart, length * utf->data.byteSize);

    // On copie les octets convertis
    memcpy(pStart, valueUtf, bytesNeeded * utf->data.byteSize);
    
    (utf->data.length) += bytesNeeded;
    utf->bytes[utf->data.length] = '\0';
    (utf->length)++;

    free(valueUtf);
    return bytesNeeded;
}

void strutf8_insert_char(String_UTF8 *utf, ulonglong index, char value) {
    if(index > utf->length)
        return;

    // On augmente la taille du buffer
    utf->bytes = (uchar *) realloc(utf->bytes, (utf->data.length + 1) * utf->data.byteSize + utf->data.byteSize);

    uchar *pStart = NULL;
    uchar *pEnd   = NULL;
    int bytes;
    ulonglong internalIndex = strutf8_index_by_index(utf->bytes, &utf->bytes[utf->data.length - 1], index, &pStart, &pEnd, &bytes);

    // On déplace vers la droite les données
    ulonglong length = utf->data.length - internalIndex;
    memcpy(pStart + 1, pStart, length * utf->data.byteSize);

    *pStart = value;

    (utf->data.length)++;
    utf->bytes[utf->data.length] = '\0';

    (utf->length)++;
}

void strutf8_insert_char_internal(String_UTF8 *utf, ulonglong internalIndex, char value, pika_bool increaseLength) {
    if(internalIndex > utf->data.length)
        return;

    // On augmente la taille du buffer
    utf->bytes = (uchar *) realloc(utf->bytes, (utf->data.length + 1) * utf->data.byteSize + utf->data.byteSize);

    // On déplace vers la droite les données
    ulonglong length = utf->data.length - internalIndex;
    memcpy(&utf->bytes[internalIndex] + 1, &utf->bytes[internalIndex], length * utf->data.byteSize);

    utf->bytes[internalIndex] = value;

    (utf->data.length)++;
    utf->bytes[utf->data.length] = '\0';

    if(increaseLength)
        (utf->length)++;
}

pika_bool strutf8_equals(String_UTF8 *utf, char *compare) {
    if(str_count(compare) != utf->data.length)
        return pika_false;

    return memcmp(utf->bytes, compare, utf->data.length) == 0;
}
