#include "dds/queue.h"

dds_result_t dds_queue_init(dds_queue_t *queue, const size_t element_size, const dds_alloc_t alloc) {
    if (queue == NULL) return DDS_INVALID_PARAMETER;

    return dds_ring_buffer_init(&queue->ring_buffer, element_size, alloc);
}

void dds_queue_free(dds_queue_t *queue) {
    if (queue == NULL) return;

    dds_ring_buffer_free(&queue->ring_buffer);
}

dds_result_t dds_queue_clear(dds_queue_t *queue) {
    if (queue == NULL) return DDS_INVALID_PARAMETER;

    return dds_ring_buffer_clear(&queue->ring_buffer);
}

dds_result_t dds_queue_reserve(dds_queue_t *queue, const size_t capacity) {
    if (queue == NULL) return DDS_INVALID_PARAMETER;

    return dds_ring_buffer_reserve(&queue->ring_buffer, capacity);
}

dds_result_t dds_queue_shrink_to_fit(dds_queue_t *queue) {
    if (queue == NULL) return DDS_INVALID_PARAMETER;

    return dds_ring_buffer_shrink_to_fit(&queue->ring_buffer);
}

dds_result_t dds_queue_enqueue(dds_queue_t *queue, const void *element) {
    if (queue == NULL) return DDS_INVALID_PARAMETER;

    return dds_ring_buffer_push_back(&queue->ring_buffer, element);
}

dds_result_t dds_queue_dequeue(dds_queue_t *queue, void *out_element) {
    if (queue == NULL) return DDS_INVALID_PARAMETER;

    return dds_ring_buffer_pop_front(&queue->ring_buffer, out_element);
}

dds_result_t dds_queue_peek_front(const dds_queue_t *queue, void *out_element) {
    if (queue == NULL) return DDS_INVALID_PARAMETER;

    return dds_ring_buffer_peek_front(&queue->ring_buffer, out_element);
}

size_t dds_queue_get_size(const dds_queue_t *queue) {
    if (queue == NULL) return 0;

    return dds_ring_buffer_get_size(&queue->ring_buffer);
}

size_t dds_queue_get_capacity(const dds_queue_t *queue) {
    if (queue == NULL) return 0;

    return dds_ring_buffer_get_capacity(&queue->ring_buffer);
}

bool dds_queue_is_empty(const dds_queue_t *queue) {
    if (queue == NULL) return true;

    return dds_ring_buffer_is_empty(&queue->ring_buffer);
}
