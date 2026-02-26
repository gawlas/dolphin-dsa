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

    TEST_ASSERT_EQUAL_INT(DDS_ERROR, result);
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
