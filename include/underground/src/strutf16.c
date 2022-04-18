#include "../strutf16.h"
#include "../strutf8.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <wchar.h>

void cake_create_strutf16(Cake_String_UTF16 *utf) {
    utf->length = 0;
    utf->characteres = NULL;
}

Cake_String_UTF16 *cake_strutf16(const wchar_t *value) {
    Cake_String_UTF16 *utf = (Cake_String_UTF16 *) malloc(sizeof(Cake_String_UTF16));

    utf->length = cake_wstr_count(value);
    utf->characteres = (wchar_t *) malloc(utf->length * sizeof(wchar_t) + sizeof(wchar_t));
    memcpy(utf->characteres, value, utf->length * sizeof(wchar_t));
    utf->characteres[utf->length] = L'\0';

    return utf;
}

void cake_clear_strutf16(Cake_String_UTF16 *utf) {
    free(utf->characteres);
    cake_create_strutf16(utf);
}

void cake_strutf16_set_value(Cake_String_UTF16 *utf, wchar_t *str) {
    utf->length = wcslen(str);
    free(utf->characteres);
    utf->characteres = (wchar_t *) malloc(utf->length * sizeof(wchar_t) + sizeof(wchar_t));
    memcpy(utf->characteres, str, utf->length * sizeof(wchar_t));
    utf->characteres[utf->length] = L'\0';
}

void cake_strutf16_add_wchar_array(Cake_String_UTF16 *utf, const wchar_t *str) {
    ulonglong lastLength = utf->length;
    ulonglong strLength = cake_wstr_count(str);
    utf->length += strLength;
    utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t) + sizeof(wchar_t));
    memcpy(&utf->characteres[lastLength], str, strLength * sizeof(wchar_t));
    utf->characteres[utf->length] = L'\0';
}

void cake_strutf16_add_char(Cake_String_UTF16 *utf, wchar_t c) {
    (utf->length)++;
    utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t) + sizeof(wchar_t));
    memcpy(&utf->characteres[utf->length - 1], &c, sizeof(wchar_t));
    utf->characteres[utf->length] = L'\0';
}

void cake_strutf16_empty(Cake_String_UTF16 *utf) {
    utf->length = 0;
    free(utf->characteres);
    utf->characteres = (wchar_t *) malloc(sizeof(wchar_t));
    utf->characteres[0] = L'\0';
}

void cake_strutf16_add_bytes(Cake_String_UTF16 *utf, unsigned char *bytes, unsigned long long size) {
    unsigned long long lastLength = utf->length;
    utf->length += size;
    utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t));
    memcpy(&utf->characteres[lastLength], bytes, size * sizeof(wchar_t));
}

unsigned long long cake_strutf16_replace_all_char(Cake_String_UTF16 *utf, wchar_t old, wchar_t replacement) {
    unsigned long long number = 0, i;
    for(i = 0; i < utf->length; i++)
        if(utf->characteres[i] == old)
            utf->characteres[i] = replacement;
    return number;
}

cake_bool cake_strutf16_remove(Cake_String_UTF16 *utf, wchar_t *str) {
    wchar_t *start = cake_strutf16_search(utf, str);
    if(start == NULL) return cake_false;
    size_t length = wcslen(str);
    memcpy(start, utf->characteres + length, (utf->length - length) * sizeof(wchar_t));
    utf->length -= length;
    utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t) + sizeof(wchar_t));
    utf->characteres[utf->length] = L'\0';
    return cake_true;
}

wchar_t *cake_strutf16_search(Cake_String_UTF16 *utf, const wchar_t *research) {
    if(utf->length == 0) return NULL;
    size_t length = wcslen(research);
    if(length > utf->length) return NULL;
    
    wchar_t *found = NULL;

    unsigned long long i, j;
    for(i = 0; i < utf->length - length; i++) {
        if(utf->characteres[i] == research[0]) {
            found = &utf->characteres[i++];
            for(j = 1; j < length; j++) {
                if(utf->characteres[i++] != research[j]) {
                    found = NULL;
                    break;
                }
            }
            if(found) break;
        }
    }

    return found;
}

