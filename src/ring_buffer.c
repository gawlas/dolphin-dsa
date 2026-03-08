#include <stdint.h>
#include <string.h>
#include "dds/ring_buffer.h"
#include "dds/types.h"

static void dds_ring_buffer_copy_to(const dds_ring_buffer_t *ring_buffer, void *dest) {
    if (ring_buffer->size == 0) return;

    const size_t end_idx = ring_buffer->head + ring_buffer->size;

    if (end_idx <= ring_buffer->capacity) {
        /* contiguous: head .. head+size-1 */
        memcpy(dest,
               (char *)ring_buffer->data + ring_buffer->head * ring_buffer->element_size,
               ring_buffer->size * ring_buffer->element_size);
    } else {
        /* wrapped: head..capacity-1, then 0..tail-1 */
        const size_t first_count = ring_buffer->capacity - ring_buffer->head;
        memcpy(dest,
               (char *)ring_buffer->data + ring_buffer->head * ring_buffer->element_size,
               first_count * ring_buffer->element_size);
        memcpy((char *)dest + first_count * ring_buffer->element_size,
               ring_buffer->data,
               ring_buffer->tail * ring_buffer->element_size);
    }
}

static size_t dds_ring_buffer_prev_index(const dds_ring_buffer_t *ring_buffer, const size_t index) {
    if (ring_buffer->capacity == 0) return 0;
    return index == 0 ? ring_buffer->capacity - 1 : index - 1;
}

static size_t dds_ring_buffer_next_index(const dds_ring_buffer_t *ring_buffer, const size_t index) {
    if (ring_buffer->capacity == 0) return 0;
    return index == ring_buffer->capacity - 1 ? 0 : index + 1;
}

static dds_result_t dds_ring_buffer_grow(dds_ring_buffer_t *ring_buffer) {
    if (ring_buffer->capacity > SIZE_MAX / DDS_RING_BUFFER_GROWTH_FACTOR) return DDS_OVERFLOW;
    const size_t new_capacity = ring_buffer->capacity == 0
                                    ? DDS_RING_BUFFER_INITIAL_SIZE
                                    : ring_buffer->capacity * DDS_RING_BUFFER_GROWTH_FACTOR;

    if (new_capacity > SIZE_MAX / ring_buffer->element_size) return DDS_OVERFLOW;
    const size_t new_buffer_size = new_capacity * ring_buffer->element_size;
    void *new_buffer = ring_buffer->alloc.malloc(ring_buffer->alloc.context, new_buffer_size);
    if (new_buffer == NULL) return DDS_OUT_OF_MEMORY;

    dds_ring_buffer_copy_to(ring_buffer, new_buffer);

    ring_buffer->alloc.free(ring_buffer->alloc.context, ring_buffer->data);
    ring_buffer->data = new_buffer;
    ring_buffer->capacity = new_capacity;
    ring_buffer->head = 0;
    ring_buffer->tail = ring_buffer->size;

    return DDS_OK;
}

dds_result_t dds_ring_buffer_init(dds_ring_buffer_t *ring_buffer, const size_t element_size, const dds_alloc_t alloc) {
    if (ring_buffer == NULL) return DDS_INVALID_PARAMETER;
    if (element_size == 0) return DDS_INVALID_PARAMETER;
    if (alloc.malloc == NULL || alloc.free == NULL) return DDS_INVALID_PARAMETER;

    ring_buffer->data = NULL;
    ring_buffer->tail = 0;
    ring_buffer->head = 0;
    ring_buffer->size = 0;
    ring_buffer->capacity = 0;
    ring_buffer->element_size = element_size;
    ring_buffer->alloc = alloc;

    return DDS_OK;
}

dds_result_t dds_ring_buffer_clear(dds_ring_buffer_t *ring_buffer) {
    if (ring_buffer == NULL) return DDS_INVALID_PARAMETER;

    ring_buffer->tail = 0;
    ring_buffer->head = 0;
    ring_buffer->size = 0;

    return DDS_OK;
}

