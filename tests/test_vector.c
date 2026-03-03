#include <stdint.h>
#include "dds/vector.h"
#include "unity.h"

void dds_vector_init_should_return_ok(void) {
    dds_vector_t vector;
    const dds_result_t result = dds_vector_init(&vector, 4, dds_alloc_stdlib());

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
}

void dds_vector_init_should_return_error_when_invalid_alloc(void) {
    dds_vector_t vector;

    dds_alloc_t alloc;
    alloc.malloc = NULL;
    alloc.realloc = NULL;
    alloc.free = NULL;
    alloc.context = NULL;

    const dds_result_t result = dds_vector_init(&vector, 4, alloc);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_vector_free_should_zero_alloc(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, 4, dds_alloc_stdlib());
    dds_vector_free(&vector);


    TEST_ASSERT_NULL(vector.alloc.malloc);
    TEST_ASSERT_NULL(vector.alloc.realloc);
    TEST_ASSERT_NULL(vector.alloc.free);
}

void dds_vector_init_should_create_correct_structure(void) {
    const size_t element_size = 4;
    dds_vector_t vector;
    const dds_result_t result = dds_vector_init(&vector, element_size, dds_alloc_stdlib());
    (void) result;

    TEST_ASSERT_NULL(vector.data);
    TEST_ASSERT_EQUAL_size_t(0, vector.size);
    TEST_ASSERT_EQUAL_size_t(0, vector.capacity);
    TEST_ASSERT_EQUAL_size_t(element_size, vector.element_size);
}

/* dds_vector_clear */

void dds_vector_clear_should_return_ok(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const dds_result_t result = dds_vector_clear(&vector);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_vector_free(&vector);
}

void dds_vector_clear_should_reset_size_to_zero(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    dds_vector_clear(&vector);

    TEST_ASSERT_EQUAL_size_t(0, dds_vector_get_size(&vector));
    dds_vector_free(&vector);
}

void dds_vector_clear_should_preserve_capacity(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    const size_t capacity_before = dds_vector_get_capacity(&vector);
    dds_vector_clear(&vector);

    TEST_ASSERT_EQUAL_size_t(capacity_before, dds_vector_get_capacity(&vector));
    dds_vector_free(&vector);
}

void dds_vector_clear_should_allow_push_after_clear(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);
    dds_vector_clear(&vector);

    const int new_value = 99;
    dds_vector_push_back(&vector, &new_value);

    int out;
    dds_vector_get(&vector, 0, &out);
    TEST_ASSERT_EQUAL_INT(99, out);
    TEST_ASSERT_EQUAL_size_t(1, dds_vector_get_size(&vector));
    dds_vector_free(&vector);
}

