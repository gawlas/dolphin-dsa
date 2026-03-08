#ifndef DOLPHIN_DSA_RING_BUFFER_H
#define DOLPHIN_DSA_RING_BUFFER_H

#include <stdbool.h>

#include "alloc.h"
#include "types.h"

/** Initial capacity allocated on first push. */
#define DDS_RING_BUFFER_INITIAL_SIZE 8

/** Capacity multiplier applied on each resize. */
#define DDS_RING_BUFFER_GROWTH_FACTOR 2

/**
 * Dynamic circular buffer (deque) of fixed-size elements.
 *
 * Elements can be pushed and popped from both ends in O(1).
 * The internal buffer grows automatically as needed.
 *
 * Must be initialized with dds_ring_buffer_init() before use
 * and released with dds_ring_buffer_free() when no longer needed.
 *
 * @note Not thread-safe. External synchronization is required
 *       when a ring buffer instance is shared across threads.
 */
typedef struct dds_ring_buffer {
    void*        data;         /**< Pointer to the element buffer. */
    size_t       head;         /**< Index of the front element (next read position). */
    size_t       tail;         /**< Index past the back element (next write position). */
    size_t       size;         /**< Number of elements currently stored. */
    size_t       capacity;     /**< Number of elements the buffer can hold. */
    size_t       element_size; /**< Size of a single element in bytes. */
    dds_alloc_t  alloc;        /**< Allocator used for all memory operations. */
} dds_ring_buffer_t;

/**
 * Initialize a ring buffer.
 *
 * Must be called before any other operation on the ring buffer.
 *
 * @param ring_buffer  Pointer to an uninitialized ring buffer.
 * @param element_size Size of a single element in bytes.
 * @param alloc        Allocator to use; malloc and free must not be NULL (realloc is not used).
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if ring_buffer is NULL,
 *         element_size is 0, or malloc or free is NULL.
 */
dds_result_t dds_ring_buffer_init(dds_ring_buffer_t* ring_buffer, size_t element_size, dds_alloc_t alloc);

/**
 * Free resources held by a ring buffer.
 *
 * Leaves the ring buffer in a zeroed state. Safe to call with NULL.
 *
 * @param ring_buffer Pointer to the ring buffer to free.
 */
void dds_ring_buffer_free(dds_ring_buffer_t* ring_buffer);

/**
 * Remove all elements from the ring buffer without freeing the internal buffer.
 *
 * Resets size, head, and tail to 0 while preserving the allocated capacity,
 * allowing the buffer to be reused without reallocation.
 *
 * @param ring_buffer Pointer to an initialized ring buffer.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if ring_buffer is NULL.
 */
dds_result_t dds_ring_buffer_clear(dds_ring_buffer_t* ring_buffer);

/**
 * Pre-allocate capacity for at least the given number of elements.
 *
 * If the current capacity already satisfies the request, does nothing.
 * Does not change the size of the ring buffer.
 *
 * @param ring_buffer Pointer to an initialized ring buffer.
 * @param capacity    Minimum number of elements to reserve space for.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if ring_buffer is NULL or capacity is 0,
 *         DDS_OVERFLOW if the required buffer size overflows size_t,
 *         DDS_OUT_OF_MEMORY if allocation fails.
 */
dds_result_t dds_ring_buffer_reserve(dds_ring_buffer_t* ring_buffer, size_t capacity);

/**
 * Release excess capacity so the internal buffer matches the current size.
 *
 * If the ring buffer is empty, frees the buffer entirely and sets capacity to 0.
 * If capacity already equals size, does nothing.
 * Does not change the size of the ring buffer or the elements it contains.
 *
 * @param ring_buffer Pointer to an initialized ring buffer.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if ring_buffer is NULL,
 *         DDS_OVERFLOW if the required buffer size overflows size_t,
 *         DDS_OUT_OF_MEMORY if allocation fails.
 */
dds_result_t dds_ring_buffer_shrink_to_fit(dds_ring_buffer_t* ring_buffer);

