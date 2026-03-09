#include "dds/queue.h"
#include "unity.h"
#include "test_structs.h"

/* ── dds_queue_init ──────────────────────────────────────────────────────── */

void dds_queue_init_should_return_ok(void) {
    dds_queue_t q;
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_queue_init(&q, sizeof(int), dds_alloc_stdlib()));
    dds_queue_free(&q);
}

void dds_queue_init_should_return_invalid_parameter_when_queue_null(void) {
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_queue_init(NULL, sizeof(int), dds_alloc_stdlib()));
}

void dds_queue_init_should_return_invalid_parameter_when_element_size_zero(void) {
    dds_queue_t q;
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_queue_init(&q, 0, dds_alloc_stdlib()));
}

void dds_queue_init_should_return_invalid_parameter_when_alloc_invalid(void) {
    dds_queue_t q;
    dds_alloc_t bad = { .malloc = NULL, .realloc = NULL, .free = NULL, .context = NULL };
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_queue_init(&q, sizeof(int), bad));
}

void dds_queue_init_should_create_correct_structure(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());

    TEST_ASSERT_EQUAL_size_t(0, dds_queue_get_size(&q));
    TEST_ASSERT_EQUAL_size_t(0, dds_queue_get_capacity(&q));
    TEST_ASSERT_TRUE(dds_queue_is_empty(&q));

    dds_queue_free(&q);
}

/* ── dds_queue_free ──────────────────────────────────────────────────────── */

void dds_queue_free_should_not_crash_when_null(void) {
    dds_queue_free(NULL);
}

void dds_queue_free_should_zero_fields(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);

    dds_queue_free(&q);

    TEST_ASSERT_EQUAL_size_t(0, dds_queue_get_size(&q));
    TEST_ASSERT_EQUAL_size_t(0, dds_queue_get_capacity(&q));
    TEST_ASSERT_NULL(q.ring_buffer.data);
}

/* ── dds_queue_clear ─────────────────────────────────────────────────────── */

void dds_queue_clear_should_return_ok(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_queue_clear(&q));
    dds_queue_free(&q);
}

void dds_queue_clear_should_return_invalid_parameter_when_queue_null(void) {
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_queue_clear(NULL));
}

void dds_queue_clear_should_reset_size_to_zero(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);
    dds_queue_enqueue(&q, &v);

    dds_queue_clear(&q);

    TEST_ASSERT_EQUAL_size_t(0, dds_queue_get_size(&q));
    dds_queue_free(&q);
}

void dds_queue_clear_should_preserve_capacity(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);
    dds_queue_enqueue(&q, &v);
    const size_t cap_before = dds_queue_get_capacity(&q);

    dds_queue_clear(&q);

    TEST_ASSERT_EQUAL_size_t(cap_before, dds_queue_get_capacity(&q));
    dds_queue_free(&q);
}

void dds_queue_clear_should_allow_enqueue_after_clear(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 42;
    dds_queue_enqueue(&q, &v);
    dds_queue_clear(&q);

    int w = 99;
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_queue_enqueue(&q, &w));

    int out;
    dds_queue_dequeue(&q, &out);
    TEST_ASSERT_EQUAL_INT(99, out);
    dds_queue_free(&q);
}

/* ── dds_queue_reserve ───────────────────────────────────────────────────── */

void dds_queue_reserve_should_return_ok(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_queue_reserve(&q, 16));
    dds_queue_free(&q);
}

void dds_queue_reserve_should_return_invalid_parameter_when_queue_null(void) {
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_queue_reserve(NULL, 16));
}

void dds_queue_reserve_should_return_invalid_parameter_when_capacity_zero(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_queue_reserve(&q, 0));
    dds_queue_free(&q);
}

void dds_queue_reserve_should_set_capacity(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    dds_queue_reserve(&q, 32);
    TEST_ASSERT_GREATER_OR_EQUAL_size_t(32, dds_queue_get_capacity(&q));
    dds_queue_free(&q);
}

void dds_queue_reserve_should_not_change_size(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);

    dds_queue_reserve(&q, 64);

    TEST_ASSERT_EQUAL_size_t(1, dds_queue_get_size(&q));
    dds_queue_free(&q);
}

void dds_queue_reserve_should_be_noop_when_capacity_already_sufficient(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    dds_queue_reserve(&q, 32);
    const size_t cap = dds_queue_get_capacity(&q);

    dds_queue_reserve(&q, 8);

    TEST_ASSERT_EQUAL_size_t(cap, dds_queue_get_capacity(&q));
    dds_queue_free(&q);
}