void dds_vector_clear_should_return_invalid_parameter_when_vector_null(void) {
    const dds_result_t result = dds_vector_clear(NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

/* dds_vector_shrink_to_fit */

void dds_vector_shrink_to_fit_should_return_ok(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    dds_vector_reserve(&vector, 10);
    const int value = 1;
    dds_vector_push_back(&vector, &value);

    const dds_result_t result = dds_vector_shrink_to_fit(&vector);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_vector_free(&vector);
}

void dds_vector_shrink_to_fit_should_set_capacity_to_size(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    dds_vector_reserve(&vector, 10);
    const int value = 1;
    dds_vector_push_back(&vector, &value);

    dds_vector_shrink_to_fit(&vector);

    TEST_ASSERT_EQUAL_size_t(1, dds_vector_get_capacity(&vector));
    dds_vector_free(&vector);
}

void dds_vector_shrink_to_fit_should_preserve_elements(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);
    dds_vector_reserve(&vector, 10);

    dds_vector_shrink_to_fit(&vector);

    int out;
    dds_vector_get(&vector, 0, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_vector_get(&vector, 1, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_vector_get(&vector, 2, &out); TEST_ASSERT_EQUAL_INT(30, out);
    dds_vector_free(&vector);
}

void dds_vector_shrink_to_fit_should_not_change_size(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    dds_vector_reserve(&vector, 10);
    const int value = 1;
    dds_vector_push_back(&vector, &value);

    dds_vector_shrink_to_fit(&vector);

    TEST_ASSERT_EQUAL_size_t(1, dds_vector_get_size(&vector));
    dds_vector_free(&vector);
}

void dds_vector_shrink_to_fit_should_free_buffer_when_empty(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    dds_vector_reserve(&vector, 10);
    dds_vector_shrink_to_fit(&vector);

    TEST_ASSERT_NULL(dds_vector_get_data(&vector));
    TEST_ASSERT_EQUAL_size_t(0, dds_vector_get_capacity(&vector));
    dds_vector_free(&vector);
}

void dds_vector_shrink_to_fit_should_be_noop_when_already_fitted(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    /* after a single push capacity == size == 1 */
    const dds_result_t result = dds_vector_shrink_to_fit(&vector);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    TEST_ASSERT_EQUAL_size_t(1, dds_vector_get_capacity(&vector));
    dds_vector_free(&vector);
}

void dds_vector_shrink_to_fit_should_return_invalid_parameter_when_vector_null(void) {
    const dds_result_t result = dds_vector_shrink_to_fit(NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_vector_shrink_to_fit_should_return_overflow_on_buffer_size_overflow(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    /* make size > capacity so the no-op branch is skipped, then overflow */
    vector.element_size = SIZE_MAX / 2 + 1;
    vector.size = 2;
    vector.capacity = 4;

    const dds_result_t result = dds_vector_shrink_to_fit(&vector);

    TEST_ASSERT_EQUAL_INT(DDS_OVERFLOW, result);
    vector.element_size = sizeof(int);
    vector.size = 0;
    vector.capacity = 0;
    dds_vector_free(&vector);
}

/* dds_vector_reserve */

void dds_vector_reserve_should_return_ok(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const dds_result_t result = dds_vector_reserve(&vector, 10);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_vector_free(&vector);
}

void dds_vector_reserve_should_set_capacity(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    dds_vector_reserve(&vector, 10);

    TEST_ASSERT_EQUAL_size_t(10, dds_vector_get_capacity(&vector));
    dds_vector_free(&vector);
}

void dds_vector_reserve_should_not_change_size(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);
    dds_vector_reserve(&vector, 10);

    TEST_ASSERT_EQUAL_size_t(1, dds_vector_get_size(&vector));
    dds_vector_free(&vector);
}

void dds_vector_reserve_should_be_noop_when_capacity_already_sufficient(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    dds_vector_reserve(&vector, 10);
    const dds_result_t result = dds_vector_reserve(&vector, 5);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    TEST_ASSERT_EQUAL_size_t(10, dds_vector_get_capacity(&vector));
    dds_vector_free(&vector);
}

void dds_vector_reserve_should_return_invalid_parameter_when_vector_null(void) {
    const dds_result_t result = dds_vector_reserve(NULL, 10);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_vector_reserve_should_return_invalid_parameter_when_capacity_zero(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const dds_result_t result = dds_vector_reserve(&vector, 0);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
    dds_vector_free(&vector);
}

void dds_vector_reserve_should_return_overflow_on_buffer_size_overflow(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    vector.element_size = SIZE_MAX / 2 + 1;
    const dds_result_t result = dds_vector_reserve(&vector, 2);

    TEST_ASSERT_EQUAL_INT(DDS_OVERFLOW, result);
    vector.element_size = sizeof(int);
    dds_vector_free(&vector);
}

/* dds_vector_push_back */

void dds_vector_push_back_should_return_ok(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 42;
    const dds_result_t result = dds_vector_push_back(&vector, &value);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_vector_free(&vector);
}

void dds_vector_push_back_should_increase_size(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int a = 1, b = 2, c = 3;
    dds_vector_push_back(&vector, &a);
    dds_vector_push_back(&vector, &b);
    dds_vector_push_back(&vector, &c);

    TEST_ASSERT_EQUAL_size_t(3, vector.size);
    dds_vector_free(&vector);
}

void dds_vector_push_back_should_return_invalid_parameter_when_vector_null(void) {
    const int value = 1;
    const dds_result_t result = dds_vector_push_back(NULL, &value);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_vector_push_back_should_return_invalid_parameter_when_element_null(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const dds_result_t result = dds_vector_push_back(&vector, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
    dds_vector_free(&vector);
}

void dds_vector_push_back_should_store_correct_values(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    int out;
    dds_vector_get(&vector, 0, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_vector_get(&vector, 1, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_vector_get(&vector, 2, &out); TEST_ASSERT_EQUAL_INT(30, out);

    dds_vector_free(&vector);
}

void dds_vector_push_back_should_grow_capacity(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    /* push INITIAL_SIZE + 1 elements to trigger a resize */
    for (int i = 0; i <= INITIAL_SIZE; i++) dds_vector_push_back(&vector, &i);

    TEST_ASSERT_TRUE(vector.capacity > INITIAL_SIZE);
    dds_vector_free(&vector);
}

void dds_vector_push_back_should_return_overflow_when_capacity_overflows(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    /* set capacity just above SIZE_MAX / GROWTH_FACTOR so capacity * GROWTH_FACTOR wraps */
    vector.capacity = SIZE_MAX / GROWTH_FACTOR + 1;
    vector.size = vector.capacity;

    const int value = 1;
    const dds_result_t result = dds_vector_push_back(&vector, &value);

    TEST_ASSERT_EQUAL_INT(DDS_OVERFLOW, result);
    vector.capacity = 0;
    vector.size = 0;
    dds_vector_free(&vector);
}

void dds_vector_push_back_should_return_overflow_when_buffer_size_overflows(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    /* with capacity=1 and element_size=SIZE_MAX/2+1, new_capacity=2 and
       2 * element_size overflows */
    vector.element_size = SIZE_MAX / 2 + 1;
    vector.capacity = 1;
    vector.size = 1;

    const int value = 1;
    const dds_result_t result = dds_vector_push_back(&vector, &value);

    TEST_ASSERT_EQUAL_INT(DDS_OVERFLOW, result);
    vector.capacity = 0;
    vector.size = 0;
    vector.element_size = sizeof(int);
    dds_vector_free(&vector);
}

/* dds_vector_insert */

void dds_vector_insert_should_return_ok(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    const int insert = 99;
    const dds_result_t result = dds_vector_insert(&vector, 0, &insert);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_vector_free(&vector);
}

void dds_vector_insert_should_increase_size(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int a = 1, b = 2;
    dds_vector_push_back(&vector, &a);

    dds_vector_insert(&vector, 0, &b);

    TEST_ASSERT_EQUAL_size_t(2, dds_vector_get_size(&vector));
    dds_vector_free(&vector);
}

void dds_vector_insert_should_place_element_at_index(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    const int insert = 99;
    dds_vector_insert(&vector, 1, &insert);

    int out;
    dds_vector_get(&vector, 0, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_vector_get(&vector, 1, &out); TEST_ASSERT_EQUAL_INT(99, out);
    dds_vector_get(&vector, 2, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_vector_get(&vector, 3, &out); TEST_ASSERT_EQUAL_INT(30, out);
    dds_vector_free(&vector);
}

void dds_vector_insert_should_shift_elements_right(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    const int insert = 99;
    dds_vector_insert(&vector, 0, &insert);

    int out;
    dds_vector_get(&vector, 0, &out); TEST_ASSERT_EQUAL_INT(99, out);
    dds_vector_get(&vector, 1, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_vector_get(&vector, 2, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_vector_get(&vector, 3, &out); TEST_ASSERT_EQUAL_INT(30, out);
    dds_vector_free(&vector);
}

void dds_vector_insert_at_end_should_append(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20};
    for (int i = 0; i < 2; i++) dds_vector_push_back(&vector, &values[i]);

    const int insert = 99;
    dds_vector_insert(&vector, 2, &insert);

    int out;
    dds_vector_get(&vector, 2, &out);
    TEST_ASSERT_EQUAL_INT(99, out);
    TEST_ASSERT_EQUAL_size_t(3, dds_vector_get_size(&vector));
    dds_vector_free(&vector);
}

void dds_vector_insert_should_return_invalid_parameter_when_vector_null(void) {
    const int value = 1;
    const dds_result_t result = dds_vector_insert(NULL, 0, &value);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_vector_insert_should_return_invalid_parameter_when_element_null(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const dds_result_t result = dds_vector_insert(&vector, 0, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
    dds_vector_free(&vector);
}

void dds_vector_insert_should_return_out_of_range_when_index_exceeds_size(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    const int insert = 99;
    const dds_result_t result = dds_vector_insert(&vector, 2, &insert);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_vector_free(&vector);
}

/* dds_vector_remove */

void dds_vector_remove_should_return_ok(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    int out;
    const dds_result_t result = dds_vector_remove(&vector, 0, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_vector_free(&vector);
}

void dds_vector_remove_should_return_correct_element(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    int out;
    dds_vector_remove(&vector, 1, &out);

    TEST_ASSERT_EQUAL_INT(20, out);
    dds_vector_free(&vector);
}

void dds_vector_remove_should_decrease_size(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    dds_vector_remove(&vector, 0, NULL);

    TEST_ASSERT_EQUAL_size_t(2, dds_vector_get_size(&vector));
    dds_vector_free(&vector);
}

void dds_vector_remove_should_shift_elements_left(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    dds_vector_remove(&vector, 1, NULL);

    int out;
    dds_vector_get(&vector, 0, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_vector_get(&vector, 1, &out); TEST_ASSERT_EQUAL_INT(30, out);
    dds_vector_free(&vector);
}

void dds_vector_remove_should_not_change_capacity(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    const size_t capacity_before = dds_vector_get_capacity(&vector);
    dds_vector_remove(&vector, 0, NULL);

    TEST_ASSERT_EQUAL_size_t(capacity_before, dds_vector_get_capacity(&vector));
    dds_vector_free(&vector);
}

void dds_vector_remove_should_discard_when_element_null(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    const dds_result_t result = dds_vector_remove(&vector, 0, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    TEST_ASSERT_EQUAL_size_t(0, dds_vector_get_size(&vector));
    dds_vector_free(&vector);
}

void dds_vector_remove_should_return_invalid_parameter_when_vector_null(void) {
    int out;
    const dds_result_t result = dds_vector_remove(NULL, 0, &out);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_vector_remove_should_return_out_of_range_when_index_exceeds_size(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    int out;
    const dds_result_t result = dds_vector_remove(&vector, 1, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_vector_free(&vector);
}

void dds_vector_remove_should_return_out_of_range_on_empty_vector(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    int out;
    const dds_result_t result = dds_vector_remove(&vector, 0, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_vector_free(&vector);
}

/* dds_vector_pop_back */

void dds_vector_pop_back_should_return_ok(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    int out;
    const dds_result_t result = dds_vector_pop_back(&vector, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_vector_free(&vector);
}

void dds_vector_pop_back_should_return_correct_element(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int a = 10, b = 20, c = 30;
    dds_vector_push_back(&vector, &a);
    dds_vector_push_back(&vector, &b);
    dds_vector_push_back(&vector, &c);

    int out;
    dds_vector_pop_back(&vector, &out);

    TEST_ASSERT_EQUAL_INT(30, out);
    dds_vector_free(&vector);
}

void dds_vector_pop_back_should_decrease_size(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int a = 1, b = 2;
    dds_vector_push_back(&vector, &a);
    dds_vector_push_back(&vector, &b);

    int out;
    dds_vector_pop_back(&vector, &out);

    TEST_ASSERT_EQUAL_size_t(1, dds_vector_get_size(&vector));
    dds_vector_free(&vector);
}

void dds_vector_pop_back_should_not_change_capacity(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    const size_t capacity_before = dds_vector_get_capacity(&vector);
    int out;
    dds_vector_pop_back(&vector, &out);

    TEST_ASSERT_EQUAL_size_t(capacity_before, dds_vector_get_capacity(&vector));
    dds_vector_free(&vector);
}

void dds_vector_pop_back_should_return_elements_in_lifo_order(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    int out;
    dds_vector_pop_back(&vector, &out); TEST_ASSERT_EQUAL_INT(30, out);
    dds_vector_pop_back(&vector, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_vector_pop_back(&vector, &out); TEST_ASSERT_EQUAL_INT(10, out);

    dds_vector_free(&vector);
}

void dds_vector_pop_back_should_discard_when_element_null(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    const dds_result_t result = dds_vector_pop_back(&vector, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    TEST_ASSERT_EQUAL_size_t(0, dds_vector_get_size(&vector));
    dds_vector_free(&vector);
}

void dds_vector_pop_back_should_return_invalid_parameter_when_vector_null(void) {
    int out;
    const dds_result_t result = dds_vector_pop_back(NULL, &out);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_vector_pop_back_should_return_out_of_range_when_empty(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    int out;
    const dds_result_t result = dds_vector_pop_back(&vector, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_vector_free(&vector);
}

/* dds_vector_get */

void dds_vector_get_should_return_ok(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 7;
    dds_vector_push_back(&vector, &value);

    int out;
    const dds_result_t result = dds_vector_get(&vector, 0, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    TEST_ASSERT_EQUAL_INT(7, out);
    dds_vector_free(&vector);
}

void dds_vector_get_should_return_invalid_parameter_when_vector_null(void) {
    int out;
    const dds_result_t result = dds_vector_get(NULL, 0, &out);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_vector_get_should_return_invalid_parameter_when_element_null(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    const dds_result_t result = dds_vector_get(&vector, 0, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
    dds_vector_free(&vector);
}

void dds_vector_get_should_return_out_of_range_when_index_exceeds_size(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    int out;
    const dds_result_t result = dds_vector_get(&vector, 1, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_vector_free(&vector);
}

void dds_vector_get_should_return_out_of_range_on_empty_vector(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    int out;
    const dds_result_t result = dds_vector_get(&vector, 0, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_vector_free(&vector);
}

/* dds_vector_get_size */

void dds_vector_get_size_should_return_zero_when_vector_null(void) {
    TEST_ASSERT_EQUAL_size_t(0, dds_vector_get_size(NULL));
}

void dds_vector_get_size_should_return_zero_on_empty_vector(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_EQUAL_size_t(0, dds_vector_get_size(&vector));

    dds_vector_free(&vector);
}

void dds_vector_get_size_should_return_correct_size(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    TEST_ASSERT_EQUAL_size_t(3, dds_vector_get_size(&vector));

    dds_vector_free(&vector);
}

/* dds_vector_get_capacity */

void dds_vector_get_capacity_should_return_zero_when_vector_null(void) {
    TEST_ASSERT_EQUAL_size_t(0, dds_vector_get_capacity(NULL));
}

void dds_vector_get_capacity_should_return_zero_on_empty_vector(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_EQUAL_size_t(0, dds_vector_get_capacity(&vector));

    dds_vector_free(&vector);
}

void dds_vector_get_capacity_should_return_at_least_size(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    TEST_ASSERT_TRUE(dds_vector_get_capacity(&vector) >= dds_vector_get_size(&vector));

    dds_vector_free(&vector);
}

/* dds_vector_get_data */

void dds_vector_get_data_should_return_null_when_vector_null(void) {
    TEST_ASSERT_NULL(dds_vector_get_data(NULL));
}

void dds_vector_get_data_should_return_null_on_empty_vector(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_NULL(dds_vector_get_data(&vector));

    dds_vector_free(&vector);
}

void dds_vector_get_data_should_return_non_null_after_push(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 42;
    dds_vector_push_back(&vector, &value);

    TEST_ASSERT_NOT_NULL(dds_vector_get_data(&vector));

    dds_vector_free(&vector);
}

void dds_vector_get_data_should_point_to_correct_values(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    const int* data = (const int*)dds_vector_get_data(&vector);
    TEST_ASSERT_EQUAL_INT(10, data[0]);
    TEST_ASSERT_EQUAL_INT(20, data[1]);
    TEST_ASSERT_EQUAL_INT(30, data[2]);

    dds_vector_free(&vector);
}

/* dds_vector_is_empty */

void dds_vector_is_empty_should_return_true_when_vector_null(void) {
    TEST_ASSERT_TRUE(dds_vector_is_empty(NULL));
}

void dds_vector_is_empty_should_return_true_on_empty_vector(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_TRUE(dds_vector_is_empty(&vector));

    dds_vector_free(&vector);
}

void dds_vector_is_empty_should_return_false_after_push(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    TEST_ASSERT_FALSE(dds_vector_is_empty(&vector));

    dds_vector_free(&vector);
}

void dds_vector_is_empty_should_return_true_after_all_elements_popped(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    int out;
    dds_vector_pop_back(&vector, &out);

    TEST_ASSERT_TRUE(dds_vector_is_empty(&vector));

    dds_vector_free(&vector);
}

/* dds_vector_at */

void dds_vector_at_should_return_null_when_vector_null(void) {
    TEST_ASSERT_NULL(dds_vector_at(NULL, 0));
}

void dds_vector_at_should_return_null_on_empty_vector(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_NULL(dds_vector_at(&vector, 0));

    dds_vector_free(&vector);
}

void dds_vector_at_should_return_null_when_index_out_of_bounds(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    TEST_ASSERT_NULL(dds_vector_at(&vector, 1));

    dds_vector_free(&vector);
}

void dds_vector_at_should_return_correct_value(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    TEST_ASSERT_EQUAL_INT(10, *(int*)dds_vector_at(&vector, 0));
    TEST_ASSERT_EQUAL_INT(20, *(int*)dds_vector_at(&vector, 1));
    TEST_ASSERT_EQUAL_INT(30, *(int*)dds_vector_at(&vector, 2));

    dds_vector_free(&vector);
}

void dds_vector_at_should_allow_mutation(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    *(int*)dds_vector_at(&vector, 0) = 99;

    int out;
    dds_vector_get(&vector, 0, &out);
    TEST_ASSERT_EQUAL_INT(99, out);

    dds_vector_free(&vector);
}

/* dds_vector_index */

void dds_vector_index_should_return_correct_value(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_vector_push_back(&vector, &values[i]);

    TEST_ASSERT_EQUAL_INT(10, dds_vector_index(&vector, int, 0));
    TEST_ASSERT_EQUAL_INT(20, dds_vector_index(&vector, int, 1));
    TEST_ASSERT_EQUAL_INT(30, dds_vector_index(&vector, int, 2));

    dds_vector_free(&vector);
}

void dds_vector_index_should_allow_mutation(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_vector_push_back(&vector, &value);

    dds_vector_index(&vector, int, 0) = 99;

    int out;
    dds_vector_get(&vector, 0, &out);
    TEST_ASSERT_EQUAL_INT(99, out);

    dds_vector_free(&vector);
}

void dds_vector_index_local_copy_should_not_affect_vector(void) {
    dds_vector_t vector;
    dds_vector_init(&vector, sizeof(int), dds_alloc_stdlib());

    const int value = 42;
    dds_vector_push_back(&vector, &value);

    int local = dds_vector_index(&vector, int, 0);
    local = 99;
    (void)local;

    int out;
    dds_vector_get(&vector, 0, &out);
    TEST_ASSERT_EQUAL_INT(42, out);

    dds_vector_free(&vector);
}
