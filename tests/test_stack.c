#include "dds/stack.h"
#include "unity.h"
#include "test_structs.h"

/* ── dds_stack_init ──────────────────────────────────────────────────────── */

void dds_stack_init_should_return_ok(void) {
    dds_stack_t s;
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_stack_init(&s, sizeof(int), dds_alloc_stdlib()));
    dds_stack_free(&s);
}

void dds_stack_init_should_return_invalid_parameter_when_stack_null(void) {
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_stack_init(NULL, sizeof(int), dds_alloc_stdlib()));
}

void dds_stack_init_should_return_invalid_parameter_when_element_size_zero(void) {
    dds_stack_t s;
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_stack_init(&s, 0, dds_alloc_stdlib()));
}

void dds_stack_init_should_return_invalid_parameter_when_alloc_invalid(void) {
    dds_stack_t s;
    dds_alloc_t bad = { .malloc = NULL, .realloc = NULL, .free = NULL, .context = NULL };
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_stack_init(&s, sizeof(int), bad));
}

void dds_stack_init_should_create_correct_structure(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_EQUAL_size_t(0, dds_stack_get_size(&s));
    TEST_ASSERT_EQUAL_size_t(0, dds_stack_get_capacity(&s));
    TEST_ASSERT_TRUE(dds_stack_is_empty(&s));

    dds_stack_free(&s);
}

/* ── dds_stack_free ──────────────────────────────────────────────────────── */

void dds_stack_free_should_not_crash_when_null(void) {
    dds_stack_free(NULL);
}

void dds_stack_free_should_zero_fields(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);

    dds_stack_free(&s);

    TEST_ASSERT_EQUAL_size_t(0, dds_stack_get_size(&s));
    TEST_ASSERT_EQUAL_size_t(0, dds_stack_get_capacity(&s));
    TEST_ASSERT_NULL(s.vector.data);
}

/* ── dds_stack_clear ─────────────────────────────────────────────────────── */

void dds_stack_clear_should_return_ok(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_stack_clear(&s));
    dds_stack_free(&s);
}

void dds_stack_clear_should_return_invalid_parameter_when_stack_null(void) {
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_stack_clear(NULL));
}

void dds_stack_clear_should_reset_size_to_zero(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);
    dds_stack_push(&s, &v);

    dds_stack_clear(&s);

    TEST_ASSERT_EQUAL_size_t(0, dds_stack_get_size(&s));
    dds_stack_free(&s);
}

void dds_stack_clear_should_preserve_capacity(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);
    dds_stack_push(&s, &v);
    const size_t cap_before = dds_stack_get_capacity(&s);

    dds_stack_clear(&s);

    TEST_ASSERT_EQUAL_size_t(cap_before, dds_stack_get_capacity(&s));
    dds_stack_free(&s);
}

void dds_stack_clear_should_allow_push_after_clear(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 42;
    dds_stack_push(&s, &v);
    dds_stack_clear(&s);

    int w = 99;
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_stack_push(&s, &w));

    int out;
    dds_stack_pop(&s, &out);
    TEST_ASSERT_EQUAL_INT(99, out);
    dds_stack_free(&s);
}

/* ── dds_stack_reserve ───────────────────────────────────────────────────── */

void dds_stack_reserve_should_return_ok(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_stack_reserve(&s, 16));
    dds_stack_free(&s);
}

void dds_stack_reserve_should_return_invalid_parameter_when_stack_null(void) {
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_stack_reserve(NULL, 16));
}

void dds_stack_reserve_should_return_invalid_parameter_when_capacity_zero(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_stack_reserve(&s, 0));
    dds_stack_free(&s);
}

void dds_stack_reserve_should_set_capacity(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    dds_stack_reserve(&s, 32);
    TEST_ASSERT_GREATER_OR_EQUAL_size_t(32, dds_stack_get_capacity(&s));
    dds_stack_free(&s);
}

void dds_stack_reserve_should_not_change_size(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);

    dds_stack_reserve(&s, 64);

    TEST_ASSERT_EQUAL_size_t(1, dds_stack_get_size(&s));
    dds_stack_free(&s);
}

void dds_stack_reserve_should_be_noop_when_capacity_already_sufficient(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    dds_stack_reserve(&s, 32);
    const size_t cap = dds_stack_get_capacity(&s);

    dds_stack_reserve(&s, 8);

    TEST_ASSERT_EQUAL_size_t(cap, dds_stack_get_capacity(&s));
    dds_stack_free(&s);
}

void dds_stack_reserve_should_preserve_elements(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 77;
    dds_stack_push(&s, &v);

    dds_stack_reserve(&s, 64);

    int out;
    dds_stack_pop(&s, &out);
    TEST_ASSERT_EQUAL_INT(77, out);
    dds_stack_free(&s);
}

