#include "../option.h"

#include <stdio.h>
#include <stdlib.h>

Option *__load_option(String_UTF8 *source, const uchar *key, uchar delim, ulonglong *keyIndex, ulonglong *valueIndex) {
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
            pika_bool firstSpace = pika_true;
            uchar *space = NULL;
            while(1) {
                while(1) {
                    if(internalIndex < source->data.length) {
                        if(source->bytes[internalIndex] == ' ' || source->bytes[internalIndex] == '\t') {
                            if(firstSpace) {
                                space = &source->bytes[internalIndex];
                                firstSpace = pika_false;
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
                loop = pika_false;
            }
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

FileOption *file_option_load(const uchar *filename, uchar delim) {
    pika_fd fd = fdio_open_file(filename, FDIO_ACCESS_READ, FDIO_SHARE_READ, FDIO_OPEN_IF_EXISTS, FDIO_ATTRIBUTE_NORMAL);
    if(fd == FDIO_ERROR_OPEN)
        return NULL;
    FileOption *fileOpt = (FileOption *) malloc(sizeof(FileOption));
    fileOpt->fd = fd;
    fileOpt->fileCopy = strutf8("");
    fileOpt->delim = delim;
    fdio_mem_copy_strutf8(fileOpt->fileCopy, fd, PIKA_BUFF_SIZE);
    return fileOpt;
}

FileOptionElement *file_option_get(FileOption *fileOpt, const uchar *key) {
    ulonglong keyIndex, valueIndex;
    Option *opt = __load_option(fileOpt->fileCopy, key, fileOpt->delim, &keyIndex, &valueIndex);
    if(opt == NULL)
        return NULL;
    FileOptionElement *optElement = (FileOptionElement *) malloc(sizeof(FileOptionElement));
    optElement->keyIndex = keyIndex;
    optElement->valueIndex = valueIndex;
    optElement->opt = opt;
    return optElement;
}

void free_file_option(FileOption *fileOpt) {
    fdio_close(fileOpt->fd);
    free_strutf8(fileOpt->fileCopy);
    free(fileOpt);
}