wchar_t *cake_strutf16_search_from_end(Cake_String_UTF16 *utf, wchar_t *research) {
    size_t length = wcslen(research);
    wchar_t *found = NULL;

    unsigned long long i, j, k;
    for(i = utf->length - length; ; i--) {
        if(utf->characteres[i] == research[0]) {
            k = i + 1;
            found = &utf->characteres[i];
            for(j = 1; j < length; j++) {
                if(utf->characteres[k++] != research[j]) {
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

wchar_t *cake_strutf16_search_from(Cake_String_UTF16 *utf, wchar_t *research, unsigned long long *index) {
    size_t length = wcslen(research);
    wchar_t *found = NULL;

    unsigned long long j;
    for(; *index < utf->length - length; (*index)++) {
        if(utf->characteres[*index] == research[0]) {
            found = &utf->characteres[(*index)++];
            for(j = 1; j < length; j++) {
                if(utf->characteres[(*index)++] != research[j]) {
                    found = NULL;
                    break;
                }
            }
            if(found) break;
        }
    }

    return found;
}

cake_bool __strutf16_replace(Cake_String_UTF16 *utf, wchar_t *ptr, wchar_t *old, wchar_t *replacement) {
    if(ptr == NULL) return cake_false;

    size_t oldLength = wcslen(old);
    size_t replacementLength = wcslen(replacement);

    if(oldLength > replacementLength) {
        memcpy(ptr + replacementLength, ptr + oldLength, (utf->length - ((ptr - utf->characteres) + oldLength)) * sizeof(wchar_t));
        memcpy(ptr, replacement, replacementLength * sizeof(wchar_t));
        (utf->length) -= (oldLength - replacementLength);
        utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t) + sizeof(wchar_t));
        utf->characteres[utf->length] = L'\0';
    }else if(oldLength < replacementLength) {
        (utf->length) += (replacementLength - oldLength);
        utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t) + sizeof(wchar_t));
        memcpy(ptr + replacementLength, ptr + oldLength, (utf->length - ((ptr - utf->characteres) + oldLength)) * sizeof(wchar_t));
        memcpy(ptr, replacement, replacementLength * sizeof(wchar_t));
        utf->characteres[utf->length] = L'\0';
    }else
        memcpy(ptr, replacement, replacementLength * sizeof(wchar_t));

    return cake_true;
}

cake_bool cake_strutf16_replace(Cake_String_UTF16 *utf, wchar_t *old, wchar_t *replacement) {
    return __strutf16_replace(utf, cake_strutf16_search(utf, old), old, replacement);
}

cake_bool cake_strutf16_replace_from_end(Cake_String_UTF16 *utf, wchar_t *old, wchar_t *replacement) {
    return __strutf16_replace(utf, cake_strutf16_search_from_end(utf, old), old, replacement);
}

void cake_strutf16_insert(Cake_String_UTF16 *utf, wchar_t *str) {
    size_t length = wcslen(str);
    utf->characteres = (wchar_t *) realloc(utf->characteres, (utf->length + length) * sizeof(wchar_t) + sizeof(wchar_t));

    // Déplacement vers la droite de la chaîne déjà existante :
    memcpy(utf->characteres + length, utf->characteres, utf->length * sizeof(wchar_t));

    utf->length += length;

    // Ajout de la nouvelle chaîne à gauche :
    memcpy(utf->characteres, str, length * sizeof(wchar_t));

    utf->characteres[utf->length] = L'\0';
}

void cake_strutf16_copy(Cake_String_UTF16 *from, Cake_String_UTF16 *to) {
    cake_create_strutf16(to);
    cake_strutf16_set_value(to, from->characteres);
}

cake_bool cake_strutf16_remove_from_index(Cake_String_UTF16 *utf, unsigned long long index) {
    if(index < utf->length) {
        utf->length = index;
        if(utf->length > 0)
            utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t) + sizeof(wchar_t));
        else {
            free(utf->characteres);
            utf->characteres = (wchar_t *) malloc(sizeof(wchar_t));
        }
        utf->characteres[utf->length] = L'\0';
        return cake_true;
    }
    return cake_false;
}

cake_bool cake_strutf16_remove_before_index(Cake_String_UTF16 *utf, unsigned long long index) {
    if(index < utf->length) {
        utf->length -= index;
        memcpy(utf->characteres, &utf->characteres[index], utf->length * sizeof(wchar_t));
        utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t) + sizeof(wchar_t));
        utf->characteres[utf->length] = L'\0';
        return cake_true;
    }
    return cake_false;
}

