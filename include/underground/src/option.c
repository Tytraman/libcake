#include "../option.h"

#include <stdio.h>
#include <stdlib.h>

Option *load_option(String_UTF8 *source, const uchar *key, uchar delim) {
    if(source == NULL || source->bytes == NULL)
        return NULL;

    // On cherche si la source contient la clé
    ulonglong internalIndex = 0;
    uchar *keyFound;

    // On vérifie que la valeur trouvée soit bien une clé
    // en parcourant la ligne dans le sens inverse
    uchar *testKey;
    pika_bool loop = pika_true;
    while(loop) {
        keyFound = strutf8_search(source, key, &internalIndex);
        if(keyFound == NULL)
            return NULL;
        testKey = keyFound - 1;
        // Si on trouve une valeur au lieu d'une clé
        if(*testKey == delim) {
            testKey = strutf8_search(source, key, &internalIndex);
            if(testKey == NULL)
                return NULL;
        }else if(testKey <= source->bytes || *testKey == '\n') {
            testKey++;
            while(internalIndex < source->data.length && source->bytes[internalIndex] != delim)
                internalIndex++;
            // On vérifie que la clé trouvée soit exactement celle qu'on veut
            uchar temp = source->bytes[internalIndex];
            source->bytes[internalIndex] = '\0';
            int res = strcmp(testKey, key);
            source->bytes[internalIndex] = temp;
            if(res == 0)
                loop = pika_false;
            testKey--;
        }
        testKey--;
    }

    Option *opt = (Option *) malloc(sizeof(Option));

    opt->key = strutf8(key);
    opt->value = strutf8("");

    

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
