#ifndef DOLPHIN_DSA_STACK_H
#define DOLPHIN_DSA_STACK_H

#include <stdbool.h>

#include "alloc.h"
#include "types.h"
#include "vector.h"

/**
 * Dynamic LIFO stack of fixed-size elements backed by a dds_vector_t.
 *
 * Elements are pushed and popped from the top in O(1).
 * The internal buffer grows automatically as needed.
 *
 * Must be initialized with dds_stack_init() before use
 * and released with dds_stack_free() when no longer needed.
 *
 * @note Not thread-safe. External synchronization is required
 *       when a stack instance is shared across threads.
 */
typedef struct dds_stack {
    dds_vector_t vector; /**< Underlying storage. */
} dds_stack_t;

/**
 * Initialize a stack.
 *
 * Must be called before any other operation on the stack.
 *
 * @param stack        Pointer to an uninitialized stack.
 * @param element_size Size of a single element in bytes.
 * @param alloc        Allocator to use; malloc, realloc, and free must not be NULL.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if stack is NULL,
 *         element_size is 0, or any alloc function is NULL.
 */
dds_result_t dds_stack_init(dds_stack_t* stack, size_t element_size, dds_alloc_t alloc);

/**
 * Free resources held by a stack.
 *
 * Leaves the stack in a zeroed state. Safe to call with NULL.
 *
 * @param stack Pointer to the stack to free.
 */
void dds_stack_free(dds_stack_t* stack);

/**
 * Remove all elements from the stack without freeing the internal buffer.
 *
 * Resets size to 0 while preserving the allocated capacity,
 * allowing the buffer to be reused without reallocation.
 *
 * @param stack Pointer to an initialized stack.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if stack is NULL.
 */
dds_result_t dds_stack_clear(dds_stack_t* stack);

/**
 * Pre-allocate capacity for at least the given number of elements.
 *
 * If the current capacity already satisfies the request, does nothing.
 * Does not change the size of the stack.
 *
 * @param stack    Pointer to an initialized stack.
 * @param capacity Minimum number of elements to reserve space for.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if stack is NULL or capacity is 0,
 *         DDS_OVERFLOW if the required buffer size overflows size_t,
 *         DDS_OUT_OF_MEMORY if allocation fails.
 */
dds_result_t dds_stack_reserve(dds_stack_t* stack, size_t capacity);

/**
 * Release excess capacity so the internal buffer matches the current size.
 *
 * If the stack is empty, frees the buffer entirely and sets capacity to 0.
 * If capacity already equals size, does nothing.
 * Does not change the size of the stack or the elements it contains.
 *
 * @param stack Pointer to an initialized stack.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if stack is NULL,
 *         DDS_OVERFLOW if the required buffer size overflows size_t,
 *         DDS_OUT_OF_MEMORY if allocation fails.
 */
dds_result_t dds_stack_shrink_to_fit(dds_stack_t* stack);

/**
 * Push an element onto the top of the stack.
 *
 * Grows the internal buffer if necessary.
 *
 * @param stack   Pointer to an initialized stack.
 * @param element Pointer to the element to copy onto the stack.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if any argument is NULL,
 *         DDS_OVERFLOW if the required buffer size overflows size_t,
 *         DDS_OUT_OF_MEMORY if allocation fails.
 */
dds_result_t dds_stack_push(dds_stack_t* stack, const void* element);

/**
 * Remove the top element from the stack and optionally copy it into the provided buffer.
 *
 * @param stack       Pointer to an initialized stack.
 * @param out_element Destination buffer of at least element_size bytes, or NULL to discard.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if stack is NULL,
 *         DDS_OUT_OF_RANGE if the stack is empty.
 */
dds_result_t dds_stack_pop(dds_stack_t* stack, void* out_element);

/**
 * Copy the top element into the provided buffer without removing it.
 *
 * @param stack       Pointer to an initialized stack.
 * @param out_element Destination buffer of at least element_size bytes.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if any argument is NULL,
 *         DDS_OUT_OF_RANGE if the stack is empty.
 */
dds_result_t dds_stack_peek(const dds_stack_t* stack, void* out_element);

/**
 * Return the number of elements currently stored in the stack.
 *
 * @param stack Pointer to an initialized stack.
 * @return Number of elements, or 0 if stack is NULL.
 */
size_t dds_stack_get_size(const dds_stack_t* stack);

/**
 * Return the current capacity of the stack's internal buffer.
 *
 * @param stack Pointer to an initialized stack.
 * @return Capacity in elements, or 0 if stack is NULL.
 */
size_t dds_stack_get_capacity(const dds_stack_t* stack);

/**
 * Check whether the stack contains no elements.
 *
 * @param stack Pointer to an initialized stack.
 * @return true if the stack is empty or NULL, false otherwise.
 */
bool dds_stack_is_empty(const dds_stack_t* stack);

#endif //DOLPHIN_DSA_STACK_H