cake_bool cake_strutf16_remove_part_from_end(Cake_String_UTF16 *utf, wchar_t delim) {
    wchar_t *ptr = &utf->characteres[utf->length - 1];
    while(ptr >= utf->characteres) {
        if(*ptr == delim) {
            cake_strutf16_remove_from_index(utf, ptr - utf->characteres);
            return cake_true;
        }
        ptr--;
    }
    return cake_false;
}

cake_bool cake_strutf16_copy_between(Cake_String_UTF16 *from, Cake_String_UTF16 *to, unsigned long long begin, unsigned long long end) {
    if(end <= begin) {
        cake_create_strutf16(to);
        cake_strutf16_empty(to);
        return cake_false;
    }

    to->length = end - begin;
    to->characteres = (wchar_t *) malloc(to->length * sizeof(wchar_t) + sizeof(wchar_t));
    memcpy(to->characteres, &from->characteres[begin], (end - begin) * sizeof(wchar_t));
    to->characteres[to->length] = L'\0';

    return cake_true;
}

unsigned long long cake_strutf16_number_of(Cake_String_UTF16 *utf, wchar_t charactere) {
    unsigned long long number = 0;
    unsigned long long i;

    for(i = 0; i < utf->length; i++)
        if(utf->characteres[i] == charactere)
            number++;

    return number;
}

wchar_t *cake_strutf16_find(Cake_String_UTF16 *utf, wchar_t research, unsigned long long *index) {
    unsigned long long i;
    
    for(i = *index; i < utf->length; i++) {
        if(utf->characteres[i] == research) {
            *index = i;
            return &utf->characteres[i];
        }
    }

    return NULL;
}

unsigned long long cake_strutf16_rtrim(Cake_String_UTF16 *utf, wchar_t charactere) {
    if(utf->length == 0) return 0;

    unsigned long long number = 0;
    unsigned long long index = utf->length - 1;

    while(utf->characteres[index] == charactere) {
        number++;
        if(index == 0) break;
        index--;
    }

    if(number > 0)
        cake_strutf16_remove_from_index(utf, index + 1);

    return number;
}

unsigned long long cake_strutf16_lower(Cake_String_UTF16 *utf) {
    unsigned long long number = 0, i;
    for(i = 0; i < utf->length; i++) {
        if(utf->characteres[i] > 0x0040 && utf->characteres[i] < 0x005B) {
            utf->characteres[i] += 32;
            number++;
        }
    }
    return number;
}

unsigned long long cake_strutf16_upper(Cake_String_UTF16 *utf) {
    unsigned long long number = 0, i;
    for(i = 0; i < utf->length; i++) {
        if(utf->characteres[i] > 0x0060 && utf->characteres[i] < 0x007B) {
            utf->characteres[i] -= 32;
            number++;
        }
    }
    return number;
}

void cake_free_strutf16(Cake_String_UTF16 *utf) {
    free(utf->characteres);
    free(utf);
}

cake_bool cake_strutf16_remove_index(Cake_String_UTF16 *utf, unsigned long long index) {
    if(index >= utf->length) return cake_false;

    memcpy(&utf->characteres[index], &utf->characteres[index + 1], (utf->length - index) * sizeof(wchar_t));
    (utf->length)--;
    utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t) + sizeof(wchar_t));
    utf->characteres[utf->length] = L'\0';

    return cake_true;
}

cake_bool cake_strutf16_start_with(Cake_String_UTF16 *utf, wchar_t *str) {
    unsigned long long size = wcslen(str);
    if(size > utf->length) return cake_false;

    unsigned long long i;
    for(i = 0; i < size; i++)
        if(utf->characteres[i] != str[i])
            return cake_false;
    return cake_true;
}

ulonglong cake_wstr_count(const wchar_t *str) {
    ulonglong number = 0;
    while(str[number] != L'\0')
        number++;
    return number;
}

void cake_char_array_to_strutf16(const char *source, Cake_String_UTF16 *dest) {
    ulonglong length = cake_str_count(source);
    uchar *end = (uchar *) &source[length - 1];
    dest->length = 0;
    uchar *pStart, *pEnd;

    int bytes;
    ushort value;
    while((uchar *) source <= end) {
        cake_strutf8_index_by_index((const uchar *) source, end, 0L, &pStart, &pEnd, &bytes);
        value = cake_strutf8_decode(pStart, bytes);
        cake_strutf16_add_char(dest, value);
        source = (const char *) pEnd;
    }
}
