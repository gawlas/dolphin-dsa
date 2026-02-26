#include <unity.h>

/* dds_vector_init */
void dds_vector_init_should_return_ok(void);
void dds_vector_init_should_return_error_when_invalid_alloc(void);
void dds_vector_init_should_create_correct_structure(void);

/* dds_vector_free */
void dds_vector_free_should_zero_alloc(void);

/* dds_vector_push_back */
void dds_vector_push_back_should_return_ok(void);
void dds_vector_push_back_should_increase_size(void);
void dds_vector_push_back_should_return_invalid_parameter_when_vector_null(void);
void dds_vector_push_back_should_return_invalid_parameter_when_element_null(void);
void dds_vector_push_back_should_store_correct_values(void);
void dds_vector_push_back_should_grow_capacity(void);

/* dds_vector_get */
void dds_vector_get_should_return_ok(void);
void dds_vector_get_should_return_invalid_parameter_when_vector_null(void);
void dds_vector_get_should_return_invalid_parameter_when_element_null(void);

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(dds_vector_init_should_return_ok);
    RUN_TEST(dds_vector_init_should_return_error_when_invalid_alloc);
    RUN_TEST(dds_vector_init_should_create_correct_structure);

    RUN_TEST(dds_vector_free_should_zero_alloc);

    RUN_TEST(dds_vector_push_back_should_return_ok);
    RUN_TEST(dds_vector_push_back_should_increase_size);
    RUN_TEST(dds_vector_push_back_should_return_invalid_parameter_when_vector_null);
    RUN_TEST(dds_vector_push_back_should_return_invalid_parameter_when_element_null);
    RUN_TEST(dds_vector_push_back_should_store_correct_values);
    RUN_TEST(dds_vector_push_back_should_grow_capacity);

    RUN_TEST(dds_vector_get_should_return_ok);
    RUN_TEST(dds_vector_get_should_return_invalid_parameter_when_vector_null);
    RUN_TEST(dds_vector_get_should_return_invalid_parameter_when_element_null);

    return UNITY_END();
}
