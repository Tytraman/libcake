#include "../option.h"

#include <stdio.h>
#include <stdlib.h>

Option *load_option(String_UTF8 *source, const uchar *key, uchar delim) {
    if(source == NULL || source->bytes == NULL)
        return NULL;

    ulonglong internalIndex = 0;
    uchar *keyFound = strutf8_search(source, (uchar *) key, &internalIndex);
    if(keyFound == NULL)
        return NULL;

    Option *opt = (Option *) malloc(sizeof(Option));

    opt->key = strutf8(key);
    opt->value = strutf8("");

    while(internalIndex < source->data.length && source->bytes[internalIndex] != delim)
        internalIndex++;

    if(internalIndex < source->data.length) {
        internalIndex++;
        while(internalIndex < source->data.length && (source->bytes[internalIndex] == ' ' || source->bytes[internalIndex] == '\t'))
            internalIndex++;

        ulonglong startIndex = internalIndex;
        if(internalIndex < source->data.length) {
            ulonglong keyLength = 0;
            while(internalIndex < source->data.length && source->bytes[internalIndex] != '\r' && source->bytes[internalIndex] != '\n') {
                internalIndex++;
                keyLength++;
            }
            if(keyLength > 0) {
                opt->value->bytes = (uchar *) realloc(opt->value->bytes, keyLength * sizeof(uchar) + sizeof(uchar));
                opt->value->data.length = keyLength;
                memcpy(opt->value->bytes, &source->bytes[startIndex], keyLength * sizeof(uchar));
                opt->value->bytes[keyLength] = '\0';
                opt->value->length = strutf8_length(opt->value);
            }
        }
    }

    return opt;
}

void free_option(Option *opt) {
    free_strutf8(opt->key);
    free_strutf8(opt->value);
    free(opt);
}

void set_option(Option **opt, const uchar *key, const uchar *value) {
    if(*opt == NULL)
        *opt = (Option *) malloc(sizeof(Option));

    (*opt)->key = strutf8(key);
    (*opt)->value = strutf8(value);
}