void dds_ring_buffer_free(dds_ring_buffer_t *ring_buffer) {
    if (ring_buffer == NULL) return;

    ring_buffer->alloc.free(ring_buffer->alloc.context, ring_buffer->data);

    ring_buffer->data = NULL;
    ring_buffer->tail = 0;
    ring_buffer->head = 0;
    ring_buffer->size = 0;
    ring_buffer->capacity = 0;
    ring_buffer->element_size = 0;
    memset(&ring_buffer->alloc, 0, sizeof(ring_buffer->alloc));
}

dds_result_t dds_ring_buffer_reserve(dds_ring_buffer_t *ring_buffer, const size_t capacity) {
    if (ring_buffer == NULL) return DDS_INVALID_PARAMETER;
    if (capacity == 0) return DDS_INVALID_PARAMETER;
    if (capacity <= ring_buffer->capacity) return DDS_OK;

    if (capacity > SIZE_MAX / ring_buffer->element_size) return DDS_OVERFLOW;
    const size_t new_buffer_size = capacity * ring_buffer->element_size;
    void *new_buffer = ring_buffer->alloc.malloc(ring_buffer->alloc.context, new_buffer_size);
    if (new_buffer == NULL) return DDS_OUT_OF_MEMORY;

    dds_ring_buffer_copy_to(ring_buffer, new_buffer);

    ring_buffer->alloc.free(ring_buffer->alloc.context, ring_buffer->data);
    ring_buffer->data = new_buffer;
    ring_buffer->capacity = capacity;
    ring_buffer->head = 0;
    ring_buffer->tail = ring_buffer->size;

    return DDS_OK;
}

dds_result_t dds_ring_buffer_shrink_to_fit(dds_ring_buffer_t *ring_buffer) {
    if (ring_buffer == NULL) return DDS_INVALID_PARAMETER;
    if (ring_buffer->size == ring_buffer->capacity) return DDS_OK;

    if (ring_buffer->size == 0) {
        ring_buffer->alloc.free(ring_buffer->alloc.context, ring_buffer->data);
        ring_buffer->data = NULL;
        ring_buffer->capacity = 0;
        ring_buffer->head = 0;
        ring_buffer->tail = 0;
        return DDS_OK;
    }

    if (ring_buffer->size > SIZE_MAX / ring_buffer->element_size) return DDS_OVERFLOW;
    const size_t new_buffer_size = ring_buffer->size * ring_buffer->element_size;
    void *new_buffer = ring_buffer->alloc.malloc(ring_buffer->alloc.context, new_buffer_size);
    if (new_buffer == NULL) return DDS_OUT_OF_MEMORY;

    dds_ring_buffer_copy_to(ring_buffer, new_buffer);

    ring_buffer->alloc.free(ring_buffer->alloc.context, ring_buffer->data);
    ring_buffer->data = new_buffer;
    ring_buffer->capacity = ring_buffer->size;
    ring_buffer->head = 0;
    ring_buffer->tail = ring_buffer->size;

    return DDS_OK;
}

dds_result_t dds_ring_buffer_push_back(dds_ring_buffer_t *ring_buffer, const void *element) {
    if (ring_buffer == NULL) return DDS_INVALID_PARAMETER;
    if (element == NULL) return DDS_INVALID_PARAMETER;

    if (ring_buffer->size >= ring_buffer->capacity) {
        const dds_result_t result = dds_ring_buffer_grow(ring_buffer);
        if (result != DDS_OK) return result;
    }

    void *destination = (char *)ring_buffer->data + ring_buffer->tail * ring_buffer->element_size;

    memcpy(destination, element, ring_buffer->element_size);
    ring_buffer->tail = dds_ring_buffer_next_index(ring_buffer, ring_buffer->tail);
    ring_buffer->size++;

    return DDS_OK;
}

