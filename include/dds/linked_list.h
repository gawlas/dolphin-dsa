#ifndef DOLPHIN_DSA_LINKED_LIST_H
#define DOLPHIN_DSA_LINKED_LIST_H
#include <stdbool.h>
#include <stddef.h>

#include "alloc.h"
#include "types.h"

typedef struct dds_linked_list_node {
    struct dds_linked_list_node* next;
    struct dds_linked_list_node* prev;
    unsigned char data[];
} dds_linked_list_node_t;

/**
 * Doubly-linked list of fixed-size elements.
 *
 * Must be initialized with dds_linked_list_init() before use
 * and released with dds_linked_list_free() when no longer needed.
 *
 * @note Not thread-safe. External synchronization is required
 *       when a list instance is shared across threads.
 */
typedef struct dds_linked_list {
    dds_linked_list_node_t*         head;         /**< Pointer to the first node, or NULL if empty. */
    dds_linked_list_node_t*         tail;         /**< Pointer to the last node, or NULL if empty. */
    size_t                          size;         /**< Number of elements currently stored. */
    size_t                          element_size; /**< Size of a single element in bytes. */
    dds_alloc_t                     alloc;        /**< Allocator used for all memory operations. */
} dds_linked_list_t;

/**
 * Initialize a linked list.
 *
 * Must be called before any other operation on the list.
 *
 * @param linked_list  Pointer to an uninitialized linked list.
 * @param element_size Size of a single element in bytes.
 * @param alloc        Allocator to use; malloc and free must not be NULL.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if linked_list is NULL,
 *         element_size is 0, or any alloc function is NULL.
 */
dds_result_t dds_linked_list_init(dds_linked_list_t* linked_list, size_t element_size, dds_alloc_t alloc);

/**
 * Remove all elements from the list and free their nodes.
 *
 * Resets the list to an empty state while preserving the allocator and
 * element_size, allowing the list to be reused without re-initialization.
 *
 * @param linked_list Pointer to an initialized list.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if linked_list is NULL.
 */
dds_result_t dds_linked_list_clear(dds_linked_list_t* linked_list);

/**
 * Free all nodes and resources held by a linked list.
 *
 * Traverses every node and frees its memory using the list's allocator,
 * then leaves the list in a zeroed state. Safe to call with NULL.
 *
 * @param linked_list Pointer to the linked list to free.
 */
void dds_linked_list_free(dds_linked_list_t* linked_list);

/**
 * Append an element to the end of the list.
 *
 * @param list    Pointer to an initialized list.
 * @param element Pointer to the element to copy into the list.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if any argument is NULL,
 *         DDS_OUT_OF_MEMORY if allocation fails.
 */
dds_result_t dds_linked_list_push_back(dds_linked_list_t* list, const void* element);

/**
 * Prepend an element to the front of the list.
 *
 * @param list    Pointer to an initialized list.
 * @param element Pointer to the element to copy into the list.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if any argument is NULL,
 *         DDS_OUT_OF_MEMORY if allocation fails.
 */
dds_result_t dds_linked_list_push_front(dds_linked_list_t* list, const void* element);

/**
 * Remove the last element from the list and optionally copy it to the provided buffer.
 *
 * @param list        Pointer to an initialized list.
 * @param out_element Destination buffer of at least element_size bytes, or NULL to discard.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if list is NULL,
 *         DDS_OUT_OF_RANGE if the list is empty.
 */
dds_result_t dds_linked_list_pop_back(dds_linked_list_t* list, void* out_element);

/**
 * Remove the first element from the list and optionally copy it to the provided buffer.
 *
 * @param list        Pointer to an initialized list.
 * @param out_element Destination buffer of at least element_size bytes, or NULL to discard.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if list is NULL,
 *         DDS_OUT_OF_RANGE if the list is empty.
 */
dds_result_t dds_linked_list_pop_front(dds_linked_list_t* list, void* out_element);

/**
 * Copy the element at the given index into the provided buffer.
 *
 * Does not modify the list.
 *
 * @param list        Pointer to an initialized list.
 * @param index       Zero-based index of the element.
 * @param out_element Destination buffer of at least element_size bytes.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if list or out_element is NULL,
 *         DDS_OUT_OF_RANGE if index is out of bounds.
 */
dds_result_t dds_linked_list_get(const dds_linked_list_t* list, size_t index, void* out_element);

/**
 * Overwrite the element at the given index with a copy of the provided value.
 *
 * @param list    Pointer to an initialized list.
 * @param index   Zero-based index of the element to overwrite.
 * @param element Pointer to the value to copy into the list.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if list or element is NULL,
 *         DDS_OUT_OF_RANGE if index is out of bounds.
 */
dds_result_t dds_linked_list_set(dds_linked_list_t* list, size_t index, const void* element);

/**
 * Insert an element at the given index, shifting subsequent elements forward.
 *
 * An index of 0 prepends to the front. An index equal to the current size
 * appends to the end.
 *
 * @param list    Pointer to an initialized list.
 * @param index   Zero-based index at which to insert; must be <= size.
 * @param element Pointer to the element to copy into the list.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if list or element is NULL,
 *         DDS_OUT_OF_RANGE if index exceeds size,
 *         DDS_OUT_OF_MEMORY if allocation fails.
 */
dds_result_t dds_linked_list_insert(dds_linked_list_t* list, size_t index, const void* element);

/**
 * Remove the element at the given index and optionally copy it to the provided buffer.
 *
 * @param list        Pointer to an initialized list.
 * @param index       Zero-based index of the element to remove.
 * @param out_element Destination buffer of at least element_size bytes, or NULL to discard.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if list is NULL,
 *         DDS_OUT_OF_RANGE if index is out of bounds.
 */
dds_result_t dds_linked_list_remove(dds_linked_list_t* list, size_t index, void* out_element);

/**
 * Return the number of elements currently stored in the list.
 *
 * @param list Pointer to an initialized list.
 * @return Number of elements, or 0 if list is NULL.
 */
size_t dds_linked_list_get_size(const dds_linked_list_t* list);

/**
 * Check whether the list contains no elements.
 *
 * @param list Pointer to an initialized list.
 * @return true if the list is empty or NULL, false otherwise.
 */
bool dds_linked_list_is_empty(const dds_linked_list_t* list);


#endif //DOLPHIN_DSA_LINKED_LIST_H
