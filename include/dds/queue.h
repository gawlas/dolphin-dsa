#ifndef DOLPHIN_DSA_QUEUE_H
#define DOLPHIN_DSA_QUEUE_H

#include <stdbool.h>

#include "alloc.h"
#include "types.h"
#include "ring_buffer.h"

/**
 * Dynamic FIFO queue of fixed-size elements backed by a dds_ring_buffer_t.
 *
 * Elements are enqueued at the back and dequeued from the front, both in O(1).
 * The internal buffer grows automatically as needed.
 *
 * Must be initialized with dds_queue_init() before use
 * and released with dds_queue_free() when no longer needed.
 *
 * @note Not thread-safe. External synchronization is required
 *       when a queue instance is shared across threads.
 */
typedef struct dds_queue {
    dds_ring_buffer_t ring_buffer; /**< Underlying storage. */
} dds_queue_t;

/**
 * Initialize a queue.
 *
 * Must be called before any other operation on the queue.
 *
 * @param queue        Pointer to an uninitialized queue.
 * @param element_size Size of a single element in bytes.
 * @param alloc        Allocator to use; malloc and free must not be NULL (realloc is not used).
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if queue is NULL,
 *         element_size is 0, or malloc or free is NULL.
 */
dds_result_t dds_queue_init(dds_queue_t* queue, size_t element_size, dds_alloc_t alloc);

/**
 * Free resources held by a queue.
 *
 * Leaves the queue in a zeroed state. Safe to call with NULL.
 *
 * @param queue Pointer to the queue to free.
 */
void dds_queue_free(dds_queue_t* queue);

/**
 * Remove all elements from the queue without freeing the internal buffer.
 *
 * Resets size to 0 while preserving the allocated capacity,
 * allowing the buffer to be reused without reallocation.
 *
 * @param queue Pointer to an initialized queue.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if queue is NULL.
 */
dds_result_t dds_queue_clear(dds_queue_t* queue);

/**
 * Pre-allocate capacity for at least the given number of elements.
 *
 * If the current capacity already satisfies the request, does nothing.
 * Does not change the size of the queue.
 *
 * @param queue    Pointer to an initialized queue.
 * @param capacity Minimum number of elements to reserve space for.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if queue is NULL or capacity is 0,
 *         DDS_OVERFLOW if the required buffer size overflows size_t,
 *         DDS_OUT_OF_MEMORY if allocation fails.
 */
dds_result_t dds_queue_reserve(dds_queue_t* queue, size_t capacity);

/**
 * Release excess capacity so the internal buffer matches the current size.
 *
 * If the queue is empty, frees the buffer entirely and sets capacity to 0.
 * If capacity already equals size, does nothing.
 * Does not change the size of the queue or the elements it contains.
 *
 * @param queue Pointer to an initialized queue.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if queue is NULL,
 *         DDS_OVERFLOW if the required buffer size overflows size_t,
 *         DDS_OUT_OF_MEMORY if allocation fails.
 */
dds_result_t dds_queue_shrink_to_fit(dds_queue_t* queue);

/**
 * Add an element to the back of the queue.
 *
 * Grows the internal buffer if necessary.
 *
 * @param queue   Pointer to an initialized queue.
 * @param element Pointer to the element to copy into the queue.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if any argument is NULL,
 *         DDS_OVERFLOW if the required buffer size overflows size_t,
 *         DDS_OUT_OF_MEMORY if allocation fails.
 */
dds_result_t dds_queue_enqueue(dds_queue_t* queue, const void* element);

/**
 * Remove the front element from the queue and optionally copy it into the provided buffer.
 *
 * @param queue       Pointer to an initialized queue.
 * @param out_element Destination buffer of at least element_size bytes, or NULL to discard.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if queue is NULL,
 *         DDS_OUT_OF_RANGE if the queue is empty.
 */
dds_result_t dds_queue_dequeue(dds_queue_t* queue, void* out_element);

/**
 * Copy the front element into the provided buffer without removing it.
 *
 * @param queue       Pointer to an initialized queue.
 * @param out_element Destination buffer of at least element_size bytes.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if any argument is NULL,
 *         DDS_OUT_OF_RANGE if the queue is empty.
 */
dds_result_t dds_queue_peek_front(const dds_queue_t* queue, void* out_element);

/**
 * Return the number of elements currently stored in the queue.
 *
 * @param queue Pointer to an initialized queue.
 * @return Number of elements, or 0 if queue is NULL.
 */
size_t dds_queue_get_size(const dds_queue_t* queue);

/**
 * Return the current capacity of the queue's internal buffer.
 *
 * @param queue Pointer to an initialized queue.
 * @return Capacity in elements, or 0 if queue is NULL.
 */
size_t dds_queue_get_capacity(const dds_queue_t* queue);

/**
 * Check whether the queue contains no elements.
 *
 * @param queue Pointer to an initialized queue.
 * @return true if the queue is empty or NULL, false otherwise.
 */
bool dds_queue_is_empty(const dds_queue_t* queue);

#endif //DOLPHIN_DSA_QUEUE_H