void dds_queue_reserve_should_preserve_elements(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int a = 10, b = 20;
    dds_queue_enqueue(&q, &a);
    dds_queue_enqueue(&q, &b);

    dds_queue_reserve(&q, 64);

    int out;
    dds_queue_dequeue(&q, &out);
    TEST_ASSERT_EQUAL_INT(10, out);
    dds_queue_dequeue(&q, &out);
    TEST_ASSERT_EQUAL_INT(20, out);
    dds_queue_free(&q);
}

/* ── dds_queue_shrink_to_fit ─────────────────────────────────────────────── */

void dds_queue_shrink_to_fit_should_return_ok(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_queue_shrink_to_fit(&q));
    dds_queue_free(&q);
}

void dds_queue_shrink_to_fit_should_return_invalid_parameter_when_queue_null(void) {
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_queue_shrink_to_fit(NULL));
}

void dds_queue_shrink_to_fit_should_set_capacity_to_size(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    dds_queue_reserve(&q, 64);
    int v = 1;
    dds_queue_enqueue(&q, &v);
    dds_queue_enqueue(&q, &v);

    dds_queue_shrink_to_fit(&q);

    TEST_ASSERT_EQUAL_size_t(2, dds_queue_get_capacity(&q));
    dds_queue_free(&q);
}

void dds_queue_shrink_to_fit_should_not_change_size(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    dds_queue_reserve(&q, 64);
    int v = 1;
    dds_queue_enqueue(&q, &v);
    dds_queue_enqueue(&q, &v);

    dds_queue_shrink_to_fit(&q);

    TEST_ASSERT_EQUAL_size_t(2, dds_queue_get_size(&q));
    dds_queue_free(&q);
}

void dds_queue_shrink_to_fit_should_preserve_elements(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    dds_queue_reserve(&q, 64);
    int a = 10, b = 20;
    dds_queue_enqueue(&q, &a);
    dds_queue_enqueue(&q, &b);

    dds_queue_shrink_to_fit(&q);

    int out;
    dds_queue_dequeue(&q, &out);
    TEST_ASSERT_EQUAL_INT(10, out);
    dds_queue_dequeue(&q, &out);
    TEST_ASSERT_EQUAL_INT(20, out);
    dds_queue_free(&q);
}

void dds_queue_shrink_to_fit_should_free_buffer_when_empty(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    dds_queue_reserve(&q, 32);

    dds_queue_shrink_to_fit(&q);

    TEST_ASSERT_EQUAL_size_t(0, dds_queue_get_capacity(&q));
    TEST_ASSERT_NULL(q.ring_buffer.data);
    dds_queue_free(&q);
}

void dds_queue_shrink_to_fit_should_be_noop_when_already_fitted(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);
    dds_queue_shrink_to_fit(&q);
    const size_t cap = dds_queue_get_capacity(&q);

    dds_queue_shrink_to_fit(&q);

    TEST_ASSERT_EQUAL_size_t(cap, dds_queue_get_capacity(&q));
    dds_queue_free(&q);
}

void dds_queue_shrink_to_fit_should_preserve_elements_when_data_is_wrapped(void) {
    /*
     * Force wrap-around: reserve exact capacity, dequeue half from the front
     * (advancing head), then enqueue the same count at the back (advancing
     * tail past capacity boundary). shrink_to_fit must linearise the data.
     */
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    dds_queue_reserve(&q, 8);

    for (int i = 0; i < 8; i++) dds_queue_enqueue(&q, &i);
    /* dequeue 4 from front — head advances to slot 4 */
    for (int i = 0; i < 4; i++) dds_queue_dequeue(&q, NULL);
    /* enqueue 4 more — tail wraps past end */
    for (int i = 8; i < 12; i++) dds_queue_enqueue(&q, &i);

    dds_queue_shrink_to_fit(&q);

    /* should now dequeue 4, 5, 6, 7, 8, 9, 10, 11 in order */
    for (int expected = 4; expected < 12; expected++) {
        int out;
        TEST_ASSERT_EQUAL_INT(DDS_OK, dds_queue_dequeue(&q, &out));
        TEST_ASSERT_EQUAL_INT(expected, out);
    }
    dds_queue_free(&q);
}

/* ── dds_queue_enqueue ───────────────────────────────────────────────────── */

void dds_queue_enqueue_should_return_ok(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_queue_enqueue(&q, &v));
    dds_queue_free(&q);
}