/* ── dds_stack_shrink_to_fit ─────────────────────────────────────────────── */

void dds_stack_shrink_to_fit_should_return_ok(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_stack_shrink_to_fit(&s));
    dds_stack_free(&s);
}

void dds_stack_shrink_to_fit_should_return_invalid_parameter_when_stack_null(void) {
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_stack_shrink_to_fit(NULL));
}

void dds_stack_shrink_to_fit_should_set_capacity_to_size(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    dds_stack_reserve(&s, 64);
    int v = 1;
    dds_stack_push(&s, &v);
    dds_stack_push(&s, &v);

    dds_stack_shrink_to_fit(&s);

    TEST_ASSERT_EQUAL_size_t(2, dds_stack_get_capacity(&s));
    dds_stack_free(&s);
}

void dds_stack_shrink_to_fit_should_not_change_size(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    dds_stack_reserve(&s, 64);
    int v = 1;
    dds_stack_push(&s, &v);
    dds_stack_push(&s, &v);

    dds_stack_shrink_to_fit(&s);

    TEST_ASSERT_EQUAL_size_t(2, dds_stack_get_size(&s));
    dds_stack_free(&s);
}

void dds_stack_shrink_to_fit_should_preserve_elements(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    dds_stack_reserve(&s, 64);
    int a = 10, b = 20;
    dds_stack_push(&s, &a);
    dds_stack_push(&s, &b);

    dds_stack_shrink_to_fit(&s);

    int out;
    dds_stack_pop(&s, &out);
    TEST_ASSERT_EQUAL_INT(20, out);
    dds_stack_pop(&s, &out);
    TEST_ASSERT_EQUAL_INT(10, out);
    dds_stack_free(&s);
}

void dds_stack_shrink_to_fit_should_free_buffer_when_empty(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    dds_stack_reserve(&s, 32);

    dds_stack_shrink_to_fit(&s);

    TEST_ASSERT_EQUAL_size_t(0, dds_stack_get_capacity(&s));
    TEST_ASSERT_NULL(s.vector.data);
    dds_stack_free(&s);
}

void dds_stack_shrink_to_fit_should_be_noop_when_already_fitted(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);
    dds_stack_shrink_to_fit(&s);
    const size_t cap = dds_stack_get_capacity(&s);

    dds_stack_shrink_to_fit(&s);

    TEST_ASSERT_EQUAL_size_t(cap, dds_stack_get_capacity(&s));
    dds_stack_free(&s);
}

/* ── dds_stack_push ──────────────────────────────────────────────────────── */

void dds_stack_push_should_return_ok(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_stack_push(&s, &v));
    dds_stack_free(&s);
}

void dds_stack_push_should_return_invalid_parameter_when_stack_null(void) {
    int v = 1;
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_stack_push(NULL, &v));
}

void dds_stack_push_should_return_invalid_parameter_when_element_null(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_stack_push(&s, NULL));
    dds_stack_free(&s);
}

void dds_stack_push_should_increase_size(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);
    dds_stack_push(&s, &v);
    dds_stack_push(&s, &v);
    TEST_ASSERT_EQUAL_size_t(3, dds_stack_get_size(&s));
    dds_stack_free(&s);
}

void dds_stack_push_should_store_correct_value(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 42;
    dds_stack_push(&s, &v);

    int out;
    dds_stack_pop(&s, &out);
    TEST_ASSERT_EQUAL_INT(42, out);
    dds_stack_free(&s);
}

void dds_stack_push_should_grow_capacity(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    /* Push enough elements to force at least two growth events */
    for (int i = 0; i < 32; i++) {
        TEST_ASSERT_EQUAL_INT(DDS_OK, dds_stack_push(&s, &i));
    }
    TEST_ASSERT_EQUAL_size_t(32, dds_stack_get_size(&s));
    TEST_ASSERT_GREATER_OR_EQUAL_size_t(32, dds_stack_get_capacity(&s));
    dds_stack_free(&s);
}

void dds_stack_push_should_store_struct(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(test_record_t), dds_alloc_stdlib());
    test_record_t r = make_record(7, "hello", 3.14);
    dds_stack_push(&s, &r);

    test_record_t out;
    dds_stack_pop(&s, &out);
    TEST_ASSERT_TRUE(records_equal(&r, &out));
    dds_stack_free(&s);
}

void dds_stack_push_should_allow_reuse_after_pop(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);
    dds_stack_pop(&s, NULL);

    int w = 99;
    dds_stack_push(&s, &w);

    int out;
    dds_stack_pop(&s, &out);
    TEST_ASSERT_EQUAL_INT(99, out);
    dds_stack_free(&s);
}

