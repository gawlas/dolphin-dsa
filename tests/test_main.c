#include <unity.h>

/* dds_linked_list_init */
void dds_linked_list_init_should_return_ok(void);
void dds_linked_list_init_should_return_invalid_parameter_when_list_null(void);
void dds_linked_list_init_should_return_invalid_parameter_when_element_size_zero(void);
void dds_linked_list_init_should_return_invalid_parameter_when_alloc_invalid(void);
void dds_linked_list_init_should_create_correct_structure(void);

/* dds_linked_list_free */
void dds_linked_list_free_should_not_crash_when_null(void);
void dds_linked_list_free_should_zero_fields(void);
void dds_linked_list_free_should_free_all_nodes(void);

/* dds_linked_list_clear */
void dds_linked_list_clear_should_return_ok(void);
void dds_linked_list_clear_should_return_invalid_parameter_when_list_null(void);
void dds_linked_list_clear_should_reset_size_to_zero(void);
void dds_linked_list_clear_should_null_head_and_tail(void);
void dds_linked_list_clear_should_preserve_element_size_and_alloc(void);
void dds_linked_list_clear_should_free_all_nodes(void);
void dds_linked_list_clear_should_allow_reuse(void);

/* dds_linked_list_push_back */
void dds_linked_list_push_back_should_return_ok(void);
void dds_linked_list_push_back_should_return_invalid_parameter_when_list_null(void);
void dds_linked_list_push_back_should_return_invalid_parameter_when_element_null(void);
void dds_linked_list_push_back_should_increase_size(void);
void dds_linked_list_push_back_should_store_correct_values(void);
void dds_linked_list_push_back_should_store_struct(void);

/* dds_linked_list_push_front */
void dds_linked_list_push_front_should_return_ok(void);
void dds_linked_list_push_front_should_return_invalid_parameter_when_list_null(void);
void dds_linked_list_push_front_should_return_invalid_parameter_when_element_null(void);
void dds_linked_list_push_front_should_increase_size(void);
void dds_linked_list_push_front_should_store_correct_values(void);
void dds_linked_list_push_front_should_store_struct(void);

/* dds_linked_list_pop_back */
void dds_linked_list_pop_back_should_return_ok(void);
void dds_linked_list_pop_back_should_return_invalid_parameter_when_list_null(void);
void dds_linked_list_pop_back_should_return_out_of_range_when_empty(void);
void dds_linked_list_pop_back_should_return_correct_element(void);
void dds_linked_list_pop_back_should_decrease_size(void);
void dds_linked_list_pop_back_should_discard_when_element_null(void);
void dds_linked_list_pop_back_should_return_elements_in_lifo_order(void);
void dds_linked_list_pop_back_should_leave_empty_list_consistent(void);
void dds_linked_list_pop_back_should_return_struct(void);

/* dds_linked_list_pop_front */
void dds_linked_list_pop_front_should_return_ok(void);
void dds_linked_list_pop_front_should_return_invalid_parameter_when_list_null(void);
void dds_linked_list_pop_front_should_return_out_of_range_when_empty(void);
void dds_linked_list_pop_front_should_return_correct_element(void);
void dds_linked_list_pop_front_should_decrease_size(void);
void dds_linked_list_pop_front_should_discard_when_element_null(void);
void dds_linked_list_pop_front_should_return_elements_in_fifo_order(void);
void dds_linked_list_pop_front_should_leave_empty_list_consistent(void);
void dds_linked_list_pop_front_should_return_struct(void);

/* dds_linked_list_get */
void dds_linked_list_get_should_return_ok(void);
void dds_linked_list_get_should_return_invalid_parameter_when_list_null(void);
void dds_linked_list_get_should_return_invalid_parameter_when_element_null(void);
void dds_linked_list_get_should_return_out_of_range_when_index_exceeds_size(void);
void dds_linked_list_get_should_return_out_of_range_when_empty(void);
void dds_linked_list_get_should_return_correct_values(void);
void dds_linked_list_get_should_not_modify_list(void);
void dds_linked_list_get_should_return_struct(void);

/* dds_linked_list_set */
void dds_linked_list_set_should_return_ok(void);
void dds_linked_list_set_should_return_invalid_parameter_when_list_null(void);
void dds_linked_list_set_should_return_invalid_parameter_when_element_null(void);
void dds_linked_list_set_should_return_out_of_range_when_index_exceeds_size(void);
void dds_linked_list_set_should_update_value(void);
void dds_linked_list_set_should_not_change_size(void);
void dds_linked_list_set_should_store_struct(void);