void dds_queue_enqueue_should_return_invalid_parameter_when_queue_null(void) {
    int v = 1;
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_queue_enqueue(NULL, &v));
}

void dds_queue_enqueue_should_return_invalid_parameter_when_element_null(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_queue_enqueue(&q, NULL));
    dds_queue_free(&q);
}

void dds_queue_enqueue_should_increase_size(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);
    dds_queue_enqueue(&q, &v);
    dds_queue_enqueue(&q, &v);
    TEST_ASSERT_EQUAL_size_t(3, dds_queue_get_size(&q));
    dds_queue_free(&q);
}

void dds_queue_enqueue_should_store_correct_value(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 42;
    dds_queue_enqueue(&q, &v);

    int out;
    dds_queue_dequeue(&q, &out);
    TEST_ASSERT_EQUAL_INT(42, out);
    dds_queue_free(&q);
}

void dds_queue_enqueue_should_grow_capacity(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    for (int i = 0; i < 32; i++) {
        TEST_ASSERT_EQUAL_INT(DDS_OK, dds_queue_enqueue(&q, &i));
    }
    TEST_ASSERT_EQUAL_size_t(32, dds_queue_get_size(&q));
    TEST_ASSERT_GREATER_OR_EQUAL_size_t(32, dds_queue_get_capacity(&q));
    dds_queue_free(&q);
}

void dds_queue_enqueue_should_store_struct(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(test_record_t), dds_alloc_stdlib());
    test_record_t r = make_record(7, "hello", 3.14);
    dds_queue_enqueue(&q, &r);

    test_record_t out;
    dds_queue_dequeue(&q, &out);
    TEST_ASSERT_TRUE(records_equal(&r, &out));
    dds_queue_free(&q);
}

/* ── dds_queue_dequeue ───────────────────────────────────────────────────── */

void dds_queue_dequeue_should_return_ok(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_queue_dequeue(&q, NULL));
    dds_queue_free(&q);
}

void dds_queue_dequeue_should_return_invalid_parameter_when_queue_null(void) {
    int out;
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_queue_dequeue(NULL, &out));
}

void dds_queue_dequeue_should_return_out_of_range_when_empty(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, dds_queue_dequeue(&q, NULL));
    dds_queue_free(&q);
}

void dds_queue_dequeue_should_return_correct_element(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 55;
    dds_queue_enqueue(&q, &v);

    int out;
    dds_queue_dequeue(&q, &out);
    TEST_ASSERT_EQUAL_INT(55, out);
    dds_queue_free(&q);
}

void dds_queue_dequeue_should_decrease_size(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);
    dds_queue_enqueue(&q, &v);

    dds_queue_dequeue(&q, NULL);

    TEST_ASSERT_EQUAL_size_t(1, dds_queue_get_size(&q));
    dds_queue_free(&q);
}

void dds_queue_dequeue_should_discard_when_element_null(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);

    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_queue_dequeue(&q, NULL));
    TEST_ASSERT_EQUAL_size_t(0, dds_queue_get_size(&q));
    dds_queue_free(&q);
}

void dds_queue_dequeue_should_return_elements_in_fifo_order(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int vals[] = { 10, 20, 30, 40, 50 };
    for (int i = 0; i < 5; i++) dds_queue_enqueue(&q, &vals[i]);

    int out;
    dds_queue_dequeue(&q, &out); TEST_ASSERT_EQUAL_INT(10, out);
    dds_queue_dequeue(&q, &out); TEST_ASSERT_EQUAL_INT(20, out);
    dds_queue_dequeue(&q, &out); TEST_ASSERT_EQUAL_INT(30, out);
    dds_queue_dequeue(&q, &out); TEST_ASSERT_EQUAL_INT(40, out);
    dds_queue_dequeue(&q, &out); TEST_ASSERT_EQUAL_INT(50, out);
    dds_queue_free(&q);
}

void dds_queue_dequeue_should_leave_empty_queue_consistent(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);
    dds_queue_dequeue(&q, NULL);

    TEST_ASSERT_EQUAL_size_t(0, dds_queue_get_size(&q));
    TEST_ASSERT_TRUE(dds_queue_is_empty(&q));
    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, dds_queue_dequeue(&q, NULL));
    dds_queue_free(&q);
}

void dds_queue_dequeue_should_return_struct(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(test_record_t), dds_alloc_stdlib());
    test_record_t r = make_record(3, "world", 2.71);
    dds_queue_enqueue(&q, &r);

    test_record_t out;
    dds_queue_dequeue(&q, &out);
    TEST_ASSERT_TRUE(records_equal(&r, &out));
    dds_queue_free(&q);
}