/* ── dds_stack_pop ───────────────────────────────────────────────────────── */

void dds_stack_pop_should_return_ok(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_stack_pop(&s, NULL));
    dds_stack_free(&s);
}

void dds_stack_pop_should_return_invalid_parameter_when_stack_null(void) {
    int out;
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_stack_pop(NULL, &out));
}

void dds_stack_pop_should_return_out_of_range_when_empty(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, dds_stack_pop(&s, NULL));
    dds_stack_free(&s);
}

void dds_stack_pop_should_return_correct_element(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 55;
    dds_stack_push(&s, &v);

    int out;
    dds_stack_pop(&s, &out);
    TEST_ASSERT_EQUAL_INT(55, out);
    dds_stack_free(&s);
}

void dds_stack_pop_should_decrease_size(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);
    dds_stack_push(&s, &v);

    dds_stack_pop(&s, NULL);

    TEST_ASSERT_EQUAL_size_t(1, dds_stack_get_size(&s));
    dds_stack_free(&s);
}

void dds_stack_pop_should_discard_when_element_null(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);

    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_stack_pop(&s, NULL));
    TEST_ASSERT_EQUAL_size_t(0, dds_stack_get_size(&s));
    dds_stack_free(&s);
}

void dds_stack_pop_should_return_elements_in_lifo_order(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int vals[] = { 10, 20, 30, 40, 50 };
    for (int i = 0; i < 5; i++) dds_stack_push(&s, &vals[i]);

    int out;
    dds_stack_pop(&s, &out); TEST_ASSERT_EQUAL_INT(50, out);
    dds_stack_pop(&s, &out); TEST_ASSERT_EQUAL_INT(40, out);
    dds_stack_pop(&s, &out); TEST_ASSERT_EQUAL_INT(30, out);
    dds_stack_pop(&s, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_stack_pop(&s, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_stack_free(&s);
}

void dds_stack_pop_should_leave_empty_stack_consistent(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);
    dds_stack_pop(&s, NULL);

    TEST_ASSERT_EQUAL_size_t(0, dds_stack_get_size(&s));
    TEST_ASSERT_TRUE(dds_stack_is_empty(&s));
    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, dds_stack_pop(&s, NULL));
    dds_stack_free(&s);
}

void dds_stack_pop_should_return_struct(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(test_record_t), dds_alloc_stdlib());
    test_record_t r = make_record(3, "world", 2.71);
    dds_stack_push(&s, &r);

    test_record_t out;
    dds_stack_pop(&s, &out);
    TEST_ASSERT_TRUE(records_equal(&r, &out));
    dds_stack_free(&s);
}

/* ── dds_stack_peek ──────────────────────────────────────────────────────── */

void dds_stack_peek_should_return_ok(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);
    int out;
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_stack_peek(&s, &out));
    dds_stack_free(&s);
}

void dds_stack_peek_should_return_invalid_parameter_when_stack_null(void) {
    int out;
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_stack_peek(NULL, &out));
}

void dds_stack_peek_should_return_invalid_parameter_when_element_null(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_stack_peek(&s, NULL));
    dds_stack_free(&s);
}

void dds_stack_peek_should_return_out_of_range_when_empty(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int out;
    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, dds_stack_peek(&s, &out));
    dds_stack_free(&s);
}

void dds_stack_peek_should_return_correct_element(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int a = 1, b = 2, c = 3;
    dds_stack_push(&s, &a);
    dds_stack_push(&s, &b);
    dds_stack_push(&s, &c);

    int out;
    dds_stack_peek(&s, &out);
    TEST_ASSERT_EQUAL_INT(3, out);
    dds_stack_free(&s);
}

void dds_stack_peek_should_not_remove_element(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 42;
    dds_stack_push(&s, &v);

    int out;
    dds_stack_peek(&s, &out);

    TEST_ASSERT_EQUAL_size_t(1, dds_stack_get_size(&s));
    dds_stack_free(&s);
}

void dds_stack_peek_should_return_same_value_as_subsequent_pop(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int a = 10, b = 20;
    dds_stack_push(&s, &a);
    dds_stack_push(&s, &b);

    int peeked, popped;
    dds_stack_peek(&s, &peeked);
    dds_stack_pop(&s, &popped);
    TEST_ASSERT_EQUAL_INT(peeked, popped);
    dds_stack_free(&s);
}

