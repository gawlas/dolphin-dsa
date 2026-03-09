#include "dds/stack.h"

dds_result_t dds_stack_init(dds_stack_t *stack, const size_t element_size, const dds_alloc_t alloc) {
    if (stack == NULL) return DDS_INVALID_PARAMETER;
    if (element_size == 0) return DDS_INVALID_PARAMETER;
    if (alloc.malloc == NULL || alloc.free == NULL) return DDS_INVALID_PARAMETER;

    dds_vector_init(&stack->vector, element_size, alloc);

    return DDS_OK;
}

void dds_stack_free(dds_stack_t *stack) {
    if (stack == NULL) return;

    dds_vector_free(&stack->vector);
}

dds_result_t dds_stack_clear(dds_stack_t *stack) {
    if (stack == NULL) return DDS_INVALID_PARAMETER;

    return dds_vector_clear(&stack->vector);
}

dds_result_t dds_stack_reserve(dds_stack_t *stack, const size_t capacity) {
    if (stack == NULL) return DDS_INVALID_PARAMETER;
    if (capacity == 0) return DDS_INVALID_PARAMETER;

    return dds_vector_reserve(&stack->vector, capacity);
}

dds_result_t dds_stack_shrink_to_fit(dds_stack_t *stack) {
    if (stack == NULL) return DDS_INVALID_PARAMETER;

    return dds_vector_shrink_to_fit(&stack->vector);
}

dds_result_t dds_stack_push(dds_stack_t *stack, const void *element) {
    if (stack == NULL) return DDS_INVALID_PARAMETER;
    if (element == NULL) return DDS_INVALID_PARAMETER;

    return dds_vector_push_back(&stack->vector, element);
}

dds_result_t dds_stack_pop(dds_stack_t *stack, void *out_element) {
    if (stack == NULL) return DDS_INVALID_PARAMETER;
    if (dds_vector_get_size(&stack->vector) == 0) return DDS_OUT_OF_RANGE;

    return dds_vector_pop_back(&stack->vector, out_element);
}

dds_result_t dds_stack_peek(const dds_stack_t *stack, void *out_element) {
    if (stack == NULL) return DDS_INVALID_PARAMETER;
    if (dds_vector_get_size(&stack->vector) == 0) return DDS_OUT_OF_RANGE;

    return dds_vector_get(&stack->vector, dds_vector_get_size(&stack->vector) - 1, out_element);
}

size_t dds_stack_get_size(const dds_stack_t *stack) {
    if (stack == NULL) return 0;

    return dds_vector_get_size(&stack->vector);
}

size_t dds_stack_get_capacity(const dds_stack_t *stack) {
    if (stack == NULL) return 0;

    return dds_vector_get_capacity(&stack->vector);
}

bool dds_stack_is_empty(const dds_stack_t *stack) {
    return dds_vector_is_empty(&stack->vector);
}