/* ── dds_queue_peek_front ────────────────────────────────────────────────── */

void dds_queue_peek_front_should_return_ok(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);
    int out;
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_queue_peek_front(&q, &out));
    dds_queue_free(&q);
}

void dds_queue_peek_front_should_return_invalid_parameter_when_queue_null(void) {
    int out;
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_queue_peek_front(NULL, &out));
}

void dds_queue_peek_front_should_return_invalid_parameter_when_element_null(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_queue_peek_front(&q, NULL));
    dds_queue_free(&q);
}

void dds_queue_peek_front_should_return_out_of_range_when_empty(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int out;
    TEST_ASSERT_EQUAL_INT(DDS_OUT_OF_RANGE, dds_queue_peek_front(&q, &out));
    dds_queue_free(&q);
}

void dds_queue_peek_front_should_return_correct_element(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int a = 1, b = 2, c = 3;
    dds_queue_enqueue(&q, &a);
    dds_queue_enqueue(&q, &b);
    dds_queue_enqueue(&q, &c);

    int out;
    dds_queue_peek_front(&q, &out);
    TEST_ASSERT_EQUAL_INT(1, out);
    dds_queue_free(&q);
}

void dds_queue_peek_front_should_not_remove_element(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 42;
    dds_queue_enqueue(&q, &v);

    int out;
    dds_queue_peek_front(&q, &out);

    TEST_ASSERT_EQUAL_size_t(1, dds_queue_get_size(&q));
    dds_queue_free(&q);
}

void dds_queue_peek_front_should_return_same_value_as_subsequent_dequeue(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int a = 10, b = 20;
    dds_queue_enqueue(&q, &a);
    dds_queue_enqueue(&q, &b);

    int peeked, dequeued;
    dds_queue_peek_front(&q, &peeked);
    dds_queue_dequeue(&q, &dequeued);
    TEST_ASSERT_EQUAL_INT(peeked, dequeued);
    dds_queue_free(&q);
}

void dds_queue_peek_front_should_return_struct(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(test_record_t), dds_alloc_stdlib());
    test_record_t r = make_record(5, "peek", 1.0);
    dds_queue_enqueue(&q, &r);

    test_record_t out;
    dds_queue_peek_front(&q, &out);
    TEST_ASSERT_TRUE(records_equal(&r, &out));
    TEST_ASSERT_EQUAL_size_t(1, dds_queue_get_size(&q));
    dds_queue_free(&q);
}

/* ── dds_queue_get_size ──────────────────────────────────────────────────── */

void dds_queue_get_size_should_return_zero_when_queue_null(void) {
    TEST_ASSERT_EQUAL_size_t(0, dds_queue_get_size(NULL));
}

void dds_queue_get_size_should_return_zero_on_empty_queue(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_size_t(0, dds_queue_get_size(&q));
    dds_queue_free(&q);
}

void dds_queue_get_size_should_return_correct_size(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);
    dds_queue_enqueue(&q, &v);
    dds_queue_enqueue(&q, &v);
    TEST_ASSERT_EQUAL_size_t(3, dds_queue_get_size(&q));
    dds_queue_free(&q);
}

/* ── dds_queue_get_capacity ──────────────────────────────────────────────── */

void dds_queue_get_capacity_should_return_zero_when_queue_null(void) {
    TEST_ASSERT_EQUAL_size_t(0, dds_queue_get_capacity(NULL));
}

void dds_queue_get_capacity_should_return_zero_on_empty_queue(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_EQUAL_size_t(0, dds_queue_get_capacity(&q));
    dds_queue_free(&q);
}

void dds_queue_get_capacity_should_return_at_least_size(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    for (int i = 0; i < 10; i++) dds_queue_enqueue(&q, &v);
    TEST_ASSERT_GREATER_OR_EQUAL_size_t(dds_queue_get_size(&q), dds_queue_get_capacity(&q));
    dds_queue_free(&q);
}

/* ── dds_queue_is_empty ──────────────────────────────────────────────────── */

void dds_queue_is_empty_should_return_true_when_queue_null(void) {
    TEST_ASSERT_TRUE(dds_queue_is_empty(NULL));
}

void dds_queue_is_empty_should_return_true_on_empty_queue(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    TEST_ASSERT_TRUE(dds_queue_is_empty(&q));
    dds_queue_free(&q);
}

