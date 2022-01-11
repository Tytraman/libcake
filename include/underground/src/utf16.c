#include "../utf16.h"
#include "../utf8.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <wchar.h>

void create_strutf16(String_UTF16 *utf) {
    utf->length = 0;
    utf->characteres = NULL;
}

void clear_strutf16(String_UTF16 *utf) {
    free(utf->characteres);
    create_strutf16(utf);
}

void strutf16_set_value(String_UTF16 *utf, wchar_t *str) {
    utf->length = wcslen(str);
    free(utf->characteres);
    utf->characteres = (wchar_t *) malloc(utf->length * sizeof(wchar_t) + sizeof(wchar_t));
    memcpy(utf->characteres, str, utf->length * sizeof(wchar_t));
    utf->characteres[utf->length] = L'\0';
}

void strutf16_add_wchar_array(String_UTF16 *utf, wchar_t *str) {
    unsigned long long lastLength = utf->length;
    unsigned long long strLength = wcslen(str);
    utf->length += strLength;
    utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t) + sizeof(wchar_t));
    memcpy(&utf->characteres[lastLength], str, strLength * sizeof(wchar_t));
    utf->characteres[utf->length] = L'\0';
}

void strutf16_add_char(String_UTF16 *utf, wchar_t c) {
    (utf->length)++;
    utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t) + sizeof(wchar_t));
    memcpy(&utf->characteres[utf->length - 1], &c, sizeof(wchar_t));
    utf->characteres[utf->length] = L'\0';
}

pika_bool strutf16_key_value(const wchar_t *key, String_UTF16 *src, String_UTF16 *dest) {
    String_UTF16_Reader reader;
    init_strutf16_reader(&reader, src);
    
    String_UTF16 *line;
    wchar_t *ptr = NULL;

    while(ptr == NULL && (line = strutf16_getline(&reader)) != NULL)
        ptr = strutf16_search(line, key);

    if(ptr == NULL) return pika_false;

    unsigned long long index = wcslen(key);
    ptr = strutf16_search_from(line, L":", &index);
    if(ptr == NULL) {
        free_strutf16(line);
        return pika_false;
    }

    while(index++ < line->length && (*ptr == L' ' || *ptr == L'\t'));

    if(index != line->length)
        strutf16_copy_between(line, dest, index, line->length);
    else {
        strutf16_empty(dest);
        free_strutf16(line);
        return pika_false;
    }

    free_strutf16(line);
    return pika_true;
}

void strutf16_empty(String_UTF16 *utf) {
    utf->length = 0;
    free(utf->characteres);
    utf->characteres = (wchar_t *) malloc(sizeof(wchar_t));
    utf->characteres[0] = L'\0';
}

void strutf16_add_bytes(String_UTF16 *utf, unsigned char *bytes, unsigned long long size) {
    unsigned long long lastLength = utf->length;
    utf->length += size;
    utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t));
    memcpy(&utf->characteres[lastLength], bytes, size * sizeof(wchar_t));
}

unsigned long long strutf16_replace_all_char(String_UTF16 *utf, wchar_t old, wchar_t replacement) {
    unsigned long long number = 0, i;
    for(i = 0; i < utf->length; i++)
        if(utf->characteres[i] == old)
            utf->characteres[i] = replacement;
    return number;
}

pika_bool strutf16_remove(String_UTF16 *utf, wchar_t *str) {
    wchar_t *start = strutf16_search(utf, str);
    if(start == NULL) return pika_false;
    size_t length = wcslen(str);
    memcpy(start, utf->characteres + length, (utf->length - length) * sizeof(wchar_t));
    utf->length -= length;
    utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t) + sizeof(wchar_t));
    utf->characteres[utf->length] = L'\0';
    return pika_true;
}

wchar_t *strutf16_search(String_UTF16 *utf, const wchar_t *research) {
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

wchar_t *strutf16_search_from_end(String_UTF16 *utf, wchar_t *research) {
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

wchar_t *strutf16_search_from(String_UTF16 *utf, wchar_t *research, unsigned long long *index) {
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

pika_bool __strutf16_replace(String_UTF16 *utf, wchar_t *ptr, wchar_t *old, wchar_t *replacement) {
    if(ptr == NULL) return pika_false;

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

    return pika_true;
}

pika_bool strutf16_replace(String_UTF16 *utf, wchar_t *old, wchar_t *replacement) {
    return __strutf16_replace(utf, strutf16_search(utf, old), old, replacement);
}

pika_bool strutf16_replace_from_end(String_UTF16 *utf, wchar_t *old, wchar_t *replacement) {
    return __strutf16_replace(utf, strutf16_search_from_end(utf, old), old, replacement);
}

void strutf16_insert(String_UTF16 *utf, wchar_t *str) {
    size_t length = wcslen(str);
    utf->characteres = (wchar_t *) realloc(utf->characteres, (utf->length + length) * sizeof(wchar_t) + sizeof(wchar_t));

    // Déplacement vers la droite de la chaîne déjà existante :
    memcpy(utf->characteres + length, utf->characteres, utf->length * sizeof(wchar_t));

    utf->length += length;

    // Ajout de la nouvelle chaîne à gauche :
    memcpy(utf->characteres, str, length * sizeof(wchar_t));

    utf->characteres[utf->length] = L'\0';
}

void strutf16_copy(String_UTF16 *from, String_UTF16 *to) {
    create_strutf16(to);
    strutf16_set_value(to, from->characteres);
}

pika_bool strutf16_remove_from_index(String_UTF16 *utf, unsigned long long index) {
    if(index < utf->length) {
        utf->length = index;
        if(utf->length > 0)
            utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t) + sizeof(wchar_t));
        else {
            free(utf->characteres);
            utf->characteres = (wchar_t *) malloc(sizeof(wchar_t));
        }
        utf->characteres[utf->length] = L'\0';
        return pika_true;
    }
    return pika_false;
}

pika_bool strutf16_remove_before_index(String_UTF16 *utf, unsigned long long index) {
    if(index < utf->length) {
        utf->length -= index;
        memcpy(utf->characteres, &utf->characteres[index], utf->length * sizeof(wchar_t));
        utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t) + sizeof(wchar_t));
        utf->characteres[utf->length] = L'\0';
        return pika_true;
    }
    return pika_false;
}

