#include "../vector.h"

#include <stdio.h>
#include <stdlib.h>

Cake_Vector *cake_vector_new(ushort elementSize, void (*delete_callback)(void *args)) {
    Cake_Vector *vec = (Cake_Vector *) malloc(sizeof(Cake_Vector));

    if(vec == NULL)
        return NULL;
    
    vec->data = (uchar *) malloc(elementSize * CAKE_VECTOR_DEFAULT_CAPACITY);
    if(vec->data == NULL) {
        free(vec);
        return NULL;
    }
    vec->capacity = CAKE_VECTOR_DEFAULT_CAPACITY;
    vec->elementSize = elementSize;
    vec->length = 0;
    vec->delete_callback = delete_callback;

    return vec;
}

cake_bool cake_vector_resize(Cake_Vector *vec, ulonglong capacity) {
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
        return cake_false;
    vec->data = ptr;
    vec->capacity = capacity;
    return cake_true;
}

#include "../utf8.h"

void *cake_vector_push_back(Cake_Vector *vec, void *data) {
    if(vec->length == vec->capacity) {
        if(!cake_vector_resize(vec, vec->capacity + 1)) {
            return NULL;
        }
    }
    void *ptr = memcpy(&vec->data[vec->length * vec->elementSize], data, vec->elementSize);
    (vec->length)++;
    return ptr;
}

void *cake_vector_get(Cake_Vector *vec, ulonglong index) {
    if(index >= vec->length)
        return NULL;
    return vec->data + index * vec->elementSize;
}

void cake_vector_delete(Cake_Vector *vec) {
    if(vec->delete_callback != NULL) {
        ulonglong i;
        for(i = 0; i < vec->length; ++i) {
            vec->delete_callback(&vec->data[i * vec->elementSize]);
        }
    }
    free(vec->data);
    free(vec);
}
