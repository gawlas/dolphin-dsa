#ifndef DOLPHIN_DSA_RING_BUFFER_H
#define DOLPHIN_DSA_RING_BUFFER_H

#include <stdbool.h>

#include "alloc.h"
#include "types.h"

/** Initial capacity allocated on first push. */
#define DDS_RING_BUFFER_INITIAL_SIZE 8

/** Capacity multiplier applied on each resize. */
#define DDS_RING_BUFFER_GROWTH_FACTOR 2

typedef struct dds_ring_buffer {
    void*        data;         /**< Pointer to the element buffer. */
    size_t       size;         /**< Number of elements currently stored. */
    size_t       head;         /**< index of the head element. */
    size_t       capacity;     /**< Number of elements the buffer can hold. */
    size_t       element_size; /**< Size of a single element in bytes. */
    dds_alloc_t  alloc;        /**< Allocator used for all memory operations. */
} dds_ring_buffer_t;

dds_result_t dds_ring_buffer_init(dds_ring_buffer_t* ring_buffer, size_t element_size, dds_alloc_t alloc);
void dds_ring_buffer_free(dds_ring_buffer_t* ring_buffer);
dds_result_t dds_ring_buffer_clear(dds_ring_buffer_t* ring_buffer);
dds_result_t dds_ring_buffer_reserve(dds_ring_buffer_t* ring_buffer, size_t capacity);
dds_result_t dds_ring_buffer_shrink_to_fit(dds_ring_buffer_t* ring_buffer);

dds_result_t dds_ring_buffer_push_back(dds_ring_buffer_t* ring_buffer, const void* element);
dds_result_t dds_ring_buffer_push_front(dds_ring_buffer_t* ring_buffer, const void* element);
dds_result_t dds_ring_buffer_insert(dds_ring_buffer_t* ring_buffer, size_t index, const void* element);
dds_result_t dds_ring_buffer_set(dds_ring_buffer_t* ring_buffer, size_t index, const void* element);

dds_result_t dds_ring_buffer_pop_back(dds_ring_buffer_t* ring_buffer, void* element);
dds_result_t dds_ring_buffer_peek_back(const dds_ring_buffer_t* ring_buffer, void* element);
dds_result_t dds_ring_buffer_pop_front(dds_ring_buffer_t* ring_buffer, void* element);
dds_result_t dds_ring_buffer_peek_front(const dds_ring_buffer_t* ring_buffer, void* element);
dds_result_t dds_ring_buffer_get(const dds_ring_buffer_t* ring_buffer, size_t index, void* element);
dds_result_t dds_ring_buffer_remove(dds_ring_buffer_t* ring_buffer, size_t index, void* element);
void* dds_ring_buffer_at(const dds_ring_buffer_t* ring_buffer, size_t index);
// TODO: implement correct index macro
// #define dds_ring_buffer_index(ring_buffer, Type, index) (((Type*)(ring_buffer)->data)[(index)])

size_t dds_ring_buffer_get_size(const dds_ring_buffer_t* ring_buffer);
size_t dds_ring_buffer_get_capacity(const dds_ring_buffer_t* ring_buffer);
void* dds_ring_buffer_get_data(const dds_ring_buffer_t* ring_buffer);
bool dds_ring_buffer_is_empty(const dds_ring_buffer_t* ring_buffer);

#endif //DOLPHIN_DSA_RING_BUFFER_H
