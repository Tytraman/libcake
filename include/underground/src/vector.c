#include "../vector.h"

#include <stdio.h>
#include <stdlib.h>

Vector *vector_new(ushort elementSize, void (*delete_callback)(void *args)) {
    Vector *vec = (Vector *) malloc(sizeof(Vector));

    if(vec == NULL)
        return NULL;
    
    vec->data = (uchar *) malloc(elementSize * PIKA_VECTOR_DEFAULT_CAPACITY);
    if(vec->data == NULL) {
        free(vec);
        return NULL;
    }
    vec->capacity = PIKA_VECTOR_DEFAULT_CAPACITY;
    vec->elementSize = elementSize;
    vec->length = 0;
    vec->delete_callback = delete_callback;

    return vec;
}

pika_bool vector_resize(Vector *vec, ulonglong capacity) {
    // si capacity < length, free la différence d'éléments
    if(capacity < vec->length
        && vec->delete_callback != NULL) {
            ulonglong i;
            for(i = vec->length - 1; i >= capacity; --i)
                vec->delete_callback(&vec->data[i * vec->elementSize]);
            vec->length = capacity;
    }
    void *ptr = realloc(vec->data, capacity * vec->elementSize);

    // Si realloc n'a pas fonctionné, les données au-dessus ne seront pas restaurées
    if(ptr == NULL)
        return pika_false;
    vec->data = ptr;
    vec->capacity = capacity;
    return pika_true;
}

#include "../utf8.h"

void *vector_push_back(Vector *vec, void *data) {
    if(vec->length == vec->capacity) {
        if(!vector_resize(vec, vec->capacity + 1)) {
            return NULL;
        }
    }
    void *ptr = memcpy(&vec->data[vec->length * vec->elementSize], data, vec->elementSize);
    (vec->length)++;
    return ptr;
}

void *vector_get(Vector *vec, ulonglong index) {
    if(index >= vec->length)
        return NULL;
    return vec->data + index * vec->elementSize;
}

void vector_delete(Vector *vec) {
    if(vec->delete_callback != NULL) {
        ulonglong i;
        for(i = 0; i < vec->length; ++i) {
            vec->delete_callback(&vec->data[i * vec->elementSize]);
        }
    }
    free(vec->data);
    free(vec);
}