/**
 * Append an element to the back of the ring buffer.
 *
 * Grows the internal buffer if necessary.
 *
 * @param ring_buffer Pointer to an initialized ring buffer.
 * @param element     Pointer to the element to copy into the ring buffer.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if any argument is NULL,
 *         DDS_OVERFLOW if the required buffer size overflows size_t,
 *         DDS_OUT_OF_MEMORY if allocation fails.
 */
dds_result_t dds_ring_buffer_push_back(dds_ring_buffer_t* ring_buffer, const void* element);

/**
 * Copy the back element into the provided buffer without removing it.
 *
 * @param ring_buffer Pointer to an initialized ring buffer.
 * @param out_element Destination buffer of at least element_size bytes.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if any argument is NULL,
 *         DDS_OUT_OF_RANGE if the ring buffer is empty.
 */
dds_result_t dds_ring_buffer_peek_back(const dds_ring_buffer_t* ring_buffer, void* out_element);

/**
 * Remove the back element and optionally copy it into the provided buffer.
 *
 * @param ring_buffer Pointer to an initialized ring buffer.
 * @param out_element Destination buffer of at least element_size bytes, or NULL to discard.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if ring_buffer is NULL,
 *         DDS_OUT_OF_RANGE if the ring buffer is empty.
 */
dds_result_t dds_ring_buffer_pop_back(dds_ring_buffer_t* ring_buffer, void* out_element);

/**
 * Prepend an element to the front of the ring buffer.
 *
 * Grows the internal buffer if necessary.
 *
 * @param ring_buffer Pointer to an initialized ring buffer.
 * @param element     Pointer to the element to copy into the ring buffer.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if any argument is NULL,
 *         DDS_OVERFLOW if the required buffer size overflows size_t,
 *         DDS_OUT_OF_MEMORY if allocation fails.
 */
dds_result_t dds_ring_buffer_push_front(dds_ring_buffer_t* ring_buffer, const void* element);

/**
 * Copy the front element into the provided buffer without removing it.
 *
 * @param ring_buffer Pointer to an initialized ring buffer.
 * @param out_element Destination buffer of at least element_size bytes.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if any argument is NULL,
 *         DDS_OUT_OF_RANGE if the ring buffer is empty.
 */
dds_result_t dds_ring_buffer_peek_front(const dds_ring_buffer_t* ring_buffer, void* out_element);

/**
 * Remove the front element and optionally copy it into the provided buffer.
 *
 * @param ring_buffer Pointer to an initialized ring buffer.
 * @param out_element Destination buffer of at least element_size bytes, or NULL to discard.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if ring_buffer is NULL,
 *         DDS_OUT_OF_RANGE if the ring buffer is empty.
 */
dds_result_t dds_ring_buffer_pop_front(dds_ring_buffer_t* ring_buffer, void* out_element);

/**
 * Return the number of elements currently stored in the ring buffer.
 *
 * @param ring_buffer Pointer to an initialized ring buffer.
 * @return Number of elements, or 0 if ring_buffer is NULL.
 */
size_t dds_ring_buffer_get_size(const dds_ring_buffer_t* ring_buffer);

/**
 * Return the current capacity of the ring buffer's internal buffer.
 *
 * @param ring_buffer Pointer to an initialized ring buffer.
 * @return Capacity in elements, or 0 if ring_buffer is NULL.
 */
size_t dds_ring_buffer_get_capacity(const dds_ring_buffer_t* ring_buffer);

/**
 * Return a pointer to the ring buffer's internal data buffer.
 *
 * The layout is circular: elements are not necessarily contiguous from index 0.
 * The pointer is valid until the next operation that modifies the ring buffer.
 *
 * @param ring_buffer Pointer to an initialized ring buffer.
 * @return Pointer to the raw data buffer, or NULL if ring_buffer is NULL or
 *         the internal buffer has not been allocated yet.
 */
void* dds_ring_buffer_get_data(const dds_ring_buffer_t* ring_buffer);

/**
 * Check whether the ring buffer contains no elements.
 *
 * @param ring_buffer Pointer to an initialized ring buffer.
 * @return true if the ring buffer is empty or NULL, false otherwise.
 */
bool dds_ring_buffer_is_empty(const dds_ring_buffer_t* ring_buffer);

#endif //DOLPHIN_DSA_RING_BUFFER_H
