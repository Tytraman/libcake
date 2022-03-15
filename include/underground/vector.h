#ifndef __PIKA_VECTOR_H__
#define __PIKA_VECTOR_H__

#include "def.h"

#define PIKA_VECTOR_DEFAULT_CAPACITY 20

typedef struct vector {
    uchar *data;
    ushort elementSize;
    ulonglong length;
    ulonglong capacity;
    void (*delete_callback)(void *args);
} Vector;

Vector *vector_new(ushort elementSize, void (*delete_callback)(void *args));
#define VECTOR_NEW(type, delete_callback) vector_new(sizeof(type), delete_callback)

void vector_delete(Vector *vec);

pika_bool vector_resize(Vector *vec, ulonglong capacity);

void *vector_push_back(Vector *vec, void *data);
#define VECTOR_PUSH_BACK(vec, data, type) ((type *) vector_push_back(vec, data))

void *vector_get(Vector *vec, ulonglong index);
#define VECTOR_GET(vec, index, type) (*(type *) vector_get(vec, index))

#endif