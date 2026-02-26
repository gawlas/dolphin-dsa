#include <stddef.h>
#include <stdlib.h>

#include "dds/alloc.h"

static void *dds__stdlib_malloc(void *context, size_t size) {
    (void) context;
    return malloc(size);
}

static void *dds__stdlib_realloc(void *context, void *ptr, size_t size) {
    (void) context;
    return realloc(ptr, size);
}

static void dds__stdlib_free(void *context, void *ptr) {
    (void) context;
    free(ptr);
}

dds_alloc_t dds_alloc_stdlib(void) {
    dds_alloc_t alloc;
    alloc.context = NULL;
    alloc.malloc = dds__stdlib_malloc;
    alloc.realloc = dds__stdlib_realloc;
    alloc.free = dds__stdlib_free;

    return alloc;
}
