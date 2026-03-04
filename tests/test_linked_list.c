#include <stdint.h>
#include <stdlib.h>
#include "dds/linked_list.h"
#include "unity.h"
#include "test_structs.h"

static void* counting_malloc(void* ctx, size_t size) {
    (*(int*)ctx)++;
    return malloc(size);
}

static void counting_free(void* ctx, void* ptr) {
    if (ptr) (*(int*)ctx)--;
    free(ptr);
}

static dds_alloc_t counting_alloc(int* counter) {
    dds_alloc_t alloc;
    alloc.context = counter;
    alloc.malloc = counting_malloc;
    alloc.realloc = NULL;
    alloc.free = counting_free;
    return alloc;
}

/* dds_linked_list_init */

void dds_linked_list_init_should_return_ok(void) {
    dds_linked_list_t list;
    const dds_result_t result = dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_init_should_return_invalid_parameter_when_list_null(void) {
    const dds_result_t result = dds_linked_list_init(NULL, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_linked_list_init_should_return_invalid_parameter_when_element_size_zero(void) {
    dds_linked_list_t list;
    const dds_result_t result = dds_linked_list_init(&list, 0, dds_alloc_stdlib());

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_linked_list_init_should_return_invalid_parameter_when_alloc_invalid(void) {
    dds_linked_list_t list;
    dds_alloc_t alloc = { .malloc = NULL, .realloc = NULL, .free = NULL, .context = NULL };
    const dds_result_t result = dds_linked_list_init(&list, sizeof(int), alloc);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_linked_list_init_should_create_correct_structure(void) {
    const size_t element_size = sizeof(int);
    dds_linked_list_t list;
    dds_linked_list_init(&list, element_size, dds_alloc_stdlib());

    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_NULL(list.tail);
    TEST_ASSERT_EQUAL_size_t(0, list.size);
    TEST_ASSERT_EQUAL_size_t(element_size, list.element_size);

    dds_linked_list_free(&list);
}

/* dds_linked_list_free */

void dds_linked_list_free_should_not_crash_when_null(void) {
    dds_linked_list_free(NULL);
}

void dds_linked_list_free_should_zero_fields(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());
    dds_linked_list_free(&list);

    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_NULL(list.tail);
    TEST_ASSERT_EQUAL_size_t(0, list.size);
    TEST_ASSERT_EQUAL_size_t(0, list.element_size);
    TEST_ASSERT_NULL(list.alloc.malloc);
    TEST_ASSERT_NULL(list.alloc.realloc);
    TEST_ASSERT_NULL(list.alloc.free);
}

void dds_linked_list_free_should_free_all_nodes(void) {
    int count = 0;
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), counting_alloc(&count));

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    TEST_ASSERT_EQUAL_INT(3, count);
    dds_linked_list_free(&list);
    TEST_ASSERT_EQUAL_INT(0, count);
}

/* dds_linked_list_clear */

void dds_linked_list_clear_should_return_ok(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const dds_result_t result = dds_linked_list_clear(&list);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_clear_should_return_invalid_parameter_when_list_null(void) {
    const dds_result_t result = dds_linked_list_clear(NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_linked_list_clear_should_reset_size_to_zero(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    dds_linked_list_clear(&list);

    TEST_ASSERT_EQUAL_size_t(0, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_clear_should_null_head_and_tail(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    dds_linked_list_clear(&list);

    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_NULL(list.tail);
    dds_linked_list_free(&list);
}

void dds_linked_list_clear_should_preserve_element_size_and_alloc(void) {
    const size_t element_size = sizeof(int);
    dds_linked_list_t list;
    dds_linked_list_init(&list, element_size, dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    dds_linked_list_clear(&list);

    TEST_ASSERT_EQUAL_size_t(element_size, list.element_size);
    TEST_ASSERT_NOT_NULL(list.alloc.malloc);
    TEST_ASSERT_NOT_NULL(list.alloc.free);
    dds_linked_list_free(&list);
}

void dds_linked_list_clear_should_free_all_nodes(void) {
    int count = 0;
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), counting_alloc(&count));

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    TEST_ASSERT_EQUAL_INT(3, count);
    dds_linked_list_clear(&list);
    TEST_ASSERT_EQUAL_INT(0, count);

    dds_linked_list_free(&list);
}

void dds_linked_list_clear_should_allow_reuse(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    dds_linked_list_clear(&list);

    const int new_value = 99;
    dds_linked_list_push_back(&list, &new_value);

    int out;
    dds_linked_list_get(&list, 0, &out);
    TEST_ASSERT_EQUAL_INT(99, out);
    TEST_ASSERT_EQUAL_size_t(1, list.size);

    dds_linked_list_free(&list);
}

/* dds_linked_list_push_back */

void dds_linked_list_push_back_should_return_ok(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    const dds_result_t result = dds_linked_list_push_back(&list, &value);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_push_back_should_return_invalid_parameter_when_list_null(void) {
    const int value = 1;
    const dds_result_t result = dds_linked_list_push_back(NULL, &value);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_linked_list_push_back_should_return_invalid_parameter_when_element_null(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const dds_result_t result = dds_linked_list_push_back(&list, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_push_back_should_increase_size(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int a = 1, b = 2, c = 3;
    dds_linked_list_push_back(&list, &a);
    dds_linked_list_push_back(&list, &b);
    dds_linked_list_push_back(&list, &c);

    TEST_ASSERT_EQUAL_size_t(3, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_push_back_should_store_correct_values(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    int out;
    dds_linked_list_get(&list, 0, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_linked_list_get(&list, 1, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_linked_list_get(&list, 2, &out); TEST_ASSERT_EQUAL_INT(30, out);

    dds_linked_list_free(&list);
}

void dds_linked_list_push_back_should_store_struct(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(test_record_t), dds_alloc_stdlib());

    test_record_t r = make_record(1, "Alice", 3.14);
    dds_linked_list_push_back(&list, &r);

    test_record_t out;
    dds_linked_list_get(&list, 0, &out);
    TEST_ASSERT_TRUE(records_equal(&r, &out));

    dds_linked_list_free(&list);
}

/* dds_linked_list_push_front */

void dds_linked_list_push_front_should_return_ok(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    const dds_result_t result = dds_linked_list_push_front(&list, &value);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_push_front_should_return_invalid_parameter_when_list_null(void) {
    const int value = 1;
    const dds_result_t result = dds_linked_list_push_front(NULL, &value);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_linked_list_push_front_should_return_invalid_parameter_when_element_null(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const dds_result_t result = dds_linked_list_push_front(&list, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_push_front_should_increase_size(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int a = 1, b = 2, c = 3;
    dds_linked_list_push_front(&list, &a);
    dds_linked_list_push_front(&list, &b);
    dds_linked_list_push_front(&list, &c);

    TEST_ASSERT_EQUAL_size_t(3, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_push_front_should_store_correct_values(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_linked_list_push_front(&list, &values[i]);

    int out;
    dds_linked_list_get(&list, 0, &out); TEST_ASSERT_EQUAL_INT(30, out);
    dds_linked_list_get(&list, 1, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_linked_list_get(&list, 2, &out); TEST_ASSERT_EQUAL_INT(10, out);

    dds_linked_list_free(&list);
}

void dds_linked_list_push_front_should_store_struct(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(test_record_t), dds_alloc_stdlib());

    test_record_t r0 = make_record(0, "Zero", 0.0);
    test_record_t r1 = make_record(1, "One",  1.0);
    dds_linked_list_push_back(&list, &r0);
    dds_linked_list_push_front(&list, &r1);

    test_record_t out;
    dds_linked_list_get(&list, 0, &out);
    TEST_ASSERT_TRUE(records_equal(&r1, &out));

    dds_linked_list_free(&list);
}

/* dds_linked_list_pop_back */

void dds_linked_list_pop_back_should_return_ok(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    int out;
    const dds_result_t result = dds_linked_list_pop_back(&list, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_pop_back_should_return_invalid_parameter_when_list_null(void) {
    int out;
    const dds_result_t result = dds_linked_list_pop_back(NULL, &out);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_linked_list_pop_back_should_return_out_of_range_when_empty(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    int out;
    const dds_result_t result = dds_linked_list_pop_back(&list, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_pop_back_should_return_correct_element(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    int out;
    dds_linked_list_pop_back(&list, &out);

    TEST_ASSERT_EQUAL_INT(30, out);
    dds_linked_list_free(&list);
}

void dds_linked_list_pop_back_should_decrease_size(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int a = 1, b = 2;
    dds_linked_list_push_back(&list, &a);
    dds_linked_list_push_back(&list, &b);

    int out;
    dds_linked_list_pop_back(&list, &out);

    TEST_ASSERT_EQUAL_size_t(1, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_pop_back_should_discard_when_element_null(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    const dds_result_t result = dds_linked_list_pop_back(&list, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    TEST_ASSERT_EQUAL_size_t(0, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_pop_back_should_return_elements_in_lifo_order(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    int out;
    dds_linked_list_pop_back(&list, &out); TEST_ASSERT_EQUAL_INT(30, out);
    dds_linked_list_pop_back(&list, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_linked_list_pop_back(&list, &out); TEST_ASSERT_EQUAL_INT(10, out);

    dds_linked_list_free(&list);
}

void dds_linked_list_pop_back_should_leave_empty_list_consistent(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);
    dds_linked_list_pop_back(&list, NULL);

    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_NULL(list.tail);
    TEST_ASSERT_EQUAL_size_t(0, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_pop_back_should_return_struct(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(test_record_t), dds_alloc_stdlib());

    test_record_t r = make_record(42, "Bob", 2.71);
    dds_linked_list_push_back(&list, &r);

    test_record_t out;
    dds_linked_list_pop_back(&list, &out);
    TEST_ASSERT_TRUE(records_equal(&r, &out));

    dds_linked_list_free(&list);
}

/* dds_linked_list_pop_front */

void dds_linked_list_pop_front_should_return_ok(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    int out;
    const dds_result_t result = dds_linked_list_pop_front(&list, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_pop_front_should_return_invalid_parameter_when_list_null(void) {
    int out;
    const dds_result_t result = dds_linked_list_pop_front(NULL, &out);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_linked_list_pop_front_should_return_out_of_range_when_empty(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    int out;
    const dds_result_t result = dds_linked_list_pop_front(&list, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_pop_front_should_return_correct_element(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    int out;
    dds_linked_list_pop_front(&list, &out);

    TEST_ASSERT_EQUAL_INT(10, out);
    dds_linked_list_free(&list);
}

void dds_linked_list_pop_front_should_decrease_size(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int a = 1, b = 2;
    dds_linked_list_push_back(&list, &a);
    dds_linked_list_push_back(&list, &b);

    int out;
    dds_linked_list_pop_front(&list, &out);

    TEST_ASSERT_EQUAL_size_t(1, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_pop_front_should_discard_when_element_null(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    const dds_result_t result = dds_linked_list_pop_front(&list, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    TEST_ASSERT_EQUAL_size_t(0, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_pop_front_should_return_elements_in_fifo_order(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    int out;
    dds_linked_list_pop_front(&list, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_linked_list_pop_front(&list, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_linked_list_pop_front(&list, &out); TEST_ASSERT_EQUAL_INT(30, out);

    dds_linked_list_free(&list);
}

void dds_linked_list_pop_front_should_leave_empty_list_consistent(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);
    dds_linked_list_pop_front(&list, NULL);

    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_NULL(list.tail);
    TEST_ASSERT_EQUAL_size_t(0, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_pop_front_should_return_struct(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(test_record_t), dds_alloc_stdlib());

    test_record_t r = make_record(7, "Carol", 1.41);
    dds_linked_list_push_back(&list, &r);

    test_record_t out;
    dds_linked_list_pop_front(&list, &out);
    TEST_ASSERT_TRUE(records_equal(&r, &out));

    dds_linked_list_free(&list);
}

/* dds_linked_list_get */

void dds_linked_list_get_should_return_ok(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 42;
    dds_linked_list_push_back(&list, &value);

    int out;
    const dds_result_t result = dds_linked_list_get(&list, 0, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    TEST_ASSERT_EQUAL_INT(42, out);
    dds_linked_list_free(&list);
}

void dds_linked_list_get_should_return_invalid_parameter_when_list_null(void) {
    int out;
    const dds_result_t result = dds_linked_list_get(NULL, 0, &out);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_linked_list_get_should_return_invalid_parameter_when_element_null(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    const dds_result_t result = dds_linked_list_get(&list, 0, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_get_should_return_out_of_range_when_index_exceeds_size(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    int out;
    const dds_result_t result = dds_linked_list_get(&list, 1, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_get_should_return_out_of_range_when_empty(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    int out;
    const dds_result_t result = dds_linked_list_get(&list, 0, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_get_should_return_correct_values(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    int out;
    dds_linked_list_get(&list, 0, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_linked_list_get(&list, 1, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_linked_list_get(&list, 2, &out); TEST_ASSERT_EQUAL_INT(30, out);

    dds_linked_list_free(&list);
}

void dds_linked_list_get_should_not_modify_list(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 42;
    dds_linked_list_push_back(&list, &value);

    int out;
    dds_linked_list_get(&list, 0, &out);

    TEST_ASSERT_EQUAL_size_t(1, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_get_should_return_struct(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(test_record_t), dds_alloc_stdlib());

    test_record_t r = make_record(3, "Dave", 9.99);
    dds_linked_list_push_back(&list, &r);

    test_record_t out;
    dds_linked_list_get(&list, 0, &out);
    TEST_ASSERT_TRUE(records_equal(&r, &out));

    dds_linked_list_free(&list);
}

/* dds_linked_list_set */

void dds_linked_list_set_should_return_ok(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    const int new_value = 99;
    const dds_result_t result = dds_linked_list_set(&list, 0, &new_value);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_set_should_return_invalid_parameter_when_list_null(void) {
    const int value = 1;
    const dds_result_t result = dds_linked_list_set(NULL, 0, &value);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_linked_list_set_should_return_invalid_parameter_when_element_null(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    const dds_result_t result = dds_linked_list_set(&list, 0, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_set_should_return_out_of_range_when_index_exceeds_size(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    const int new_value = 99;
    const dds_result_t result = dds_linked_list_set(&list, 1, &new_value);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_set_should_update_value(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    const int new_value = 99;
    dds_linked_list_set(&list, 1, &new_value);

    int out;
    dds_linked_list_get(&list, 0, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_linked_list_get(&list, 1, &out); TEST_ASSERT_EQUAL_INT(99, out);
    dds_linked_list_get(&list, 2, &out); TEST_ASSERT_EQUAL_INT(30, out);

    dds_linked_list_free(&list);
}

void dds_linked_list_set_should_not_change_size(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    const int new_value = 99;
    dds_linked_list_set(&list, 0, &new_value);

    TEST_ASSERT_EQUAL_size_t(1, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_set_should_store_struct(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(test_record_t), dds_alloc_stdlib());

    test_record_t r0 = make_record(0, "Old", 0.0);
    test_record_t r1 = make_record(1, "New", 1.0);
    dds_linked_list_push_back(&list, &r0);
    dds_linked_list_set(&list, 0, &r1);

    test_record_t out;
    dds_linked_list_get(&list, 0, &out);
    TEST_ASSERT_TRUE(records_equal(&r1, &out));

    dds_linked_list_free(&list);
}

/* dds_linked_list_insert */

void dds_linked_list_insert_should_return_ok(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int a = 1;
    dds_linked_list_push_back(&list, &a);

    const int value = 99;
    const dds_result_t result = dds_linked_list_insert(&list, 1, &value);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_insert_should_return_invalid_parameter_when_list_null(void) {
    const int value = 1;
    const dds_result_t result = dds_linked_list_insert(NULL, 0, &value);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_linked_list_insert_should_return_invalid_parameter_when_element_null(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const dds_result_t result = dds_linked_list_insert(&list, 0, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_insert_should_return_out_of_range_when_index_exceeds_size(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    const dds_result_t result = dds_linked_list_insert(&list, 2, &value);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_insert_should_increase_size(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int a = 1, b = 2;
    dds_linked_list_push_back(&list, &a);
    dds_linked_list_insert(&list, 1, &b);

    TEST_ASSERT_EQUAL_size_t(2, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_insert_should_place_element_at_index(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    const int insert = 99;
    dds_linked_list_insert(&list, 1, &insert);

    int out;
    dds_linked_list_get(&list, 0, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_linked_list_get(&list, 1, &out); TEST_ASSERT_EQUAL_INT(99, out);
    dds_linked_list_get(&list, 2, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_linked_list_get(&list, 3, &out); TEST_ASSERT_EQUAL_INT(30, out);

    dds_linked_list_free(&list);
}

void dds_linked_list_insert_at_zero_should_prepend(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20};
    for (int i = 0; i < 2; i++) dds_linked_list_push_back(&list, &values[i]);

    const int insert = 99;
    dds_linked_list_insert(&list, 0, &insert);

    int out;
    dds_linked_list_get(&list, 0, &out); TEST_ASSERT_EQUAL_INT(99, out);
    dds_linked_list_get(&list, 1, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_linked_list_get(&list, 2, &out); TEST_ASSERT_EQUAL_INT(20, out);

    dds_linked_list_free(&list);
}

void dds_linked_list_insert_at_size_should_append(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20};
    for (int i = 0; i < 2; i++) dds_linked_list_push_back(&list, &values[i]);

    const int insert = 99;
    dds_linked_list_insert(&list, 2, &insert);

    int out;
    dds_linked_list_get(&list, 2, &out); TEST_ASSERT_EQUAL_INT(99, out);
    TEST_ASSERT_EQUAL_size_t(3, list.size);

    dds_linked_list_free(&list);
}

void dds_linked_list_insert_on_empty_list_should_work(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 42;
    dds_linked_list_insert(&list, 0, &value);

    int out;
    dds_linked_list_get(&list, 0, &out);
    TEST_ASSERT_EQUAL_INT(42, out);
    TEST_ASSERT_EQUAL_size_t(1, list.size);

    dds_linked_list_free(&list);
}

void dds_linked_list_insert_should_store_struct(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(test_record_t), dds_alloc_stdlib());

    test_record_t r0 = make_record(0, "Zero", 0.0);
    test_record_t r1 = make_record(1, "One",  1.0);
    test_record_t r2 = make_record(2, "Two",  2.0);
    dds_linked_list_push_back(&list, &r0);
    dds_linked_list_push_back(&list, &r2);
    dds_linked_list_insert(&list, 1, &r1);

    test_record_t out;
    dds_linked_list_get(&list, 0, &out); TEST_ASSERT_TRUE(records_equal(&r0, &out));
    dds_linked_list_get(&list, 1, &out); TEST_ASSERT_TRUE(records_equal(&r1, &out));
    dds_linked_list_get(&list, 2, &out); TEST_ASSERT_TRUE(records_equal(&r2, &out));

    dds_linked_list_free(&list);
}

/* dds_linked_list_remove */

void dds_linked_list_remove_should_return_ok(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    int out;
    const dds_result_t result = dds_linked_list_remove(&list, 0, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_remove_should_return_invalid_parameter_when_list_null(void) {
    int out;
    const dds_result_t result = dds_linked_list_remove(NULL, 0, &out);

    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, result);
}

void dds_linked_list_remove_should_return_out_of_range_when_index_exceeds_size(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    int out;
    const dds_result_t result = dds_linked_list_remove(&list, 1, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_remove_should_return_out_of_range_when_empty(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    int out;
    const dds_result_t result = dds_linked_list_remove(&list, 0, &out);

    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, result);
    dds_linked_list_free(&list);
}

void dds_linked_list_remove_should_return_correct_element(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    int out;
    dds_linked_list_remove(&list, 1, &out);

    TEST_ASSERT_EQUAL_INT(20, out);
    dds_linked_list_free(&list);
}

void dds_linked_list_remove_should_decrease_size(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    dds_linked_list_remove(&list, 0, NULL);

    TEST_ASSERT_EQUAL_size_t(2, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_remove_should_discard_when_element_null(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    const dds_result_t result = dds_linked_list_remove(&list, 0, NULL);

    TEST_ASSERT_EQUAL_INT(DDS_OK, result);
    TEST_ASSERT_EQUAL_size_t(0, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_remove_should_shift_remaining_elements(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    dds_linked_list_remove(&list, 1, NULL);

    int out;
    dds_linked_list_get(&list, 0, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_linked_list_get(&list, 1, &out); TEST_ASSERT_EQUAL_INT(30, out);

    dds_linked_list_free(&list);
}

void dds_linked_list_remove_should_leave_empty_list_consistent(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);
    dds_linked_list_remove(&list, 0, NULL);

    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_NULL(list.tail);
    TEST_ASSERT_EQUAL_size_t(0, list.size);
    dds_linked_list_free(&list);
}

void dds_linked_list_remove_should_return_struct(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(test_record_t), dds_alloc_stdlib());

    test_record_t r = make_record(42, "Bob", 2.71);
    dds_linked_list_push_back(&list, &r);

    test_record_t out;
    dds_linked_list_remove(&list, 0, &out);
    TEST_ASSERT_TRUE(records_equal(&r, &out));

    dds_linked_list_free(&list);
}

/* dds_linked_list_get_size */

void dds_linked_list_get_size_should_return_zero_when_list_null(void) {
    TEST_ASSERT_EQUAL_size_t(0, dds_linked_list_get_size(NULL));
}

void dds_linked_list_get_size_should_return_zero_on_empty_list(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_EQUAL_size_t(0, dds_linked_list_get_size(&list));

    dds_linked_list_free(&list);
}

void dds_linked_list_get_size_should_return_correct_size(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) dds_linked_list_push_back(&list, &values[i]);

    TEST_ASSERT_EQUAL_size_t(3, dds_linked_list_get_size(&list));

    dds_linked_list_free(&list);
}

/* dds_linked_list_is_empty */

void dds_linked_list_is_empty_should_return_true_when_list_null(void) {
    TEST_ASSERT_TRUE(dds_linked_list_is_empty(NULL));
}

void dds_linked_list_is_empty_should_return_true_on_empty_list(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_TRUE(dds_linked_list_is_empty(&list));

    dds_linked_list_free(&list);
}

void dds_linked_list_is_empty_should_return_false_after_push(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);

    TEST_ASSERT_FALSE(dds_linked_list_is_empty(&list));

    dds_linked_list_free(&list);
}

void dds_linked_list_is_empty_should_return_true_after_all_elements_removed(void) {
    dds_linked_list_t list;
    dds_linked_list_init(&list, sizeof(int), dds_alloc_stdlib());

    const int value = 1;
    dds_linked_list_push_back(&list, &value);
    dds_linked_list_pop_back(&list, NULL);

    TEST_ASSERT_TRUE(dds_linked_list_is_empty(&list));

    dds_linked_list_free(&list);
}
