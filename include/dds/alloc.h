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

typedef void (*dds_destroy_fn_t)(const void *data, void *context);

typedef struct dds_destroy {
    dds_destroy_fn_t fn;      /**< Destroy function; NULL means no-op. */
    void            *context; /**< User context passed to fn. */
} dds_destroy_t;

/**
 * Returns a no-op destroy (fn = NULL, context = NULL).
 * Pass wherever no cleanup is needed.
 */
dds_destroy_t dds_destroy_none(void);

/**
 * Returns a destroy that calls free() on the data pointer.
 * Use when the table owns heap-allocated keys or values.
 */
dds_destroy_t dds_destroy_stdlib(void);

#endif //DOLPHIN_DSA_ALLOC_H
