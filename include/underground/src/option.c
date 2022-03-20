#include "../option.h"

#include <stdio.h>
#include <stdlib.h>

Cake_Option *__cake_load_option(Cake_String_UTF8 *source, const uchar *key, uchar delim, ulonglong *keyIndex, ulonglong *valueIndex) {
    if(source == NULL || source->bytes == NULL)
        return NULL;
    if(keyIndex != NULL)
        *keyIndex = 0;
    if(valueIndex != NULL)
        *valueIndex = 0;

    // On cherche si la source contient la clé
    ulonglong internalIndex = 0;
    uchar *keyFound;

    // On vérifie que la valeur trouvée soit bien une clé
    // en parcourant la ligne dans le sens inverse
    uchar *testKey;
    cake_bool loop = cake_true;
    while(loop) {
        keyFound = cake_strutf8_search_from_start(source, key, &internalIndex);
        if(keyFound == NULL)
            return NULL;
        testKey = keyFound - 1;
        // Si on trouve une valeur au lieu d'une clé
        if(*testKey == delim) {
            testKey = cake_strutf8_search_from_start(source, key, &internalIndex);
            if(testKey == NULL)
                return NULL;
        }else if(testKey <= source->bytes || *testKey == '\n') {
            testKey++;
            cake_bool firstSpace = cake_true;
            uchar *space = NULL;
            while(1) {
                while(1) {
                    if(internalIndex < source->data.length) {
                        if(source->bytes[internalIndex] == ' ' || source->bytes[internalIndex] == '\t') {
                            if(firstSpace) {
                                space = &source->bytes[internalIndex];
                                firstSpace = cake_false;
                            }
                        }else
                            break;
                        internalIndex++;
                    }else
                        break;
                }
                if(source->bytes[internalIndex] == delim)
                    break;
                else if(internalIndex >= source->data.length)
                    break;
                internalIndex++;
            }
            // On vérifie que la clé trouvée soit exactement celle qu'on veut
            if(space == NULL)
                space = &source->bytes[internalIndex];
            uchar temp = *space;
            *space = '\0';
            int res = strcmp(testKey, key);
            *space = temp;
            if(res == 0) {
                if(keyIndex != NULL)
                    *keyIndex = testKey - source->bytes;
                loop = cake_false;
            }
            testKey--;
        }
        testKey--;
    }

    Cake_Option *opt = (Cake_Option *) malloc(sizeof(Cake_Option));

    opt->key = cake_strutf8(key);
    opt->value = cake_strutf8("");

    

    if(internalIndex < source->data.length) {
        internalIndex++;
        while(internalIndex < source->data.length && (source->bytes[internalIndex] == ' ' || source->bytes[internalIndex] == '\t'))
            internalIndex++;

        ulonglong startIndex = internalIndex;
        if(valueIndex != NULL)
            *valueIndex = startIndex;
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
                opt->value->length = cake_strutf8_length(opt->value);
            }
        }
    }

    return opt;
}

void cake_free_option(Cake_Option *opt) {
    cake_free_strutf8(opt->key);
    cake_free_strutf8(opt->value);
    free(opt);
}

void set_option(Cake_Option **opt, const uchar *key, const uchar *value) {
    if(*opt == NULL)
        *opt = (Cake_Option *) malloc(sizeof(Cake_Option));

    (*opt)->key = cake_strutf8(key);
    (*opt)->value = cake_strutf8(value);
}

Cake_FileOption *cake_file_option_load(const uchar *filename, uchar delim) {
    cake_fd fd = cake_fdio_open_file(filename, CAKE_FDIO_ACCESS_READ, CAKE_FDIO_SHARE_READ, CAKE_FDIO_OPEN_IF_EXISTS, CAKE_FDIO_ATTRIBUTE_NORMAL);
    if(fd == CAKE_FDIO_ERROR_OPEN)
        return NULL;
    Cake_FileOption *fileOpt = (Cake_FileOption *) malloc(sizeof(Cake_FileOption));
    fileOpt->fd = fd;
    fileOpt->fileCopy = cake_strutf8("");
    fileOpt->delim = delim;
    cake_fdio_mem_copy_strutf8(fileOpt->fileCopy, fd, CAKE_BUFF_SIZE);
    return fileOpt;
}

Cake_FileOptionElement *cake_file_option_get(Cake_FileOption *fileOpt, const uchar *key) {
    ulonglong keyIndex, valueIndex;
    Cake_Option *opt = __cake_load_option(fileOpt->fileCopy, key, fileOpt->delim, &keyIndex, &valueIndex);
    if(opt == NULL)
        return NULL;
    Cake_FileOptionElement *optElement = (Cake_FileOptionElement *) malloc(sizeof(Cake_FileOptionElement));
    optElement->keyIndex = keyIndex;
    optElement->valueIndex = valueIndex;
    optElement->opt = opt;
    return optElement;
}

void cake_free_file_option(Cake_FileOption *fileOpt) {
    cake_fdio_close(fileOpt->fd);
    cake_free_strutf8(fileOpt->fileCopy);
    free(fileOpt);
}
