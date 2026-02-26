#ifndef DOLPHIN_DSA_ALLOC_H
#define DOLPHIN_DSA_ALLOC_H

#include <stddef.h>

typedef void* (*dds_malloc_func)(void* context, size_t size);
typedef void* (*dds_realloc_func)(void* context, void* ptr, size_t size);
typedef void (*dds_free_func)(void* context, void* ptr);

typedef struct dds_alloc {
    void* context;
    dds_malloc_func malloc;
    dds_realloc_func realloc;
    dds_free_func free;
} dds_alloc_t;

dds_alloc_t dds_alloc_stdlib(void);

#endif //DOLPHIN_DSA_ALLOC_H