/* dds_linked_list_insert */
void dds_linked_list_insert_should_return_ok(void);
void dds_linked_list_insert_should_return_invalid_parameter_when_list_null(void);
void dds_linked_list_insert_should_return_invalid_parameter_when_element_null(void);
void dds_linked_list_insert_should_return_out_of_range_when_index_exceeds_size(void);
void dds_linked_list_insert_should_increase_size(void);
void dds_linked_list_insert_should_place_element_at_index(void);
void dds_linked_list_insert_at_zero_should_prepend(void);
void dds_linked_list_insert_at_size_should_append(void);
void dds_linked_list_insert_on_empty_list_should_work(void);
void dds_linked_list_insert_should_store_struct(void);

/* dds_linked_list_remove */
void dds_linked_list_remove_should_return_ok(void);
void dds_linked_list_remove_should_return_invalid_parameter_when_list_null(void);
void dds_linked_list_remove_should_return_out_of_range_when_index_exceeds_size(void);
void dds_linked_list_remove_should_return_out_of_range_when_empty(void);
void dds_linked_list_remove_should_return_correct_element(void);
void dds_linked_list_remove_should_decrease_size(void);
void dds_linked_list_remove_should_discard_when_element_null(void);
void dds_linked_list_remove_should_shift_remaining_elements(void);
void dds_linked_list_remove_should_leave_empty_list_consistent(void);
void dds_linked_list_remove_should_return_struct(void);

/* dds_linked_list_get_size */
void dds_linked_list_get_size_should_return_zero_when_list_null(void);
void dds_linked_list_get_size_should_return_zero_on_empty_list(void);
void dds_linked_list_get_size_should_return_correct_size(void);

/* dds_linked_list_is_empty */
void dds_linked_list_is_empty_should_return_true_when_list_null(void);
void dds_linked_list_is_empty_should_return_true_on_empty_list(void);
void dds_linked_list_is_empty_should_return_false_after_push(void);
void dds_linked_list_is_empty_should_return_true_after_all_elements_removed(void);

/* dds_vector_init */
void dds_vector_init_should_return_ok(void);
void dds_vector_init_should_return_error_when_invalid_alloc(void);
void dds_vector_init_should_create_correct_structure(void);

/* dds_vector_free */
void dds_vector_free_should_zero_alloc(void);

/* dds_vector_clear */
void dds_vector_clear_should_return_ok(void);
void dds_vector_clear_should_reset_size_to_zero(void);
void dds_vector_clear_should_preserve_capacity(void);
void dds_vector_clear_should_allow_push_after_clear(void);
void dds_vector_clear_should_return_invalid_parameter_when_vector_null(void);

/* dds_vector_shrink_to_fit */
void dds_vector_shrink_to_fit_should_return_ok(void);
void dds_vector_shrink_to_fit_should_set_capacity_to_size(void);
void dds_vector_shrink_to_fit_should_preserve_elements(void);
void dds_vector_shrink_to_fit_should_not_change_size(void);
void dds_vector_shrink_to_fit_should_free_buffer_when_empty(void);
void dds_vector_shrink_to_fit_should_be_noop_when_already_fitted(void);
void dds_vector_shrink_to_fit_should_return_invalid_parameter_when_vector_null(void);
void dds_vector_shrink_to_fit_should_return_overflow_on_buffer_size_overflow(void);

/* dds_vector_reserve */
void dds_vector_reserve_should_return_ok(void);
void dds_vector_reserve_should_set_capacity(void);
void dds_vector_reserve_should_not_change_size(void);
void dds_vector_reserve_should_be_noop_when_capacity_already_sufficient(void);
void dds_vector_reserve_should_return_invalid_parameter_when_vector_null(void);
void dds_vector_reserve_should_return_invalid_parameter_when_capacity_zero(void);
void dds_vector_reserve_should_return_overflow_on_buffer_size_overflow(void);

/* dds_vector_push_back */
void dds_vector_push_back_should_return_ok(void);
void dds_vector_push_back_should_increase_size(void);
void dds_vector_push_back_should_return_invalid_parameter_when_vector_null(void);
void dds_vector_push_back_should_return_invalid_parameter_when_element_null(void);
void dds_vector_push_back_should_store_correct_values(void);
void dds_vector_push_back_should_grow_capacity(void);
void dds_vector_push_back_should_return_overflow_when_capacity_overflows(void);
void dds_vector_push_back_should_return_overflow_when_buffer_size_overflows(void);
void dds_vector_push_back_should_store_struct(void);