pika_bool strutf16_remove_part_from_end(String_UTF16 *utf, wchar_t delim) {
    wchar_t *ptr = &utf->characteres[utf->length - 1];
    while(ptr >= utf->characteres) {
        if(*ptr == delim) {
            strutf16_remove_from_index(utf, ptr - utf->characteres);
            return pika_true;
        }
        ptr--;
    }
    return pika_false;
}

pika_bool strutf16_copy_between(String_UTF16 *from, String_UTF16 *to, unsigned long long begin, unsigned long long end) {
    if(end <= begin) {
        create_strutf16(to);
        strutf16_empty(to);
        return pika_false;
    }

    to->length = end - begin;
    to->characteres = (wchar_t *) malloc(to->length * sizeof(wchar_t) + sizeof(wchar_t));
    memcpy(to->characteres, &from->characteres[begin], (end - begin) * sizeof(wchar_t));
    to->characteres[to->length] = L'\0';

    return pika_true;
}

unsigned long long strutf16_number_of(String_UTF16 *utf, wchar_t charactere) {
    unsigned long long number = 0;
    unsigned long long i;

    for(i = 0; i < utf->length; i++)
        if(utf->characteres[i] == charactere)
            number++;

    return number;
}

wchar_t *strutf16_find(String_UTF16 *utf, wchar_t research, unsigned long long *index) {
    unsigned long long i;
    
    for(i = *index; i < utf->length; i++) {
        if(utf->characteres[i] == research) {
            *index = i;
            return &utf->characteres[i];
        }
    }

    return NULL;
}

unsigned long long strutf16_rtrim(String_UTF16 *utf, wchar_t charactere) {
    if(utf->length == 0) return 0;

    unsigned long long number = 0;
    unsigned long long index = utf->length - 1;

    while(utf->characteres[index] == charactere) {
        number++;
        if(index == 0) break;
        index--;
    }

    if(number > 0)
        strutf16_remove_from_index(utf, index + 1);

    return number;
}

unsigned long long strutf16_lower(String_UTF16 *utf) {
    unsigned long long number = 0, i;
    for(i = 0; i < utf->length; i++) {
        if(utf->characteres[i] > 0x0040 && utf->characteres[i] < 0x005B) {
            utf->characteres[i] += 32;
            number++;
        }
    }
    return number;
}

unsigned long long strutf16_upper(String_UTF16 *utf) {
    unsigned long long number = 0, i;
    for(i = 0; i < utf->length; i++) {
        if(utf->characteres[i] > 0x0060 && utf->characteres[i] < 0x007B) {
            utf->characteres[i] -= 32;
            number++;
        }
    }
    return number;
}

void init_strutf16_reader(String_UTF16_Reader *reader, String_UTF16 *utf) {
    reader->utf = utf;
    reader->index = 0;
    reader->lastPtr = NULL;
}

String_UTF16 *strutf16_getline(String_UTF16_Reader *reader) {
    if(reader->lastPtr != NULL)
        free_strutf16(reader->lastPtr);

    if(reader->utf->length == 0 || reader->index > reader->utf->length) return NULL;

    unsigned long long index = reader->index;
    if(strutf16_find(reader->utf, L'\n', &index) == NULL)
        index = reader->utf->length;
    
    char result;
    if((result = reader->utf->characteres[index - 1] == L'\r'))
        index--;

    reader->lastPtr = (String_UTF16 *) malloc(sizeof(String_UTF16));
    strutf16_copy_between(reader->utf, reader->lastPtr, reader->index, index);
    reader->index = index + (result ? 2 : 1);
    return reader->lastPtr;
}

void free_strutf16(String_UTF16 *utf) {
    free(utf->characteres);
    free(utf);
}

pika_bool strutf16_remove_index(String_UTF16 *utf, unsigned long long index) {
    if(index >= utf->length) return pika_false;

    memcpy(&utf->characteres[index], &utf->characteres[index + 1], (utf->length - index) * sizeof(wchar_t));
    (utf->length)--;
    utf->characteres = (wchar_t *) realloc(utf->characteres, utf->length * sizeof(wchar_t) + sizeof(wchar_t));
    utf->characteres[utf->length] = L'\0';

    return pika_true;
}

pika_bool strutf16_start_with(String_UTF16 *utf, wchar_t *str) {
    unsigned long long size = wcslen(str);
    if(size > utf->length) return pika_false;

    unsigned long long i;
    for(i = 0; i < size; i++)
        if(utf->characteres[i] != str[i])
            return pika_false;
    return pika_true;
}
