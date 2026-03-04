#include <stdint.h>
#include <string.h>
#include "dds/vector.h"
#include "dds/types.h"

/** Initial capacity allocated on first push. */
#define DDS_INITIAL_SIZE 1

/** Capacity multiplier applied on each resize. */
#define DDS_GROWTH_FACTOR 2

static dds_result_t grow_if_needed(dds_vector_t* vector) {
    if (vector->size >= vector->capacity) {
        if (vector->capacity > SIZE_MAX / DDS_GROWTH_FACTOR) return DDS_OVERFLOW;
        const size_t new_capacity = (vector->capacity == 0) ? DDS_INITIAL_SIZE : vector->capacity * DDS_GROWTH_FACTOR;

        if (new_capacity > SIZE_MAX / vector->element_size) return DDS_OVERFLOW;
        const size_t new_buffer_size = new_capacity * vector->element_size;

        void* new_buffer = vector->alloc.realloc(vector->alloc.context, vector->data, new_buffer_size);

        // if realloc fail, return error
        if (new_buffer == NULL) return DDS_OUT_OF_MEMORY;

        // update buffer
        vector->data = new_buffer;
        vector->capacity = new_capacity;
    }

    return DDS_OK;
}

dds_result_t dds_vector_init(dds_vector_t* vector, size_t element_size, dds_alloc_t alloc) {
    if (vector == NULL) return DDS_INVALID_PARAMETER;
    if (element_size == 0) return DDS_INVALID_PARAMETER;
    if (alloc.malloc == NULL || alloc.realloc == NULL || alloc.free == NULL) {
        return DDS_INVALID_PARAMETER;
    }

    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
    vector->element_size = element_size;
    vector->alloc = alloc;

    return DDS_OK;
}

void dds_vector_free(dds_vector_t* vector) {
    if (vector == NULL) return;

    if (vector->data != NULL && vector->alloc.free != NULL) {
        vector->alloc.free(vector->alloc.context, vector->data);
    }

    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
    vector->element_size = 0;
    memset(&vector->alloc, 0, sizeof(vector->alloc));
}

dds_result_t dds_vector_clear(dds_vector_t* vector) {
    if (vector == NULL) return DDS_INVALID_PARAMETER;

    vector->size = 0;

    return DDS_OK;
}

dds_result_t dds_vector_reserve(dds_vector_t* vector, const size_t capacity) {
    if (vector == NULL) return DDS_INVALID_PARAMETER;
    if (capacity == 0) return DDS_INVALID_PARAMETER;

    // resize data buffer when requested capacity greater than current capacity
    if (capacity > vector->capacity) {
        if (capacity > SIZE_MAX / vector->element_size) return DDS_OVERFLOW;
        const size_t new_buffer_size = capacity * vector->element_size;

        void* new_buffer = vector->alloc.realloc(vector->alloc.context, vector->data, new_buffer_size);

        // if realloc fail, return error
        if (new_buffer == NULL) return DDS_OUT_OF_MEMORY;

        // update buffer
        vector->data = new_buffer;
        vector->capacity = capacity;
    }

    return DDS_OK;
}

dds_result_t dds_vector_shrink_to_fit(dds_vector_t* vector) {
    if (vector == NULL) return DDS_INVALID_PARAMETER;
    if (vector->size == vector->capacity) return DDS_OK;

    if (vector->size == 0) {
        vector->alloc.free(vector->alloc.context, vector->data);
        vector->data = NULL;
        vector->capacity = 0;

        return DDS_OK;
    }

    const size_t new_capacity = vector->size;
    if (new_capacity > SIZE_MAX / vector->element_size) return DDS_OVERFLOW;
    const size_t new_buffer_size = new_capacity * vector->element_size;
    void* new_buffer = vector->alloc.realloc(vector->alloc.context, vector->data, new_buffer_size);

    if (new_buffer == NULL) return DDS_OUT_OF_MEMORY;

    vector->data = new_buffer;
    vector->capacity = new_capacity;

    return DDS_OK;
}