/* dds_vector_insert */
void dds_vector_insert_should_return_ok(void);
void dds_vector_insert_should_increase_size(void);
void dds_vector_insert_should_place_element_at_index(void);
void dds_vector_insert_should_shift_elements_right(void);
void dds_vector_insert_at_end_should_append(void);
void dds_vector_insert_should_return_invalid_parameter_when_vector_null(void);
void dds_vector_insert_should_return_invalid_parameter_when_element_null(void);
void dds_vector_insert_should_return_out_of_range_when_index_exceeds_size(void);
void dds_vector_insert_should_store_struct(void);

/* dds_vector_remove */
void dds_vector_remove_should_return_ok(void);
void dds_vector_remove_should_return_correct_element(void);
void dds_vector_remove_should_decrease_size(void);
void dds_vector_remove_should_shift_elements_left(void);
void dds_vector_remove_should_not_change_capacity(void);
void dds_vector_remove_should_discard_when_element_null(void);
void dds_vector_remove_should_return_invalid_parameter_when_vector_null(void);
void dds_vector_remove_should_return_out_of_range_when_index_exceeds_size(void);
void dds_vector_remove_should_return_out_of_range_on_empty_vector(void);
void dds_vector_remove_should_return_struct(void);

/* dds_vector_pop_back */
void dds_vector_pop_back_should_return_ok(void);
void dds_vector_pop_back_should_return_correct_element(void);
void dds_vector_pop_back_should_decrease_size(void);
void dds_vector_pop_back_should_not_change_capacity(void);
void dds_vector_pop_back_should_return_elements_in_lifo_order(void);
void dds_vector_pop_back_should_discard_when_element_null(void);
void dds_vector_pop_back_should_return_invalid_parameter_when_vector_null(void);
void dds_vector_pop_back_should_return_out_of_range_when_empty(void);
void dds_vector_pop_back_should_return_struct(void);

/* dds_vector_get */
void dds_vector_get_should_return_ok(void);
void dds_vector_get_should_return_invalid_parameter_when_vector_null(void);
void dds_vector_get_should_return_invalid_parameter_when_element_null(void);
void dds_vector_get_should_return_out_of_range_when_index_exceeds_size(void);
void dds_vector_get_should_return_out_of_range_on_empty_vector(void);
void dds_vector_get_should_return_struct(void);

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

/* dds_vector_is_empty */
void dds_vector_is_empty_should_return_true_when_vector_null(void);
void dds_vector_is_empty_should_return_true_on_empty_vector(void);
void dds_vector_is_empty_should_return_false_after_push(void);
void dds_vector_is_empty_should_return_true_after_all_elements_popped(void);

/* dds_vector_at */
void dds_vector_at_should_return_null_when_vector_null(void);
void dds_vector_at_should_return_null_on_empty_vector(void);
void dds_vector_at_should_return_null_when_index_out_of_bounds(void);
void dds_vector_at_should_return_correct_value(void);
void dds_vector_at_should_allow_mutation(void);
void dds_vector_at_should_return_pointer_to_struct(void);

