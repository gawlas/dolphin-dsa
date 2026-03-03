#ifndef DOLPHIN_DSA_VECTOR_H
#define DOLPHIN_DSA_VECTOR_H

#include "alloc.h"
#include "types.h"

/** Initial capacity allocated on first push. */
#define INITIAL_SIZE 1

/** Capacity multiplier applied on each resize. */
#define GROWTH_FACTOR 2

/**
 * Dynamic array of fixed-size elements.
 *
 * Must be initialized with dds_vector_init() before use
 * and released with dds_vector_free() when no longer needed.
 */
typedef struct dds_vector {
    void*        data;         /**< Pointer to the element buffer. */
    size_t       size;         /**< Number of elements currently stored. */
    size_t       capacity;     /**< Number of elements the buffer can hold. */
    size_t       element_size; /**< Size of a single element in bytes. */
    dds_alloc_t  alloc;        /**< Allocator used for all memory operations. */
} dds_vector_t;

/**
 * Initialize a vector.
 *
 * Must be called before any other operation on the vector.
 *
 * @param vector       Pointer to an uninitialized vector.
 * @param element_size Size of a single element in bytes.
 * @param alloc        Allocator to use; malloc, realloc, and free must not be NULL.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if vector is NULL,
 *         element_size is 0, or any alloc function is NULL.
 */
dds_result_t dds_vector_init(dds_vector_t* vector, size_t element_size, dds_alloc_t alloc);

/**
 * Free resources held by a vector.
 *
 * Leaves the vector in a zeroed state. Safe to call with NULL.
 *
 * @param vector Pointer to the vector to free.
 */
void dds_vector_free(dds_vector_t* vector);

/**
 * Append an element to the end of the vector.
 *
 * Grows the internal buffer if necessary.
 *
 * @param vector  Pointer to an initialized vector.
 * @param element Pointer to the element to copy into the vector.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if any argument is NULL,
 *         DDS_OUT_OF_MEMORY if allocation fails.
 */
dds_result_t dds_vector_push_back(dds_vector_t* vector, const void* element);


/**
 * Remove the last element from the vector and copy it into the provided buffer.
 *
 * Does not shrink the internal buffer.
 *
 * @param vector  Pointer to an initialized vector.
 * @param element Destination buffer of at least element_size bytes, or NULL to discard.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if vector is NULL,
 *         DDS_OUT_OF_RANGE if the vector is empty.
 */
dds_result_t dds_vector_pop_back(dds_vector_t* vector, void* element);


/**
 * Copy an element at the given index into the provided buffer.
 *
 * @param vector  Pointer to an initialized vector.
 * @param index   Zero-based index of the element.
 * @param element Destination buffer of at least element_size bytes.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if vector or element is NULL,
 *         DDS_OUT_OF_RANGE if index is out of bounds.
 */
dds_result_t dds_vector_get(const dds_vector_t* vector, size_t index, void* element);

/**
 * Return a pointer to the element at the given index.
 *
 * The pointer is valid until the next operation that modifies the vector.
 *
 * @param vector Pointer to an initialized vector.
 * @param index  Zero-based index of the element.
 * @return Pointer to the element, or NULL if the vector is NULL or index is out of range.
 */
void* dds_vector_at(const dds_vector_t* vector, size_t index);

/**
 * Return a typed pointer to the element at the given index.
 *
 * Convenience macro that avoids manual casting.
 * No bounds checking is performed.
 *
 * @param vector Pointer to an initialized vector.
 * @param Type   Element type.
 * @param index  Zero-based index of the element.
 */
#define dds_vector_index(vector, Type, index) (((Type*)(vector)->data)[(index)])

/**
 * Return the number of elements currently stored in the vector.
 *
 * @param vector Pointer to an initialized vector.
 * @return Number of elements, or 0 if the vector is NULL.
 */
size_t dds_vector_get_size(const dds_vector_t* vector);

/**
 * Return the current capacity of the vector's internal buffer.
 *
 * @param vector Pointer to an initialized vector.
 * @return Capacity in elements, or 0 if the vector is NULL.
 */
size_t dds_vector_get_capacity(const dds_vector_t* vector);

/**
 * Return a pointer to the vector's internal data buffer.
 *
 * The pointer is valid until the next operation that modifies the vector.
 * Returns NULL if the vector is NULL or has never had any elements pushed.
 *
 * @param vector Pointer to an initialized vector.
 * @return Pointer to the raw data buffer, or NULL if the vector is NULL or empty.
 */
void* dds_vector_get_data(const dds_vector_t* vector);

#endif //DOLPHIN_DSA_VECTOR_H