dds_result_t dds_vector_push_back(dds_vector_t* vector, const void* element) {
    if (vector == NULL) return DDS_INVALID_PARAMETER;
    if (element == NULL) return DDS_INVALID_PARAMETER;

    // resize data buffer when full
    const dds_result_t result = grow_if_needed(vector);
    if (result != DDS_OK) return result;

    // calculate destination for a new element
    void* destination = (char*)vector->data + (vector->size * vector->element_size);

    // copy element to vector
    memcpy(destination, element, vector->element_size);

    // update vector size
    vector->size++;

    return DDS_OK;
}

dds_result_t dds_vector_insert(dds_vector_t* vector, const size_t index, const void* element) {
    if (vector == NULL) return DDS_INVALID_PARAMETER;
    if (element == NULL) return DDS_INVALID_PARAMETER;
    if (index > vector->size) return DDS_OUT_OF_RANGE;

    // resize data buffer when full
    const dds_result_t result = grow_if_needed(vector);
    if (result != DDS_OK) return result;

    // move vector elements to make a space for new element
    void* source = (char*)vector->data + (index * vector->element_size);
    void* destination = (char*)source + vector->element_size;
    const size_t elements_to_move = vector->size - index;
    const size_t bytes_to_move = elements_to_move * vector->element_size;

    memmove(destination, source, bytes_to_move);

    // copy element to vector
    memcpy(source, element, vector->element_size);
    vector->size++;

    return DDS_OK;
}


dds_result_t dds_vector_remove(dds_vector_t* vector, const size_t index, void* element) {
    if (vector == NULL) return DDS_INVALID_PARAMETER;
    if (index >= vector->size) return DDS_OUT_OF_RANGE;

    // if the element is not NULL, copy element at index from vector to it, otherwise discard
    if (element != NULL) {
        const void* source = (char*)vector->data + (index * vector->element_size);
        memcpy(element, source, vector->element_size);
    }

    // move vector elements to fill the gap
    void* destination = (char*)vector->data + (index * vector->element_size);
    void* source = (char*)destination + vector->element_size;
    const size_t elements_to_move = vector->size - index - 1;
    const size_t bytes_to_move = elements_to_move * vector->element_size;

    memmove(destination, source, bytes_to_move);

    vector->size--;

    return DDS_OK;
}


dds_result_t dds_vector_pop_back(dds_vector_t* vector, void* element) {
    if (vector == NULL) return DDS_INVALID_PARAMETER;
    if (vector->size == 0) return DDS_OUT_OF_RANGE;

    // if the element is not NULL, copy the last element from vector to it, otherwise discard
    if (element != NULL) {
        const void* source = (char*)vector->data + ((vector->size - 1) * vector->element_size);
        memcpy(element, source, vector->element_size);
    }

    vector->size--;

    return DDS_OK;
}

dds_result_t dds_vector_get(const dds_vector_t* vector, size_t index, void* element) {
    if (vector == NULL) return DDS_INVALID_PARAMETER;
    if (element == NULL) return DDS_INVALID_PARAMETER;
    if (index >= vector->size) return DDS_OUT_OF_RANGE;

    void* source = (char*)vector->data + (index * vector->element_size);
    memcpy(element, source, vector->element_size);

    return DDS_OK;
}

void* dds_vector_at(const dds_vector_t* vector, size_t index) {
    if (vector == NULL) return NULL;
    if (index >= vector->size) return NULL;

    return (char*)vector->data + (index * vector->element_size);
}

size_t dds_vector_get_size(const dds_vector_t* vector) {
    if (vector == NULL) return 0;

    return vector->size;
}

size_t dds_vector_get_capacity(const dds_vector_t* vector) {
    if (vector == NULL) return 0;

    return vector->capacity;
}

void* dds_vector_get_data(const dds_vector_t* vector) {
    if (vector == NULL) return NULL;

    return vector->data;
}

bool dds_vector_is_empty(const dds_vector_t* vector) {
    if (vector == NULL) return true;

    return vector->size == 0;
}