/* dds_vector_index */
void dds_vector_index_should_return_correct_value(void);
void dds_vector_index_should_allow_mutation(void);
void dds_vector_index_local_copy_should_not_affect_vector(void);
void dds_vector_index_should_return_struct(void);

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(dds_linked_list_init_should_return_ok);
    RUN_TEST(dds_linked_list_init_should_return_invalid_parameter_when_list_null);
    RUN_TEST(dds_linked_list_init_should_return_invalid_parameter_when_element_size_zero);
    RUN_TEST(dds_linked_list_init_should_return_invalid_parameter_when_alloc_invalid);
    RUN_TEST(dds_linked_list_init_should_create_correct_structure);

    RUN_TEST(dds_linked_list_free_should_not_crash_when_null);
    RUN_TEST(dds_linked_list_free_should_zero_fields);
    RUN_TEST(dds_linked_list_free_should_free_all_nodes);

    RUN_TEST(dds_linked_list_clear_should_return_ok);
    RUN_TEST(dds_linked_list_clear_should_return_invalid_parameter_when_list_null);
    RUN_TEST(dds_linked_list_clear_should_reset_size_to_zero);
    RUN_TEST(dds_linked_list_clear_should_null_head_and_tail);
    RUN_TEST(dds_linked_list_clear_should_preserve_element_size_and_alloc);
    RUN_TEST(dds_linked_list_clear_should_free_all_nodes);
    RUN_TEST(dds_linked_list_clear_should_allow_reuse);

    RUN_TEST(dds_linked_list_push_back_should_return_ok);
    RUN_TEST(dds_linked_list_push_back_should_return_invalid_parameter_when_list_null);
    RUN_TEST(dds_linked_list_push_back_should_return_invalid_parameter_when_element_null);
    RUN_TEST(dds_linked_list_push_back_should_increase_size);
    RUN_TEST(dds_linked_list_push_back_should_store_correct_values);
    RUN_TEST(dds_linked_list_push_back_should_store_struct);

    RUN_TEST(dds_linked_list_push_front_should_return_ok);
    RUN_TEST(dds_linked_list_push_front_should_return_invalid_parameter_when_list_null);
    RUN_TEST(dds_linked_list_push_front_should_return_invalid_parameter_when_element_null);
    RUN_TEST(dds_linked_list_push_front_should_increase_size);
    RUN_TEST(dds_linked_list_push_front_should_store_correct_values);
    RUN_TEST(dds_linked_list_push_front_should_store_struct);

    RUN_TEST(dds_linked_list_pop_back_should_return_ok);
    RUN_TEST(dds_linked_list_pop_back_should_return_invalid_parameter_when_list_null);
    RUN_TEST(dds_linked_list_pop_back_should_return_out_of_range_when_empty);
    RUN_TEST(dds_linked_list_pop_back_should_return_correct_element);
    RUN_TEST(dds_linked_list_pop_back_should_decrease_size);
    RUN_TEST(dds_linked_list_pop_back_should_discard_when_element_null);
    RUN_TEST(dds_linked_list_pop_back_should_return_elements_in_lifo_order);
    RUN_TEST(dds_linked_list_pop_back_should_leave_empty_list_consistent);
    RUN_TEST(dds_linked_list_pop_back_should_return_struct);

    RUN_TEST(dds_linked_list_pop_front_should_return_ok);
    RUN_TEST(dds_linked_list_pop_front_should_return_invalid_parameter_when_list_null);
    RUN_TEST(dds_linked_list_pop_front_should_return_out_of_range_when_empty);
    RUN_TEST(dds_linked_list_pop_front_should_return_correct_element);
    RUN_TEST(dds_linked_list_pop_front_should_decrease_size);
    RUN_TEST(dds_linked_list_pop_front_should_discard_when_element_null);
    RUN_TEST(dds_linked_list_pop_front_should_return_elements_in_fifo_order);
    RUN_TEST(dds_linked_list_pop_front_should_leave_empty_list_consistent);
    RUN_TEST(dds_linked_list_pop_front_should_return_struct);

    RUN_TEST(dds_linked_list_get_should_return_ok);
    RUN_TEST(dds_linked_list_get_should_return_invalid_parameter_when_list_null);
    RUN_TEST(dds_linked_list_get_should_return_invalid_parameter_when_element_null);
    RUN_TEST(dds_linked_list_get_should_return_out_of_range_when_index_exceeds_size);
    RUN_TEST(dds_linked_list_get_should_return_out_of_range_when_empty);
    RUN_TEST(dds_linked_list_get_should_return_correct_values);
    RUN_TEST(dds_linked_list_get_should_not_modify_list);
    RUN_TEST(dds_linked_list_get_should_return_struct);

    RUN_TEST(dds_linked_list_set_should_return_ok);
    RUN_TEST(dds_linked_list_set_should_return_invalid_parameter_when_list_null);
    RUN_TEST(dds_linked_list_set_should_return_invalid_parameter_when_element_null);
    RUN_TEST(dds_linked_list_set_should_return_out_of_range_when_index_exceeds_size);
    RUN_TEST(dds_linked_list_set_should_update_value);
    RUN_TEST(dds_linked_list_set_should_not_change_size);
    RUN_TEST(dds_linked_list_set_should_store_struct);

    RUN_TEST(dds_linked_list_insert_should_return_ok);
    RUN_TEST(dds_linked_list_insert_should_return_invalid_parameter_when_list_null);
    RUN_TEST(dds_linked_list_insert_should_return_invalid_parameter_when_element_null);
    RUN_TEST(dds_linked_list_insert_should_return_out_of_range_when_index_exceeds_size);
    RUN_TEST(dds_linked_list_insert_should_increase_size);
    RUN_TEST(dds_linked_list_insert_should_place_element_at_index);
    RUN_TEST(dds_linked_list_insert_at_zero_should_prepend);
    RUN_TEST(dds_linked_list_insert_at_size_should_append);
    RUN_TEST(dds_linked_list_insert_on_empty_list_should_work);
    RUN_TEST(dds_linked_list_insert_should_store_struct);

    RUN_TEST(dds_linked_list_remove_should_return_ok);
    RUN_TEST(dds_linked_list_remove_should_return_invalid_parameter_when_list_null);
    RUN_TEST(dds_linked_list_remove_should_return_out_of_range_when_index_exceeds_size);
    RUN_TEST(dds_linked_list_remove_should_return_out_of_range_when_empty);
    RUN_TEST(dds_linked_list_remove_should_return_correct_element);
    RUN_TEST(dds_linked_list_remove_should_decrease_size);
    RUN_TEST(dds_linked_list_remove_should_discard_when_element_null);
    RUN_TEST(dds_linked_list_remove_should_shift_remaining_elements);
    RUN_TEST(dds_linked_list_remove_should_leave_empty_list_consistent);
    RUN_TEST(dds_linked_list_remove_should_return_struct);

    RUN_TEST(dds_linked_list_get_size_should_return_zero_when_list_null);
    RUN_TEST(dds_linked_list_get_size_should_return_zero_on_empty_list);
    RUN_TEST(dds_linked_list_get_size_should_return_correct_size);

    RUN_TEST(dds_linked_list_is_empty_should_return_true_when_list_null);
    RUN_TEST(dds_linked_list_is_empty_should_return_true_on_empty_list);
    RUN_TEST(dds_linked_list_is_empty_should_return_false_after_push);
    RUN_TEST(dds_linked_list_is_empty_should_return_true_after_all_elements_removed);

    RUN_TEST(dds_vector_init_should_return_ok);
    RUN_TEST(dds_vector_init_should_return_error_when_invalid_alloc);
    RUN_TEST(dds_vector_init_should_create_correct_structure);

    RUN_TEST(dds_vector_free_should_zero_alloc);

    RUN_TEST(dds_vector_clear_should_return_ok);
    RUN_TEST(dds_vector_clear_should_reset_size_to_zero);
    RUN_TEST(dds_vector_clear_should_preserve_capacity);
    RUN_TEST(dds_vector_clear_should_allow_push_after_clear);
    RUN_TEST(dds_vector_clear_should_return_invalid_parameter_when_vector_null);

    RUN_TEST(dds_vector_shrink_to_fit_should_return_ok);
    RUN_TEST(dds_vector_shrink_to_fit_should_set_capacity_to_size);
    RUN_TEST(dds_vector_shrink_to_fit_should_preserve_elements);
    RUN_TEST(dds_vector_shrink_to_fit_should_not_change_size);
    RUN_TEST(dds_vector_shrink_to_fit_should_free_buffer_when_empty);
    RUN_TEST(dds_vector_shrink_to_fit_should_be_noop_when_already_fitted);
    RUN_TEST(dds_vector_shrink_to_fit_should_return_invalid_parameter_when_vector_null);
    RUN_TEST(dds_vector_shrink_to_fit_should_return_overflow_on_buffer_size_overflow);

    RUN_TEST(dds_vector_reserve_should_return_ok);
    RUN_TEST(dds_vector_reserve_should_set_capacity);
    RUN_TEST(dds_vector_reserve_should_not_change_size);
    RUN_TEST(dds_vector_reserve_should_be_noop_when_capacity_already_sufficient);
    RUN_TEST(dds_vector_reserve_should_return_invalid_parameter_when_vector_null);
    RUN_TEST(dds_vector_reserve_should_return_invalid_parameter_when_capacity_zero);
    RUN_TEST(dds_vector_reserve_should_return_overflow_on_buffer_size_overflow);

    RUN_TEST(dds_vector_push_back_should_return_ok);
    RUN_TEST(dds_vector_push_back_should_increase_size);
    RUN_TEST(dds_vector_push_back_should_return_invalid_parameter_when_vector_null);
    RUN_TEST(dds_vector_push_back_should_return_invalid_parameter_when_element_null);
    RUN_TEST(dds_vector_push_back_should_store_correct_values);
    RUN_TEST(dds_vector_push_back_should_grow_capacity);
    RUN_TEST(dds_vector_push_back_should_return_overflow_when_capacity_overflows);
    RUN_TEST(dds_vector_push_back_should_return_overflow_when_buffer_size_overflows);
    RUN_TEST(dds_vector_push_back_should_store_struct);

    RUN_TEST(dds_vector_insert_should_return_ok);
    RUN_TEST(dds_vector_insert_should_increase_size);
    RUN_TEST(dds_vector_insert_should_place_element_at_index);
    RUN_TEST(dds_vector_insert_should_shift_elements_right);
    RUN_TEST(dds_vector_insert_at_end_should_append);
    RUN_TEST(dds_vector_insert_should_return_invalid_parameter_when_vector_null);
    RUN_TEST(dds_vector_insert_should_return_invalid_parameter_when_element_null);
    RUN_TEST(dds_vector_insert_should_return_out_of_range_when_index_exceeds_size);
    RUN_TEST(dds_vector_insert_should_store_struct);

    RUN_TEST(dds_vector_remove_should_return_ok);
    RUN_TEST(dds_vector_remove_should_return_correct_element);
    RUN_TEST(dds_vector_remove_should_decrease_size);
    RUN_TEST(dds_vector_remove_should_shift_elements_left);
    RUN_TEST(dds_vector_remove_should_not_change_capacity);
    RUN_TEST(dds_vector_remove_should_discard_when_element_null);
    RUN_TEST(dds_vector_remove_should_return_invalid_parameter_when_vector_null);
    RUN_TEST(dds_vector_remove_should_return_out_of_range_when_index_exceeds_size);
    RUN_TEST(dds_vector_remove_should_return_out_of_range_on_empty_vector);
    RUN_TEST(dds_vector_remove_should_return_struct);

    RUN_TEST(dds_vector_pop_back_should_return_ok);
    RUN_TEST(dds_vector_pop_back_should_return_correct_element);
    RUN_TEST(dds_vector_pop_back_should_decrease_size);
    RUN_TEST(dds_vector_pop_back_should_not_change_capacity);
    RUN_TEST(dds_vector_pop_back_should_return_elements_in_lifo_order);
    RUN_TEST(dds_vector_pop_back_should_discard_when_element_null);
    RUN_TEST(dds_vector_pop_back_should_return_invalid_parameter_when_vector_null);
    RUN_TEST(dds_vector_pop_back_should_return_out_of_range_when_empty);
    RUN_TEST(dds_vector_pop_back_should_return_struct);

    RUN_TEST(dds_vector_get_should_return_ok);
    RUN_TEST(dds_vector_get_should_return_invalid_parameter_when_vector_null);
    RUN_TEST(dds_vector_get_should_return_invalid_parameter_when_element_null);
    RUN_TEST(dds_vector_get_should_return_out_of_range_when_index_exceeds_size);
    RUN_TEST(dds_vector_get_should_return_out_of_range_on_empty_vector);
    RUN_TEST(dds_vector_get_should_return_struct);

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

    RUN_TEST(dds_vector_is_empty_should_return_true_when_vector_null);
    RUN_TEST(dds_vector_is_empty_should_return_true_on_empty_vector);
    RUN_TEST(dds_vector_is_empty_should_return_false_after_push);
    RUN_TEST(dds_vector_is_empty_should_return_true_after_all_elements_popped);

    RUN_TEST(dds_vector_at_should_return_null_when_vector_null);
    RUN_TEST(dds_vector_at_should_return_null_on_empty_vector);
    RUN_TEST(dds_vector_at_should_return_null_when_index_out_of_bounds);
    RUN_TEST(dds_vector_at_should_return_correct_value);
    RUN_TEST(dds_vector_at_should_allow_mutation);
    RUN_TEST(dds_vector_at_should_return_pointer_to_struct);

    RUN_TEST(dds_vector_index_should_return_correct_value);
    RUN_TEST(dds_vector_index_should_allow_mutation);
    RUN_TEST(dds_vector_index_local_copy_should_not_affect_vector);
    RUN_TEST(dds_vector_index_should_return_struct);

    return UNITY_END();
}