dds_result_t dds_ring_buffer_push_front(dds_ring_buffer_t *ring_buffer, const void *element) {
    if (ring_buffer == NULL) return DDS_INVALID_PARAMETER;
    if (element == NULL) return DDS_INVALID_PARAMETER;

    if (ring_buffer->size >= ring_buffer->capacity) {
        const dds_result_t result = dds_ring_buffer_grow(ring_buffer);
        if (result != DDS_OK) return result;
    }

    const size_t index = dds_ring_buffer_prev_index(ring_buffer, ring_buffer->head);
    void *destination = (char *)ring_buffer->data + index * ring_buffer->element_size;
    memcpy(destination, element, ring_buffer->element_size);

    ring_buffer->head = index;
    ring_buffer->size++;

    return DDS_OK;
}

dds_result_t dds_ring_buffer_pop_back(dds_ring_buffer_t *ring_buffer, void *out_element) {
    if (ring_buffer == NULL) return DDS_INVALID_PARAMETER;
    if (ring_buffer->size == 0) return DDS_OUT_OF_RANGE;

    const size_t index = dds_ring_buffer_prev_index(ring_buffer, ring_buffer->tail);

    if (out_element != NULL) {
        const void *source = (char *)ring_buffer->data + index * ring_buffer->element_size;
        memcpy(out_element, source, ring_buffer->element_size);
    }

    ring_buffer->tail = index;
    ring_buffer->size--;

    return DDS_OK;
}

dds_result_t dds_ring_buffer_peek_back(const dds_ring_buffer_t *ring_buffer, void *out_element) {
    if (ring_buffer == NULL) return DDS_INVALID_PARAMETER;
    if (out_element == NULL) return DDS_INVALID_PARAMETER;

    if (ring_buffer->size == 0) return DDS_OUT_OF_RANGE;

    const size_t index = dds_ring_buffer_prev_index(ring_buffer, ring_buffer->tail);
    const void *source = (char *)ring_buffer->data + index * ring_buffer->element_size;

    memcpy(out_element, source, ring_buffer->element_size);

    return DDS_OK;
}

dds_result_t dds_ring_buffer_pop_front(dds_ring_buffer_t *ring_buffer, void *out_element) {
    if (ring_buffer == NULL) return DDS_INVALID_PARAMETER;
    if (ring_buffer->size == 0) return DDS_OUT_OF_RANGE;

    if (out_element != NULL) {
        const void *source = (char *)ring_buffer->data + ring_buffer->head * ring_buffer->element_size;
        memcpy(out_element, source, ring_buffer->element_size);
    }

    ring_buffer->head = dds_ring_buffer_next_index(ring_buffer, ring_buffer->head);
    ring_buffer->size--;

    return DDS_OK;
}

dds_result_t dds_ring_buffer_peek_front(const dds_ring_buffer_t *ring_buffer, void *out_element) {
    if (ring_buffer == NULL) return DDS_INVALID_PARAMETER;
    if (out_element == NULL) return DDS_INVALID_PARAMETER;

    if (ring_buffer->size == 0) return DDS_OUT_OF_RANGE;

    const void *source = (char *)ring_buffer->data + ring_buffer->head * ring_buffer->element_size;

    memcpy(out_element, source, ring_buffer->element_size);

    return DDS_OK;
}

size_t dds_ring_buffer_get_size(const dds_ring_buffer_t *ring_buffer) {
    if (ring_buffer == NULL) return 0;

    return ring_buffer->size;
}

size_t dds_ring_buffer_get_capacity(const dds_ring_buffer_t *ring_buffer) {
    if (ring_buffer == NULL) return 0;

    return ring_buffer->capacity;
}

void *dds_ring_buffer_get_data(const dds_ring_buffer_t *ring_buffer) {
    if (ring_buffer == NULL) return NULL;

    return ring_buffer->data;
}

bool dds_ring_buffer_is_empty(const dds_ring_buffer_t *ring_buffer) {
    if (ring_buffer == NULL) return true;

    return ring_buffer->size == 0;
}
