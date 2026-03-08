#include "dds/ring_buffer.h"
#include "unity.h"
#include "test_structs.h"

/* dds_ring_buffer_init */

void dds_ring_buffer_init_should_return_ok(void) {
    dds_ring_buffer_t rb;
    const dds_result_t result = dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_init_should_return_invalid_parameter_when_ring_buffer_null(void) {
    const dds_result_t result = dds_ring_buffer_init(NULL, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_ring_buffer_init_should_return_invalid_parameter_when_element_size_zero(void) {
    dds_ring_buffer_t rb;
    const dds_result_t result = dds_ring_buffer_init(&rb, 0, dds_alloc_stdlib());

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_ring_buffer_init_should_return_invalid_parameter_when_alloc_invalid(void) {
    dds_ring_buffer_t rb;
    dds_alloc_t alloc = { .malloc = NULL, .realloc = NULL, .free = NULL, .context = NULL };
    const dds_result_t result = dds_ring_buffer_init(&rb, sizeof(int), alloc);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_ring_buffer_init_should_create_correct_structure(void) {
    const size_t element_size = sizeof(int);
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, element_size, dds_alloc_stdlib());

    TEST_ASSERT_NULL(rb.data);
    TEST_ASSERT_EQUAL_size_t(0, dds_ring_buffer_get_size(&rb));
    TEST_ASSERT_EQUAL_size_t(0, rb.head);
    TEST_ASSERT_EQUAL_size_t(0, rb.capacity);
    TEST_ASSERT_EQUAL_size_t(element_size, rb.element_size);

    dds_ring_buffer_free(&rb);
}

/* dds_ring_buffer_free */

void dds_ring_buffer_free_should_not_crash_when_null(void) {
    dds_ring_buffer_free(NULL);
}

void dds_ring_buffer_free_should_zero_fields(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());
    dds_ring_buffer_free(&rb);

    TEST_ASSERT_NULL(rb.data);
    TEST_ASSERT_EQUAL_size_t(0, dds_ring_buffer_get_size(&rb));
    TEST_ASSERT_EQUAL_size_t(0, rb.head);
    TEST_ASSERT_EQUAL_size_t(0, rb.capacity);
    TEST_ASSERT_EQUAL_size_t(0, rb.element_size);
    TEST_ASSERT_NULL(rb.alloc.malloc);
    TEST_ASSERT_NULL(rb.alloc.realloc);
    TEST_ASSERT_NULL(rb.alloc.free);
}

void dds_ring_buffer_free_should_release_buffer(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    int element = 99;
    dds_ring_buffer_push_back(&rb, &element);

    TEST_ASSERT_NOT_NULL(rb.data);

    dds_ring_buffer_free(&rb);

    TEST_ASSERT_NULL(rb.data);
}

/* dds_ring_buffer_clear */

void dds_ring_buffer_clear_should_return_ok(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const dds_result_t result = dds_ring_buffer_clear(&rb);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_clear_should_return_invalid_parameter_when_ring_buffer_null(void) {
    const dds_result_t result = dds_ring_buffer_clear(NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_ring_buffer_clear_should_reset_size_to_zero(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_ring_buffer_push_back(&rb, &values[i]);

    dds_ring_buffer_clear(&rb);

    TEST_ASSERT_EQUAL_size_t(0, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_clear_should_reset_head_to_zero(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_ring_buffer_push_back(&rb, &values[i]);
    dds_ring_buffer_pop_front(&rb, NULL);

    dds_ring_buffer_clear(&rb);

    TEST_ASSERT_EQUAL_size_t(0, rb.head);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_clear_should_preserve_capacity(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_ring_buffer_push_back(&rb, &values[i]);

    const size_t capacity_before = dds_ring_buffer_get_capacity(&rb);
    dds_ring_buffer_clear(&rb);

    TEST_ASSERT_EQUAL_size_t(capacity_before, dds_ring_buffer_get_capacity(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_clear_should_preserve_element_size_and_alloc(void) {
    const size_t element_size = sizeof(int);
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, element_size, dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);

    dds_ring_buffer_clear(&rb);

    TEST_ASSERT_EQUAL_size_t(element_size, rb.element_size);
    TEST_ASSERT_NOT_NULL(rb.alloc.malloc);
    TEST_ASSERT_NOT_NULL(rb.alloc.free);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_clear_should_allow_push_after_clear(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);
    dds_ring_buffer_clear(&rb);

    const int new_value = 99;
    dds_ring_buffer_push_back(&rb, &new_value);

    int out;
    dds_ring_buffer_peek_back(&rb, &out);
    TEST_ASSERT_EQUAL_INT(99, out);
    TEST_ASSERT_EQUAL_size_t(1, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

/* dds_ring_buffer_reserve */

void dds_ring_buffer_reserve_should_return_ok(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const dds_result_t result = dds_ring_buffer_reserve(&rb, 16);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_reserve_should_return_invalid_parameter_when_ring_buffer_null(void) {
    const dds_result_t result = dds_ring_buffer_reserve(NULL, 16);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_ring_buffer_reserve_should_return_invalid_parameter_when_capacity_zero(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const dds_result_t result = dds_ring_buffer_reserve(&rb, 0);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_reserve_should_set_capacity(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    dds_ring_buffer_reserve(&rb, 16);

    TEST_ASSERT_EQUAL_size_t(16, dds_ring_buffer_get_capacity(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_reserve_should_not_change_size(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);
    dds_ring_buffer_reserve(&rb, 16);

    TEST_ASSERT_EQUAL_size_t(1, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_reserve_should_be_noop_when_capacity_already_sufficient(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    dds_ring_buffer_reserve(&rb, 16);
    const dds_result_t result = dds_ring_buffer_reserve(&rb, 8);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    TEST_ASSERT_EQUAL_size_t(16, dds_ring_buffer_get_capacity(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_reserve_should_preserve_elements(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_ring_buffer_push_back(&rb, &values[i]);

    dds_ring_buffer_reserve(&rb, 16);

    int out;
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(30, out);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_reserve_should_preserve_elements_when_data_is_wrapped(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    /* push 5, pop 3 → head=3, tail=5; push 4 → tail wraps to 1 */
    const int first[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) dds_ring_buffer_push_back(&rb, &first[i]);
    for (int i = 0; i < 3; i++) dds_ring_buffer_pop_front(&rb, NULL);
    const int second[] = {60, 70, 80, 90};
    for (int i = 0; i < 4; i++) dds_ring_buffer_push_back(&rb, &second[i]);

    /* reserve linearizes the wrapped data */
    dds_ring_buffer_reserve(&rb, 32);

    TEST_ASSERT_EQUAL_size_t(6, dds_ring_buffer_get_size(&rb));
    int out;
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(40, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(50, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(60, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(70, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(80, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(90, out);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_reserve_should_return_overflow_on_buffer_size_overflow(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    rb.element_size = SIZE_MAX / 2 + 1;
    const dds_result_t result = dds_ring_buffer_reserve(&rb, 2);

    TEST_ASSERT_EQUAL_INT(DDS_OVERFLOW, result);
    rb.element_size = sizeof(int);
    dds_ring_buffer_free(&rb);
}

/* dds_ring_buffer_shrink_to_fit */

void dds_ring_buffer_shrink_to_fit_should_return_ok(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    dds_ring_buffer_reserve(&rb, 16);
    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);

    const dds_result_t result = dds_ring_buffer_shrink_to_fit(&rb);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_shrink_to_fit_should_return_invalid_parameter_when_ring_buffer_null(void) {
    const dds_result_t result = dds_ring_buffer_shrink_to_fit(NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_ring_buffer_shrink_to_fit_should_set_capacity_to_size(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    dds_ring_buffer_reserve(&rb, 16);
    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);

    dds_ring_buffer_shrink_to_fit(&rb);

    TEST_ASSERT_EQUAL_size_t(1, dds_ring_buffer_get_capacity(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_shrink_to_fit_should_not_change_size(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    dds_ring_buffer_reserve(&rb, 16);
    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);

    dds_ring_buffer_shrink_to_fit(&rb);

    TEST_ASSERT_EQUAL_size_t(1, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_shrink_to_fit_should_preserve_elements(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_ring_buffer_push_back(&rb, &values[i]);
    dds_ring_buffer_reserve(&rb, 16);

    dds_ring_buffer_shrink_to_fit(&rb);

    int out;
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(30, out);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_shrink_to_fit_should_free_buffer_when_empty(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    dds_ring_buffer_reserve(&rb, 16);
    dds_ring_buffer_shrink_to_fit(&rb);

    TEST_ASSERT_NULL(dds_ring_buffer_get_data(&rb));
    TEST_ASSERT_EQUAL_size_t(0, dds_ring_buffer_get_capacity(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_shrink_to_fit_should_be_noop_when_already_fitted(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);

    /* after first push capacity == INITIAL_SIZE, size == 1; capacity > size */
    dds_ring_buffer_reserve(&rb, 1);
    const dds_result_t result = dds_ring_buffer_shrink_to_fit(&rb);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    TEST_ASSERT_EQUAL_size_t(1, dds_ring_buffer_get_capacity(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_shrink_to_fit_should_preserve_elements_when_data_is_wrapped(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    /* create wrapped state: push 5, pop 3, push 4 → tail wraps */
    const int first[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) dds_ring_buffer_push_back(&rb, &first[i]);
    for (int i = 0; i < 3; i++) dds_ring_buffer_pop_front(&rb, NULL);
    const int second[] = {60, 70, 80, 90};
    for (int i = 0; i < 4; i++) dds_ring_buffer_push_back(&rb, &second[i]);

    /* reserve first to leave excess capacity, then shrink */
    dds_ring_buffer_reserve(&rb, 32);
    dds_ring_buffer_shrink_to_fit(&rb);

    TEST_ASSERT_EQUAL_size_t(6, dds_ring_buffer_get_capacity(&rb));
    int out;
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(40, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(50, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(60, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(70, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(80, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(90, out);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_shrink_to_fit_should_return_overflow_on_buffer_size_overflow(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    rb.element_size = SIZE_MAX / 2 + 1;
    rb.size = 2;
    rb.capacity = 4;

    const dds_result_t result = dds_ring_buffer_shrink_to_fit(&rb);

    TEST_ASSERT_EQUAL_INT(DDS_OVERFLOW, result);
    rb.element_size = sizeof(int);
    rb.size = 0;
    rb.capacity = 0;
    dds_ring_buffer_free(&rb);
}

/* dds_ring_buffer_push_back */

void dds_ring_buffer_push_back_should_return_ok(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    const dds_result_t result = dds_ring_buffer_push_back(&rb, &value);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_push_back_should_return_invalid_parameter_when_ring_buffer_null(void) {
    const int value = 1;
    const dds_result_t result = dds_ring_buffer_push_back(NULL, &value);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_ring_buffer_push_back_should_return_invalid_parameter_when_element_null(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const dds_result_t result = dds_ring_buffer_push_back(&rb, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_push_back_should_increase_size(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_ring_buffer_push_back(&rb, &values[i]);

    TEST_ASSERT_EQUAL_size_t(3, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_push_back_should_store_correct_values(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_ring_buffer_push_back(&rb, &values[i]);

    int out;
    dds_ring_buffer_pop_front(&rb, &out);
    TEST_ASSERT_EQUAL_INT(10, out);
    dds_ring_buffer_pop_front(&rb, &out);
    TEST_ASSERT_EQUAL_INT(20, out);
    dds_ring_buffer_pop_front(&rb, &out);
    TEST_ASSERT_EQUAL_INT(30, out);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_push_back_should_grow_capacity(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    for (int i = 0; i < DDS_RING_BUFFER_INITIAL_SIZE + 1; i++) {
        dds_ring_buffer_push_back(&rb, &i);
    }

    TEST_ASSERT_EQUAL_size_t(DDS_RING_BUFFER_INITIAL_SIZE + 1, dds_ring_buffer_get_size(&rb));
    TEST_ASSERT(dds_ring_buffer_get_capacity(&rb) > DDS_RING_BUFFER_INITIAL_SIZE);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_push_back_should_store_struct(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(test_record_t), dds_alloc_stdlib());

    const test_record_t rec = make_record(42, "test", 3.14);
    dds_ring_buffer_push_back(&rb, &rec);

    test_record_t out;
    dds_ring_buffer_pop_front(&rb, &out);
    TEST_ASSERT_TRUE(records_equal(&rec, &out));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_push_back_should_maintain_fifo_order_after_tail_wraps(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    /* push 5, pop 3 → head=3, tail=5, cap=8 */
    const int first[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) dds_ring_buffer_push_back(&rb, &first[i]);
    for (int i = 0; i < 3; i++) dds_ring_buffer_pop_front(&rb, NULL);

    /* push 4 more: tail advances 5→6→7→0(wrap)→1, head=3 */
    const int second[] = {60, 70, 80, 90};
    for (int i = 0; i < 4; i++) dds_ring_buffer_push_back(&rb, &second[i]);

    /* 6 elements in buffer: 40, 50, 60, 70, 80, 90 */
    TEST_ASSERT_EQUAL_size_t(6, dds_ring_buffer_get_size(&rb));

    int out;
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(40, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(50, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(60, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(70, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(80, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(90, out);
    dds_ring_buffer_free(&rb);
}

/* dds_ring_buffer_push_front */

void dds_ring_buffer_push_front_should_return_ok(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    const dds_result_t result = dds_ring_buffer_push_front(&rb, &value);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_push_front_should_return_invalid_parameter_when_ring_buffer_null(void) {
    const int value = 1;
    const dds_result_t result = dds_ring_buffer_push_front(NULL, &value);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_ring_buffer_push_front_should_return_invalid_parameter_when_element_null(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const dds_result_t result = dds_ring_buffer_push_front(&rb, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_push_front_should_increase_size(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_ring_buffer_push_front(&rb, &values[i]);

    TEST_ASSERT_EQUAL_size_t(3, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_push_front_should_store_correct_values(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int a = 1, b = 2, c = 3;
    dds_ring_buffer_push_front(&rb, &a);
    dds_ring_buffer_push_front(&rb, &b);
    dds_ring_buffer_push_front(&rb, &c);

    int out;
    dds_ring_buffer_pop_front(&rb, &out);
    TEST_ASSERT_EQUAL_INT(3, out);
    dds_ring_buffer_pop_front(&rb, &out);
    TEST_ASSERT_EQUAL_INT(2, out);
    dds_ring_buffer_pop_front(&rb, &out);
    TEST_ASSERT_EQUAL_INT(1, out);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_push_front_should_store_struct(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(test_record_t), dds_alloc_stdlib());

    const test_record_t rec = make_record(7, "front", 2.71);
    dds_ring_buffer_push_front(&rb, &rec);

    test_record_t out;
    dds_ring_buffer_pop_front(&rb, &out);
    TEST_ASSERT_TRUE(records_equal(&rec, &out));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_push_front_should_wrap_head_backward(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    /* push_back one element → capacity=8, head=0, tail=1 */
    const int back = 100;
    dds_ring_buffer_push_back(&rb, &back);

    /* push_front 3 times: head wraps 0→7→6→5 */
    const int a = 10, b = 20, c = 30;
    dds_ring_buffer_push_front(&rb, &a); /* data[7]=10, head=7 */
    dds_ring_buffer_push_front(&rb, &b); /* data[6]=20, head=6 */
    dds_ring_buffer_push_front(&rb, &c); /* data[5]=30, head=5 */

    TEST_ASSERT_EQUAL_size_t(4, dds_ring_buffer_get_size(&rb));

    /* pop order from front: 30, 20, 10, 100 */
    int out;
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(30, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(100, out);
    dds_ring_buffer_free(&rb);
}

/* dds_ring_buffer_pop_back */

void dds_ring_buffer_pop_back_should_return_ok(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);

    int out;
    const dds_result_t result = dds_ring_buffer_pop_back(&rb, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_pop_back_should_return_invalid_parameter_when_ring_buffer_null(void) {
    int out;
    const dds_result_t result = dds_ring_buffer_pop_back(NULL, &out);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_ring_buffer_pop_back_should_return_out_of_range_when_empty(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    int out;
    const dds_result_t result = dds_ring_buffer_pop_back(&rb, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_pop_back_should_return_correct_element(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int a = 10, b = 20;
    dds_ring_buffer_push_back(&rb, &a);
    dds_ring_buffer_push_back(&rb, &b);

    int out;
    dds_ring_buffer_pop_back(&rb, &out);
    TEST_ASSERT_EQUAL_INT(20, out);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_pop_back_should_decrease_size(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int a = 1, b = 2;
    dds_ring_buffer_push_back(&rb, &a);
    dds_ring_buffer_push_back(&rb, &b);

    dds_ring_buffer_pop_back(&rb, NULL);
    TEST_ASSERT_EQUAL_size_t(1, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_pop_back_should_discard_when_element_null(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 99;
    dds_ring_buffer_push_back(&rb, &value);

    const dds_result_t result = dds_ring_buffer_pop_back(&rb, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    TEST_ASSERT_EQUAL_size_t(0, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_pop_back_should_return_elements_in_lifo_order(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_ring_buffer_push_back(&rb, &values[i]);

    int out;
    dds_ring_buffer_pop_back(&rb, &out);
    TEST_ASSERT_EQUAL_INT(3, out);
    dds_ring_buffer_pop_back(&rb, &out);
    TEST_ASSERT_EQUAL_INT(2, out);
    dds_ring_buffer_pop_back(&rb, &out);
    TEST_ASSERT_EQUAL_INT(1, out);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_pop_back_should_leave_empty_ring_buffer_consistent(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);
    dds_ring_buffer_pop_back(&rb, NULL);

    TEST_ASSERT_EQUAL_size_t(0, dds_ring_buffer_get_size(&rb));
    TEST_ASSERT_TRUE(dds_ring_buffer_is_empty(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_pop_back_should_return_struct(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(test_record_t), dds_alloc_stdlib());

    const test_record_t rec = make_record(5, "pop_back", 1.41);
    dds_ring_buffer_push_back(&rb, &rec);

    test_record_t out;
    dds_ring_buffer_pop_back(&rb, &out);
    TEST_ASSERT_TRUE(records_equal(&rec, &out));
    dds_ring_buffer_free(&rb);
}

/* dds_ring_buffer_pop_front */

void dds_ring_buffer_pop_front_should_return_ok(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);

    int out;
    const dds_result_t result = dds_ring_buffer_pop_front(&rb, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_pop_front_should_return_invalid_parameter_when_ring_buffer_null(void) {
    int out;
    const dds_result_t result = dds_ring_buffer_pop_front(NULL, &out);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_ring_buffer_pop_front_should_return_out_of_range_when_empty(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    int out;
    const dds_result_t result = dds_ring_buffer_pop_front(&rb, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_pop_front_should_return_correct_element(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int a = 10, b = 20;
    dds_ring_buffer_push_back(&rb, &a);
    dds_ring_buffer_push_back(&rb, &b);

    int out;
    dds_ring_buffer_pop_front(&rb, &out);
    TEST_ASSERT_EQUAL_INT(10, out);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_pop_front_should_decrease_size(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int a = 1, b = 2;
    dds_ring_buffer_push_back(&rb, &a);
    dds_ring_buffer_push_back(&rb, &b);

    dds_ring_buffer_pop_front(&rb, NULL);
    TEST_ASSERT_EQUAL_size_t(1, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_pop_front_should_discard_when_element_null(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 99;
    dds_ring_buffer_push_back(&rb, &value);

    const dds_result_t result = dds_ring_buffer_pop_front(&rb, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    TEST_ASSERT_EQUAL_size_t(0, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_pop_front_should_return_elements_in_fifo_order(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_ring_buffer_push_back(&rb, &values[i]);

    int out;
    dds_ring_buffer_pop_front(&rb, &out);
    TEST_ASSERT_EQUAL_INT(1, out);
    dds_ring_buffer_pop_front(&rb, &out);
    TEST_ASSERT_EQUAL_INT(2, out);
    dds_ring_buffer_pop_front(&rb, &out);
    TEST_ASSERT_EQUAL_INT(3, out);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_pop_front_should_leave_empty_ring_buffer_consistent(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);
    dds_ring_buffer_pop_front(&rb, NULL);

    TEST_ASSERT_EQUAL_size_t(0, dds_ring_buffer_get_size(&rb));
    TEST_ASSERT_TRUE(dds_ring_buffer_is_empty(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_pop_front_should_return_struct(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(test_record_t), dds_alloc_stdlib());

    const test_record_t rec = make_record(3, "pop_front", 1.73);
    dds_ring_buffer_push_back(&rb, &rec);

    test_record_t out;
    dds_ring_buffer_pop_front(&rb, &out);
    TEST_ASSERT_TRUE(records_equal(&rec, &out));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_should_behave_as_queue_with_interleaved_push_pop(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    int out;
    const int a=1, b=2, c=3, d=4, e=5;

    dds_ring_buffer_push_back(&rb, &a);
    dds_ring_buffer_push_back(&rb, &b);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(1, out);
    dds_ring_buffer_push_back(&rb, &c);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(2, out);
    dds_ring_buffer_push_back(&rb, &d);
    dds_ring_buffer_push_back(&rb, &e);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(3, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(4, out);
    dds_ring_buffer_pop_front(&rb, &out); TEST_ASSERT_EQUAL_INT(5, out);
    TEST_ASSERT_TRUE(dds_ring_buffer_is_empty(&rb));
    dds_ring_buffer_free(&rb);
}

/* dds_ring_buffer_peek_back */

void dds_ring_buffer_peek_back_should_return_ok(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);

    int out;
    const dds_result_t result = dds_ring_buffer_peek_back(&rb, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_peek_back_should_return_invalid_parameter_when_ring_buffer_null(void) {
    int out;
    const dds_result_t result = dds_ring_buffer_peek_back(NULL, &out);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_ring_buffer_peek_back_should_return_invalid_parameter_when_element_null(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);

    const dds_result_t result = dds_ring_buffer_peek_back(&rb, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_peek_back_should_return_out_of_range_when_empty(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    int out;
    const dds_result_t result = dds_ring_buffer_peek_back(&rb, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_peek_back_should_return_correct_element(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int a = 10, b = 20, c = 30;
    dds_ring_buffer_push_back(&rb, &a);
    dds_ring_buffer_push_back(&rb, &b);
    dds_ring_buffer_push_back(&rb, &c);

    int out;
    dds_ring_buffer_peek_back(&rb, &out);
    TEST_ASSERT_EQUAL_INT(30, out);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_peek_back_should_not_modify_ring_buffer(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 42;
    dds_ring_buffer_push_back(&rb, &value);

    const size_t size_before = dds_ring_buffer_get_size(&rb);
    int out;
    dds_ring_buffer_peek_back(&rb, &out);

    TEST_ASSERT_EQUAL_size_t(size_before, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_peek_back_should_return_struct(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(test_record_t), dds_alloc_stdlib());

    const test_record_t rec = make_record(11, "peek_back", 2.23);
    dds_ring_buffer_push_back(&rb, &rec);

    test_record_t out;
    dds_ring_buffer_peek_back(&rb, &out);
    TEST_ASSERT_TRUE(records_equal(&rec, &out));
    TEST_ASSERT_EQUAL_size_t(1, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_peek_back_should_return_correct_element_after_tail_wraps(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    /* push 5, pop 3, push 4 — tail wraps from 7 to 0 then to 1 */
    const int first[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) dds_ring_buffer_push_back(&rb, &first[i]);
    for (int i = 0; i < 3; i++) dds_ring_buffer_pop_front(&rb, NULL);
    const int second[] = {60, 70, 80, 90};
    for (int i = 0; i < 4; i++) dds_ring_buffer_push_back(&rb, &second[i]);

    /* tail=1 (wrapped); prev(1)=0 → data[0]=90 */
    int out;
    dds_ring_buffer_peek_back(&rb, &out);
    TEST_ASSERT_EQUAL_INT(90, out);
    TEST_ASSERT_EQUAL_size_t(6, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

/* dds_ring_buffer_peek_front */

void dds_ring_buffer_peek_front_should_return_ok(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);

    int out;
    const dds_result_t result = dds_ring_buffer_peek_front(&rb, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_peek_front_should_return_invalid_parameter_when_ring_buffer_null(void) {
    int out;
    const dds_result_t result = dds_ring_buffer_peek_front(NULL, &out);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_ring_buffer_peek_front_should_return_invalid_parameter_when_element_null(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);

    const dds_result_t result = dds_ring_buffer_peek_front(&rb, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_peek_front_should_return_out_of_range_when_empty(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    int out;
    const dds_result_t result = dds_ring_buffer_peek_front(&rb, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_peek_front_should_return_correct_element(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int a = 10, b = 20, c = 30;
    dds_ring_buffer_push_back(&rb, &a);
    dds_ring_buffer_push_back(&rb, &b);
    dds_ring_buffer_push_back(&rb, &c);

    int out;
    dds_ring_buffer_peek_front(&rb, &out);
    TEST_ASSERT_EQUAL_INT(10, out);
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_peek_front_should_not_modify_ring_buffer(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 42;
    dds_ring_buffer_push_back(&rb, &value);

    const size_t size_before = dds_ring_buffer_get_size(&rb);
    int out;
    dds_ring_buffer_peek_front(&rb, &out);

    TEST_ASSERT_EQUAL_size_t(size_before, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_peek_front_should_return_struct(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(test_record_t), dds_alloc_stdlib());

    const test_record_t rec = make_record(13, "peek_front", 1.61);
    dds_ring_buffer_push_back(&rb, &rec);

    test_record_t out;
    dds_ring_buffer_peek_front(&rb, &out);
    TEST_ASSERT_TRUE(records_equal(&rec, &out));
    TEST_ASSERT_EQUAL_size_t(1, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_peek_front_should_return_correct_element_after_head_wraps(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    /* push_back one to trigger allocation; then push_front 3× to wrap head backward */
    const int back = 100;
    dds_ring_buffer_push_back(&rb, &back); /* head=0, tail=1, cap=8 */

    const int a=10, b=20, c=30;
    dds_ring_buffer_push_front(&rb, &a); /* head=7 */
    dds_ring_buffer_push_front(&rb, &b); /* head=6 */
    dds_ring_buffer_push_front(&rb, &c); /* head=5 */

    /* front is at data[5]=30 */
    int out;
    dds_ring_buffer_peek_front(&rb, &out);
    TEST_ASSERT_EQUAL_INT(30, out);
    TEST_ASSERT_EQUAL_size_t(4, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

/* dds_ring_buffer_get_size */

void dds_ring_buffer_get_size_should_return_zero_when_ring_buffer_null(void) {
    TEST_ASSERT_EQUAL_size_t(0, dds_ring_buffer_get_size(NULL));
}

void dds_ring_buffer_get_size_should_return_zero_on_empty_ring_buffer(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_EQUAL_size_t(0, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_get_size_should_return_correct_size(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) dds_ring_buffer_push_back(&rb, &values[i]);

    TEST_ASSERT_EQUAL_size_t(5, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_get_size_should_equal_capacity_when_full(void) {
    /* When all slots are filled, tail wraps back to equal head.
     * get_size must return capacity, not 0 (head==tail ambiguity). */
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    for (int i = 0; i < (int)DDS_RING_BUFFER_INITIAL_SIZE; i++) {
        dds_ring_buffer_push_back(&rb, &i);
    }

    TEST_ASSERT_EQUAL_size_t(DDS_RING_BUFFER_INITIAL_SIZE, dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

/* dds_ring_buffer_get_capacity */

void dds_ring_buffer_get_capacity_should_return_zero_when_ring_buffer_null(void) {
    TEST_ASSERT_EQUAL_size_t(0, dds_ring_buffer_get_capacity(NULL));
}

void dds_ring_buffer_get_capacity_should_return_zero_on_empty_ring_buffer(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_EQUAL_size_t(0, dds_ring_buffer_get_capacity(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_get_capacity_should_return_at_least_size(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_ring_buffer_push_back(&rb, &values[i]);

    TEST_ASSERT(dds_ring_buffer_get_capacity(&rb) >= dds_ring_buffer_get_size(&rb));
    dds_ring_buffer_free(&rb);
}

/* dds_ring_buffer_get_data */

void dds_ring_buffer_get_data_should_return_null_when_ring_buffer_null(void) {
    TEST_ASSERT_NULL(dds_ring_buffer_get_data(NULL));
}

void dds_ring_buffer_get_data_should_return_null_on_empty_ring_buffer(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_NULL(dds_ring_buffer_get_data(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_get_data_should_return_non_null_after_push(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);

    TEST_ASSERT_NOT_NULL(dds_ring_buffer_get_data(&rb));
    dds_ring_buffer_free(&rb);
}

/* dds_ring_buffer_is_empty */

void dds_ring_buffer_is_empty_should_return_true_when_ring_buffer_null(void) {
    TEST_ASSERT_TRUE(dds_ring_buffer_is_empty(NULL));
}

void dds_ring_buffer_is_empty_should_return_true_on_empty_ring_buffer(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_TRUE(dds_ring_buffer_is_empty(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_is_empty_should_return_false_after_push(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);

    TEST_ASSERT_FALSE(dds_ring_buffer_is_empty(&rb));
    dds_ring_buffer_free(&rb);
}

void dds_ring_buffer_is_empty_should_return_true_after_all_elements_popped(void) {
    dds_ring_buffer_t rb;
    dds_ring_buffer_init(&rb, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_ring_buffer_push_back(&rb, &value);
    dds_ring_buffer_pop_front(&rb, NULL);

    TEST_ASSERT_TRUE(dds_ring_buffer_is_empty(&rb));
    dds_ring_buffer_free(&rb);
}
