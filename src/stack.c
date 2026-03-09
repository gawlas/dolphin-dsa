#include "dds/stack.h"

dds_result_t dds_stack_init(dds_stack_t *stack, const size_t element_size, const dds_alloc_t alloc) {
    (void)stack; (void)element_size; (void)alloc;
    return DDS_OK;
}

void dds_stack_free(dds_stack_t *stack) {
    (void)stack;
}

dds_result_t dds_stack_clear(dds_stack_t *stack) {
    (void)stack;
    return DDS_OK;
}

dds_result_t dds_stack_reserve(dds_stack_t *stack, const size_t capacity) {
    (void)stack; (void)capacity;
    return DDS_OK;
}

dds_result_t dds_stack_shrink_to_fit(dds_stack_t *stack) {
    (void)stack;
    return DDS_OK;
}

dds_result_t dds_stack_push(dds_stack_t *stack, const void *element) {
    (void)stack; (void)element;
    return DDS_OK;
}

dds_result_t dds_stack_pop(dds_stack_t *stack, void *out_element) {
    (void)stack; (void)out_element;
    return DDS_OK;
}

dds_result_t dds_stack_peek(const dds_stack_t *stack, void *out_element) {
    (void)stack; (void)out_element;
    return DDS_OK;
}

size_t dds_stack_get_size(const dds_stack_t *stack) {
    (void)stack;
    return 0;
}

size_t dds_stack_get_capacity(const dds_stack_t *stack) {
    (void)stack;
    return 0;
}

bool dds_stack_is_empty(const dds_stack_t *stack) {
    (void)stack;
    return true;
}
