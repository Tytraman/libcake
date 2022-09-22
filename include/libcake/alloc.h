#ifndef __CAKE_ALLOC_H__
#define __CAKE_ALLOC_H__

#include "def.h"

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

inline void *cake_new(ulonglong size) {
    return malloc(size);
}

inline void *cake_realloc(void *ptr, ulonglong size) {
    return realloc(ptr, size);
}

inline void cake_free(void *ptr) {
    free(ptr);
}

#ifdef __cplusplus
}
#endif

#endif