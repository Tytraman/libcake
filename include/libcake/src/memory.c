#include "../memory.h"

#include <string.h>
#include <stdlib.h>

void cake_rem_allocate(void **array, void *start, size_t elements, size_t byteSize, unsigned long long *arraySize) {
    char *end = (char *) start + (elements * byteSize);
    size_t size = (((*arraySize) * byteSize) + byteSize) - ((char *) end - (char *) *array);
    memcpy(start, end, size);
    (*arraySize) -= elements;
    *array = realloc(*array, (*arraySize) * byteSize);
}

void cake_move_allocate(void **array, void *pToAdd, void *src, size_t elements, size_t byteSize, unsigned long long *arraySize) {
    char *pTrueEnd = ((char *) *array) + (*arraySize) * byteSize;
    char *pEnd = (char *) pToAdd + elements * byteSize;
    unsigned long long index = (char *) pToAdd - ((char *) *array);
    unsigned long long size = pTrueEnd - (char *) pToAdd;
    (*arraySize) += elements;
    *array = realloc(*array, (*arraySize) * byteSize);
    pToAdd = ((char *) *array) + index;
    pEnd = (char *) pToAdd + elements * byteSize;
    memcpy(pEnd, pToAdd, size);
    memcpy(pToAdd, src, elements * byteSize);
}

void cake_add_allocate(void **array, void *src, size_t elements, size_t byteSize, unsigned long long *arraySize) {
    unsigned long long tempArraySize = *arraySize;
    (*arraySize) += elements;
    *array = realloc(*array, (*arraySize) * byteSize);
    unsigned char *p = (unsigned char *) *array;
    memcpy(&p[tempArraySize * byteSize], src, elements * byteSize);
}

void cake_copy_value(char **buffer, unsigned char *src, unsigned long long valueSize) {
    *buffer = (char *) malloc(valueSize + 1);
    memcpy(*buffer, src, valueSize);
    (*buffer)[valueSize] = '\0';
}
