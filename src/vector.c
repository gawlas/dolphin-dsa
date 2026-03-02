#include <stdint.h>
#include <string.h>
#include "dds/vector.h"
#include "dds/types.h"

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

dds_result_t dds_vector_push_back(dds_vector_t* vector, const void* element) {
    if (vector == NULL) return DDS_INVALID_PARAMETER;
    if (element == NULL) return DDS_INVALID_PARAMETER;

    // resize data buffer when full
    if (vector->size >= vector->capacity) {
        if (vector->capacity > SIZE_MAX / GROWTH_FACTOR) return DDS_OVERFLOW;
        const size_t new_capacity = (vector->capacity == 0) ? INITIAL_SIZE : vector->capacity * GROWTH_FACTOR;

        if (new_capacity > SIZE_MAX / vector->element_size) return DDS_OVERFLOW;
        const size_t new_buffer_size = new_capacity * vector->element_size;

        void* new_buffer = vector->alloc.realloc(vector->alloc.context, vector->data, new_buffer_size);

        // if realloc fail, return error
        if (new_buffer == NULL) return DDS_OUT_OF_MEMORY;

        // update buffer
        vector->data = new_buffer;
        vector->capacity = new_capacity;
    }

    // calculate destination for a new element
    void* destination = (char*)vector->data + (vector->size * vector->element_size);

    // copy element to vector
    memcpy(destination, element, vector->element_size);

    // update vector size
    vector->size++;

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