void dds_queue_is_empty_should_return_false_after_enqueue(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);
    TEST_ASSERT_FALSE(dds_queue_is_empty(&q));
    dds_queue_free(&q);
}

void dds_queue_is_empty_should_return_true_after_all_elements_dequeued(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 1;
    dds_queue_enqueue(&q, &v);
    dds_queue_enqueue(&q, &v);
    dds_queue_dequeue(&q, NULL);
    dds_queue_dequeue(&q, NULL);
    TEST_ASSERT_TRUE(dds_queue_is_empty(&q));
    dds_queue_free(&q);
}

/* ── corner cases ────────────────────────────────────────────────────────── */

void dds_queue_should_maintain_fifo_order_across_growth(void) {
    /* Push enough to trigger multiple growth events, verify FIFO ordering */
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());

    for (int i = 0; i < 64; i++) dds_queue_enqueue(&q, &i);

    for (int i = 0; i < 64; i++) {
        int out;
        dds_queue_dequeue(&q, &out);
        TEST_ASSERT_EQUAL_INT(i, out);
    }
    dds_queue_free(&q);
}

void dds_queue_should_maintain_fifo_order_after_wrap_around(void) {
    /*
     * Fill to capacity, dequeue half (head advances), then enqueue more
     * (tail wraps). Dequeue all and verify FIFO order through the wrap.
     */
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    dds_queue_reserve(&q, 8);

    for (int i = 0; i < 8; i++) dds_queue_enqueue(&q, &i);
    for (int i = 0; i < 4; i++) dds_queue_dequeue(&q, NULL);  /* head -> slot 4 */
    for (int i = 8; i < 12; i++) dds_queue_enqueue(&q, &i);   /* tail wraps */

    for (int expected = 4; expected < 12; expected++) {
        int out;
        TEST_ASSERT_EQUAL_INT(DDS_OK, dds_queue_dequeue(&q, &out));
        TEST_ASSERT_EQUAL_INT(expected, out);
    }
    dds_queue_free(&q);
}

void dds_queue_should_behave_correctly_with_interleaved_enqueue_dequeue(void) {
    /* Streaming pattern: enqueue one, dequeue one, repeat */
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());

    for (int i = 0; i < 32; i++) {
        dds_queue_enqueue(&q, &i);
        int out;
        dds_queue_dequeue(&q, &out);
        TEST_ASSERT_EQUAL_INT(i, out);
        TEST_ASSERT_TRUE(dds_queue_is_empty(&q));
    }
    dds_queue_free(&q);
}

void dds_queue_peek_front_then_dequeue_should_agree_through_wrap(void) {
    /* Verify peek and dequeue agree even after head has wrapped */
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    dds_queue_reserve(&q, 4);

    for (int i = 0; i < 4; i++) dds_queue_enqueue(&q, &i);
    for (int i = 0; i < 2; i++) dds_queue_dequeue(&q, NULL); /* head -> slot 2 */
    for (int i = 4; i < 6; i++) dds_queue_enqueue(&q, &i);   /* tail wraps */

    /* Queue now holds: 2, 3, 4, 5 (wrapped) */
    for (int expected = 2; expected < 6; expected++) {
        int peeked, dequeued;
        dds_queue_peek_front(&q, &peeked);
        dds_queue_dequeue(&q, &dequeued);
        TEST_ASSERT_EQUAL_INT(expected, peeked);
        TEST_ASSERT_EQUAL_INT(expected, dequeued);
    }
    dds_queue_free(&q);
}

void dds_queue_should_allow_enqueue_after_clear_and_preserve_fifo(void) {
    dds_queue_t q;
    dds_queue_init(&q, sizeof(int), dds_alloc_stdlib());
    int v = 999;
    dds_queue_enqueue(&q, &v);
    dds_queue_enqueue(&q, &v);
    dds_queue_clear(&q);

    int a = 1, b = 2, c = 3;
    dds_queue_enqueue(&q, &a);
    dds_queue_enqueue(&q, &b);
    dds_queue_enqueue(&q, &c);

    int out;
    dds_queue_dequeue(&q, &out); TEST_ASSERT_EQUAL_INT(1, out);
    dds_queue_dequeue(&q, &out); TEST_ASSERT_EQUAL_INT(2, out);
    dds_queue_dequeue(&q, &out); TEST_ASSERT_EQUAL_INT(3, out);
    TEST_ASSERT_TRUE(dds_queue_is_empty(&q));
    dds_queue_free(&q);
}