void dds_stack_peek_should_return_struct(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(test_record_t), dds_alloc_stdlib());
    test_record_t r = make_record(5, "peek", 1.0);
    dds_stack_push(&s, &r);

    test_record_t out;
    dds_stack_peek(&s, &out);
    TEST_ASSERT_TRUE(records_equal(&r, &out));
    TEST_ASSERT_EQUAL_size_t(1, dds_stack_get_size(&s));
    dds_stack_free(&s);
}

/* ── dds_stack_get_size ──────────────────────────────────────────────────── */

void dds_stack_get_size_should_return_zero_when_stack_null(void) {
    TEST_ASSERT_EQUAL_size_t(0, dds_stack_get_size(NULL));
}

void dds_stack_get_size_should_return_zero_on_empty_stack(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_size_t(0, dds_stack_get_size(&s));
    dds_stack_free(&s);
}

void dds_stack_get_size_should_return_correct_size(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);
    dds_stack_push(&s, &v);
    dds_stack_push(&s, &v);
    TEST_ASSERT_EQUAL_size_t(3, dds_stack_get_size(&s));
    dds_stack_free(&s);
}

/* ── dds_stack_get_capacity ──────────────────────────────────────────────── */

void dds_stack_get_capacity_should_return_zero_when_stack_null(void) {
    TEST_ASSERT_EQUAL_size_t(0, dds_stack_get_capacity(NULL));
}

void dds_stack_get_capacity_should_return_zero_on_empty_stack(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_size_t(0, dds_stack_get_capacity(&s));
    dds_stack_free(&s);
}

void dds_stack_get_capacity_should_return_at_least_size(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    for (int i = 0; i < 10; i++) dds_stack_push(&s, &v);
    TEST_ASSERT_GREATER_OR_EQUAL_size_t(dds_stack_get_size(&s), dds_stack_get_capacity(&s));
    dds_stack_free(&s);
}

/* ── dds_stack_is_empty ──────────────────────────────────────────────────── */

void dds_stack_is_empty_should_return_true_when_stack_null(void) {
    TEST_ASSERT_TRUE(dds_stack_is_empty(NULL));
}

void dds_stack_is_empty_should_return_true_on_empty_stack(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_TRUE(dds_stack_is_empty(&s));
    dds_stack_free(&s);
}

void dds_stack_is_empty_should_return_false_after_push(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);
    TEST_ASSERT_FALSE(dds_stack_is_empty(&s));
    dds_stack_free(&s);
}

void dds_stack_is_empty_should_return_true_after_all_elements_popped(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_stack_push(&s, &v);
    dds_stack_push(&s, &v);
    dds_stack_pop(&s, NULL);
    dds_stack_pop(&s, NULL);
    TEST_ASSERT_TRUE(dds_stack_is_empty(&s));
    dds_stack_free(&s);
}

/* ── corner cases ────────────────────────────────────────────────────────── */

void dds_stack_should_maintain_lifo_order_across_growth(void) {
    /* Push enough elements to trigger multiple growth events, then verify order */
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());

    for (int i = 0; i < 64; i++) dds_stack_push(&s, &i);

    for (int i = 63; i >= 0; i--) {
        int out;
        dds_stack_pop(&s, &out);
        TEST_ASSERT_EQUAL_INT(i, out);
    }
    dds_stack_free(&s);
}

void dds_stack_should_allow_push_after_clear_and_preserve_lifo(void) {
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int v = 999;
    dds_stack_push(&s, &v);
    dds_stack_push(&s, &v);
    dds_stack_clear(&s);

    int a = 1, b = 2, c = 3;
    dds_stack_push(&s, &a);
    dds_stack_push(&s, &b);
    dds_stack_push(&s, &c);

    int out;
    dds_stack_pop(&s, &out); TEST_ASSERT_EQUAL_INT(3, out);
    dds_stack_pop(&s, &out); TEST_ASSERT_EQUAL_INT(2, out);
    dds_stack_pop(&s, &out); TEST_ASSERT_EQUAL_INT(1, out);
    TEST_ASSERT_TRUE(dds_stack_is_empty(&s));
    dds_stack_free(&s);
}

void dds_stack_peek_then_pop_should_agree(void) {
    /* Peek and pop must return the same value for multiple rounds */
    dds_stack_t s;
    dds_stack_init(&s, sizeof(int), dds_alloc_stdlib());
    int vals[] = { 5, 15, 25, 35, 45 };
    for (int i = 0; i < 5; i++) dds_stack_push(&s, &vals[i]);

    for (int i = 4; i >= 0; i--) {
        int peeked, popped;
        dds_stack_peek(&s, &peeked);
        dds_stack_pop(&s, &popped);
        TEST_ASSERT_EQUAL_INT(vals[i], peeked);
        TEST_ASSERT_EQUAL_INT(vals[i], popped);
    }
    dds_stack_free(&s);
}
