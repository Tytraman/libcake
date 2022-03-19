#ifndef __CAKE_VECTOR_H__
#define __CAKE_VECTOR_H__

#include "def.h"

#define CAKE_VECTOR_DEFAULT_CAPACITY 20

typedef struct cake_vector {
    uchar *data;
    ushort elementSize;
    ulonglong length;
    ulonglong capacity;
    void (*delete_callback)(void *args);
} Cake_Vector;

Cake_Vector *cake_vector_new(ushort elementSize, void (*delete_callback)(void *args));
#define CAKE_VECTOR_NEW(type, delete_callback) cake_vector_new(sizeof(type), delete_callback)

void cake_vector_delete(Cake_Vector *vec);

cake_bool cake_vector_resize(Cake_Vector *vec, ulonglong capacity);

void *cake_vector_push_back(Cake_Vector *vec, void *data);
#define CAKE_VECTOR_PUSH_BACK(vec, data, type) ((type *) cake_vector_push_back(vec, data))

void *cake_vector_get(Cake_Vector *vec, ulonglong index);
#define CAKE_VECTOR_GET(vec, index, type) (*(type *) cake_vector_get(vec, index))

#endif