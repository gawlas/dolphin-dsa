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
void dds_vector_push_back_should_return_overflow_when_capacity_overflows(void);
void dds_vector_push_back_should_return_overflow_when_buffer_size_overflows(void);

/* dds_vector_pop_back */
void dds_vector_pop_back_should_return_ok(void);
void dds_vector_pop_back_should_return_correct_element(void);
void dds_vector_pop_back_should_decrease_size(void);
void dds_vector_pop_back_should_not_change_capacity(void);
void dds_vector_pop_back_should_return_elements_in_lifo_order(void);
void dds_vector_pop_back_should_discard_when_element_null(void);
void dds_vector_pop_back_should_return_invalid_parameter_when_vector_null(void);
void dds_vector_pop_back_should_return_out_of_range_when_empty(void);

/* dds_vector_get */
void dds_vector_get_should_return_ok(void);
void dds_vector_get_should_return_invalid_parameter_when_vector_null(void);
void dds_vector_get_should_return_invalid_parameter_when_element_null(void);
void dds_vector_get_should_return_out_of_range_when_index_exceeds_size(void);
void dds_vector_get_should_return_out_of_range_on_empty_vector(void);

/* dds_vector_get_size */
void dds_vector_get_size_should_return_zero_when_vector_null(void);
void dds_vector_get_size_should_return_zero_on_empty_vector(void);
void dds_vector_get_size_should_return_correct_size(void);

/* dds_vector_get_capacity */
void dds_vector_get_capacity_should_return_zero_when_vector_null(void);
void dds_vector_get_capacity_should_return_zero_on_empty_vector(void);
void dds_vector_get_capacity_should_return_at_least_size(void);

/* dds_vector_get_data */
void dds_vector_get_data_should_return_null_when_vector_null(void);
void dds_vector_get_data_should_return_null_on_empty_vector(void);
void dds_vector_get_data_should_return_non_null_after_push(void);
void dds_vector_get_data_should_point_to_correct_values(void);

/* dds_vector_at */
void dds_vector_at_should_return_null_when_vector_null(void);
void dds_vector_at_should_return_null_on_empty_vector(void);
void dds_vector_at_should_return_null_when_index_out_of_bounds(void);
void dds_vector_at_should_return_correct_value(void);
void dds_vector_at_should_allow_mutation(void);

/* dds_vector_index */
void dds_vector_index_should_return_correct_value(void);
void dds_vector_index_should_allow_mutation(void);
void dds_vector_index_local_copy_should_not_affect_vector(void);

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
    RUN_TEST(dds_vector_push_back_should_return_overflow_when_capacity_overflows);
    RUN_TEST(dds_vector_push_back_should_return_overflow_when_buffer_size_overflows);

    RUN_TEST(dds_vector_pop_back_should_return_ok);
    RUN_TEST(dds_vector_pop_back_should_return_correct_element);
    RUN_TEST(dds_vector_pop_back_should_decrease_size);
    RUN_TEST(dds_vector_pop_back_should_not_change_capacity);
    RUN_TEST(dds_vector_pop_back_should_return_elements_in_lifo_order);
    RUN_TEST(dds_vector_pop_back_should_discard_when_element_null);
    RUN_TEST(dds_vector_pop_back_should_return_invalid_parameter_when_vector_null);
    RUN_TEST(dds_vector_pop_back_should_return_out_of_range_when_empty);

    RUN_TEST(dds_vector_get_should_return_ok);
    RUN_TEST(dds_vector_get_should_return_invalid_parameter_when_vector_null);
    RUN_TEST(dds_vector_get_should_return_invalid_parameter_when_element_null);
    RUN_TEST(dds_vector_get_should_return_out_of_range_when_index_exceeds_size);
    RUN_TEST(dds_vector_get_should_return_out_of_range_on_empty_vector);

    RUN_TEST(dds_vector_get_size_should_return_zero_when_vector_null);
    RUN_TEST(dds_vector_get_size_should_return_zero_on_empty_vector);
    RUN_TEST(dds_vector_get_size_should_return_correct_size);

    RUN_TEST(dds_vector_get_capacity_should_return_zero_when_vector_null);
    RUN_TEST(dds_vector_get_capacity_should_return_zero_on_empty_vector);
    RUN_TEST(dds_vector_get_capacity_should_return_at_least_size);

    RUN_TEST(dds_vector_get_data_should_return_null_when_vector_null);
    RUN_TEST(dds_vector_get_data_should_return_null_on_empty_vector);
    RUN_TEST(dds_vector_get_data_should_return_non_null_after_push);
    RUN_TEST(dds_vector_get_data_should_point_to_correct_values);

    RUN_TEST(dds_vector_at_should_return_null_when_vector_null);
    RUN_TEST(dds_vector_at_should_return_null_on_empty_vector);
    RUN_TEST(dds_vector_at_should_return_null_when_index_out_of_bounds);
    RUN_TEST(dds_vector_at_should_return_correct_value);
    RUN_TEST(dds_vector_at_should_allow_mutation);

    RUN_TEST(dds_vector_index_should_return_correct_value);
    RUN_TEST(dds_vector_index_should_allow_mutation);
    RUN_TEST(dds_vector_index_local_copy_should_not_affect_vector);

    return UNITY_END();
}
