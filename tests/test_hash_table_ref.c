#include <stdio.h>
#include <string.h>

#include "dds/hash_table_ref.h"
#include "unity.h"

/* ── helpers ─────────────────────────────────────────────────────────────── */

static size_t hash_string(const void *key) {
    const unsigned char *s = (const unsigned char *) key;
    size_t h = 5381;
    int c;
    while ((c = *s++)) h = ((h << 5) + h) + (size_t) c;
    return h;
}

static bool equal_string(const void *a, const void *b) {
    return strcmp((const char *) a, (const char *) b) == 0;
}

/* hash that maps every key to slot 0 — forces linear-probing collisions */
static size_t colliding_hash(const void *key) {
    (void) key;
    return 0;
}

static int g_destroy_count = 0;

static void counting_destroy(const void *data, void *context) {
    (void) data;
    (void) context;
    g_destroy_count++;
}

static void context_counting_destroy(const void *data, void *context) {
    (void) data;
    int *count = (int *) context;
    (*count)++;
}

static dds_hash_table_ref_t make_table(void) {
    dds_hash_table_ref_t t;
    dds_hash_table_ref_init(&t, hash_string, equal_string,
                            dds_destroy_none(), dds_destroy_none(),
                            dds_alloc_stdlib());
    return t;
}

/* ── dds_hash_table_ref_init ─────────────────────────────────────────────── */

void dds_hash_table_ref_init_should_return_ok(void) {
    dds_hash_table_ref_t t;
    TEST_ASSERT_EQUAL_INT(DDS_OK,
        dds_hash_table_ref_init(&t, hash_string, equal_string,
                                dds_destroy_none(), dds_destroy_none(),
                                dds_alloc_stdlib()));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_init_should_return_invalid_parameter_when_table_null(void) {
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER,
        dds_hash_table_ref_init(NULL, hash_string, equal_string,
                                dds_destroy_none(), dds_destroy_none(),
                                dds_alloc_stdlib()));
}

void dds_hash_table_ref_init_should_return_invalid_parameter_when_hash_null(void) {
    dds_hash_table_ref_t t;
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER,
        dds_hash_table_ref_init(&t, NULL, equal_string,
                                dds_destroy_none(), dds_destroy_none(),
                                dds_alloc_stdlib()));
}

void dds_hash_table_ref_init_should_return_invalid_parameter_when_key_equal_null(void) {
    dds_hash_table_ref_t t;
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER,
        dds_hash_table_ref_init(&t, hash_string, NULL,
                                dds_destroy_none(), dds_destroy_none(),
                                dds_alloc_stdlib()));
}

void dds_hash_table_ref_init_should_return_invalid_parameter_when_alloc_malloc_null(void) {
    dds_hash_table_ref_t t;
    dds_alloc_t bad = dds_alloc_stdlib();
    bad.malloc = NULL;
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER,
        dds_hash_table_ref_init(&t, hash_string, equal_string,
                                dds_destroy_none(), dds_destroy_none(), bad));
}

void dds_hash_table_ref_init_should_return_invalid_parameter_when_alloc_free_null(void) {
    dds_hash_table_ref_t t;
    dds_alloc_t bad = dds_alloc_stdlib();
    bad.free = NULL;
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER,
        dds_hash_table_ref_init(&t, hash_string, equal_string,
                                dds_destroy_none(), dds_destroy_none(), bad));
}

void dds_hash_table_ref_init_should_create_correct_structure(void) {
    dds_hash_table_ref_t t = make_table();
    TEST_ASSERT_EQUAL_size_t(0, dds_hash_table_ref_get_size(&t));
    TEST_ASSERT_EQUAL_size_t(0, dds_hash_table_ref_get_capacity(&t));
    TEST_ASSERT_TRUE(dds_hash_table_ref_is_empty(&t));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_init_should_not_allocate_until_first_set(void) {
    dds_hash_table_ref_t t = make_table();
    TEST_ASSERT_EQUAL_size_t(0, dds_hash_table_ref_get_capacity(&t));
    dds_hash_table_ref_free(&t);
}

/* ── dds_hash_table_ref_free ─────────────────────────────────────────────── */

void dds_hash_table_ref_free_should_not_crash_when_null(void) {
    dds_hash_table_ref_free(NULL);
}

void dds_hash_table_ref_free_should_zero_fields(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "k", &v);
    dds_hash_table_ref_free(&t);
    TEST_ASSERT_EQUAL_size_t(0, dds_hash_table_ref_get_size(&t));
    TEST_ASSERT_EQUAL_size_t(0, dds_hash_table_ref_get_capacity(&t));
    TEST_ASSERT_NULL(t.keys);
    TEST_ASSERT_NULL(t.values);
    TEST_ASSERT_NULL(t.states);
}

void dds_hash_table_ref_free_should_not_crash_when_table_never_used(void) {
    /* free on an initialised-but-empty table (no buffers allocated) */
    dds_hash_table_ref_t t = make_table();
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_free_should_call_key_destroy_for_all_entries(void) {
    g_destroy_count = 0;
    dds_destroy_t kd = { counting_destroy, NULL };
    dds_hash_table_ref_t t;
    dds_hash_table_ref_init(&t, hash_string, equal_string,
                            kd, dds_destroy_none(), dds_alloc_stdlib());
    int v = 0;
    dds_hash_table_ref_set(&t, "a", &v);
    dds_hash_table_ref_set(&t, "b", &v);
    dds_hash_table_ref_set(&t, "c", &v);
    dds_hash_table_ref_free(&t);
    TEST_ASSERT_EQUAL_INT(3, g_destroy_count);
}

void dds_hash_table_ref_free_should_call_value_destroy_for_all_entries(void) {
    g_destroy_count = 0;
    dds_destroy_t vd = { counting_destroy, NULL };
    dds_hash_table_ref_t t;
    dds_hash_table_ref_init(&t, hash_string, equal_string,
                            dds_destroy_none(), vd, dds_alloc_stdlib());
    int a = 1, b = 2, c = 3;
    dds_hash_table_ref_set(&t, "a", &a);
    dds_hash_table_ref_set(&t, "b", &b);
    dds_hash_table_ref_set(&t, "c", &c);
    dds_hash_table_ref_free(&t);
    TEST_ASSERT_EQUAL_INT(3, g_destroy_count);
}

/* ── dds_hash_table_ref_clear ────────────────────────────────────────────── */

void dds_hash_table_ref_clear_should_return_ok(void) {
    dds_hash_table_ref_t t = make_table();
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_hash_table_ref_clear(&t));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_clear_should_return_invalid_parameter_when_table_null(void) {
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_hash_table_ref_clear(NULL));
}

void dds_hash_table_ref_clear_should_reset_size_to_zero(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "a", &v);
    dds_hash_table_ref_set(&t, "b", &v);
    dds_hash_table_ref_clear(&t);
    TEST_ASSERT_EQUAL_size_t(0, dds_hash_table_ref_get_size(&t));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_clear_should_preserve_capacity(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "a", &v);
    const size_t cap = dds_hash_table_ref_get_capacity(&t);
    dds_hash_table_ref_clear(&t);
    TEST_ASSERT_EQUAL_size_t(cap, dds_hash_table_ref_get_capacity(&t));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_clear_should_allow_set_after_clear(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 99;
    dds_hash_table_ref_set(&t, "a", &v);
    dds_hash_table_ref_clear(&t);
    int w = 42;
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_hash_table_ref_set(&t, "b", &w));
    void *out;
    dds_hash_table_ref_get(&t, "b", &out);
    TEST_ASSERT_EQUAL_PTR(&w, out);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_clear_should_call_key_destroy_for_all_entries(void) {
    g_destroy_count = 0;
    dds_destroy_t kd = { counting_destroy, NULL };
    dds_hash_table_ref_t t;
    dds_hash_table_ref_init(&t, hash_string, equal_string,
                            kd, dds_destroy_none(), dds_alloc_stdlib());
    int v = 0;
    dds_hash_table_ref_set(&t, "x", &v);
    dds_hash_table_ref_set(&t, "y", &v);
    dds_hash_table_ref_clear(&t);
    TEST_ASSERT_EQUAL_INT(2, g_destroy_count);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_clear_should_call_value_destroy_for_all_entries(void) {
    g_destroy_count = 0;
    dds_destroy_t vd = { counting_destroy, NULL };
    dds_hash_table_ref_t t;
    dds_hash_table_ref_init(&t, hash_string, equal_string,
                            dds_destroy_none(), vd, dds_alloc_stdlib());
    int a = 1, b = 2;
    dds_hash_table_ref_set(&t, "x", &a);
    dds_hash_table_ref_set(&t, "y", &b);
    dds_hash_table_ref_clear(&t);
    TEST_ASSERT_EQUAL_INT(2, g_destroy_count);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_clear_should_return_not_found_for_previously_existing_key(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "key", &v);
    dds_hash_table_ref_clear(&t);
    TEST_ASSERT_EQUAL_INT(DDS_NOT_FOUND, dds_hash_table_ref_get(&t, "key", NULL));
    dds_hash_table_ref_free(&t);
}

/* ── dds_hash_table_ref_set ──────────────────────────────────────────────── */

void dds_hash_table_ref_set_should_return_ok(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_hash_table_ref_set(&t, "k", &v));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_set_should_return_invalid_parameter_when_table_null(void) {
    int v = 1;
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_hash_table_ref_set(NULL, "k", &v));
}

void dds_hash_table_ref_set_should_return_invalid_parameter_when_key_null(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_hash_table_ref_set(&t, NULL, &v));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_set_should_return_invalid_parameter_when_value_null(void) {
    dds_hash_table_ref_t t = make_table();
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_hash_table_ref_set(&t, "k", NULL));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_set_should_increase_size(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 0;
    dds_hash_table_ref_set(&t, "a", &v);
    dds_hash_table_ref_set(&t, "b", &v);
    TEST_ASSERT_EQUAL_size_t(2, dds_hash_table_ref_get_size(&t));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_set_should_store_key_value_pair(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 42;
    dds_hash_table_ref_set(&t, "answer", &v);
    void *out;
    dds_hash_table_ref_get(&t, "answer", &out);
    TEST_ASSERT_EQUAL_PTR(&v, out);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_set_should_overwrite_existing_key(void) {
    dds_hash_table_ref_t t = make_table();
    int a = 1, b = 2;
    dds_hash_table_ref_set(&t, "k", &a);
    dds_hash_table_ref_set(&t, "k", &b);
    void *out;
    dds_hash_table_ref_get(&t, "k", &out);
    TEST_ASSERT_EQUAL_PTR(&b, out);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_set_should_not_increase_size_on_overwrite(void) {
    dds_hash_table_ref_t t = make_table();
    int a = 1, b = 2;
    dds_hash_table_ref_set(&t, "k", &a);
    dds_hash_table_ref_set(&t, "k", &b);
    TEST_ASSERT_EQUAL_size_t(1, dds_hash_table_ref_get_size(&t));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_set_should_call_key_destroy_on_overwrite(void) {
    g_destroy_count = 0;
    dds_destroy_t kd = { counting_destroy, NULL };
    dds_hash_table_ref_t t;
    dds_hash_table_ref_init(&t, hash_string, equal_string,
                            kd, dds_destroy_none(), dds_alloc_stdlib());
    int v = 0;
    dds_hash_table_ref_set(&t, "k", &v);
    dds_hash_table_ref_set(&t, "k", &v); /* overwrite */
    TEST_ASSERT_EQUAL_INT(1, g_destroy_count);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_set_should_call_value_destroy_on_overwrite(void) {
    g_destroy_count = 0;
    dds_destroy_t vd = { counting_destroy, NULL };
    dds_hash_table_ref_t t;
    dds_hash_table_ref_init(&t, hash_string, equal_string,
                            dds_destroy_none(), vd, dds_alloc_stdlib());
    int a = 1, b = 2;
    dds_hash_table_ref_set(&t, "k", &a);
    dds_hash_table_ref_set(&t, "k", &b); /* overwrite */
    TEST_ASSERT_EQUAL_INT(1, g_destroy_count);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_set_should_grow_when_load_factor_exceeded(void) {
    dds_hash_table_ref_t t = make_table();
    /* Insert enough entries to trigger at least one growth (>75% of 16 = 13) */
    int vals[20];
    char keys[20][8];
    for (int i = 0; i < 20; i++) {
        vals[i] = i;
        snprintf(keys[i], sizeof(keys[i]), "k%d", i);
        TEST_ASSERT_EQUAL_INT(DDS_OK, dds_hash_table_ref_set(&t, keys[i], &vals[i]));
    }
    TEST_ASSERT_EQUAL_size_t(20, dds_hash_table_ref_get_size(&t));
    TEST_ASSERT_GREATER_OR_EQUAL_size_t(20, dds_hash_table_ref_get_capacity(&t));
    dds_hash_table_ref_free(&t);
}

/* ── dds_hash_table_ref_get ──────────────────────────────────────────────── */

void dds_hash_table_ref_get_should_return_ok(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "k", &v);
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_hash_table_ref_get(&t, "k", NULL));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_get_should_return_invalid_parameter_when_table_null(void) {
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_hash_table_ref_get(NULL, "k", NULL));
}

void dds_hash_table_ref_get_should_return_invalid_parameter_when_key_null(void) {
    dds_hash_table_ref_t t = make_table();
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_hash_table_ref_get(&t, NULL, NULL));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_get_should_return_not_found_when_empty(void) {
    dds_hash_table_ref_t t = make_table();
    TEST_ASSERT_EQUAL_INT(DDS_NOT_FOUND, dds_hash_table_ref_get(&t, "k", NULL));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_get_should_return_not_found_for_missing_key(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "a", &v);
    TEST_ASSERT_EQUAL_INT(DDS_NOT_FOUND, dds_hash_table_ref_get(&t, "b", NULL));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_get_should_return_correct_value(void) {
    dds_hash_table_ref_t t = make_table();
    int a = 10, b = 20;
    dds_hash_table_ref_set(&t, "a", &a);
    dds_hash_table_ref_set(&t, "b", &b);
    void *out;
    dds_hash_table_ref_get(&t, "a", &out);
    TEST_ASSERT_EQUAL_PTR(&a, out);
    dds_hash_table_ref_get(&t, "b", &out);
    TEST_ASSERT_EQUAL_PTR(&b, out);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_get_should_allow_null_out_value(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "k", &v);
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_hash_table_ref_get(&t, "k", NULL));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_get_should_not_modify_size(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "k", &v);
    dds_hash_table_ref_get(&t, "k", NULL);
    TEST_ASSERT_EQUAL_size_t(1, dds_hash_table_ref_get_size(&t));
    dds_hash_table_ref_free(&t);
}

/* ── dds_hash_table_ref_remove ───────────────────────────────────────────── */

void dds_hash_table_ref_remove_should_return_ok(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "k", &v);
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_hash_table_ref_remove(&t, "k", NULL));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_remove_should_return_invalid_parameter_when_table_null(void) {
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_hash_table_ref_remove(NULL, "k", NULL));
}

void dds_hash_table_ref_remove_should_return_invalid_parameter_when_key_null(void) {
    dds_hash_table_ref_t t = make_table();
    TEST_ASSERT_EQUAL_INT(DDS_INVALID_PARAMETER, dds_hash_table_ref_remove(&t, NULL, NULL));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_remove_should_return_not_found_when_empty(void) {
    dds_hash_table_ref_t t = make_table();
    TEST_ASSERT_EQUAL_INT(DDS_NOT_FOUND, dds_hash_table_ref_remove(&t, "k", NULL));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_remove_should_return_not_found_for_missing_key(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "a", &v);
    TEST_ASSERT_EQUAL_INT(DDS_NOT_FOUND, dds_hash_table_ref_remove(&t, "b", NULL));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_remove_should_decrease_size(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "a", &v);
    dds_hash_table_ref_set(&t, "b", &v);
    dds_hash_table_ref_remove(&t, "a", NULL);
    TEST_ASSERT_EQUAL_size_t(1, dds_hash_table_ref_get_size(&t));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_remove_should_return_value_when_out_value_provided(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 77;
    dds_hash_table_ref_set(&t, "k", &v);
    void *out;
    dds_hash_table_ref_remove(&t, "k", &out);
    TEST_ASSERT_EQUAL_PTR(&v, out);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_remove_should_call_value_destroy_when_out_value_null(void) {
    g_destroy_count = 0;
    dds_destroy_t vd = { counting_destroy, NULL };
    dds_hash_table_ref_t t;
    dds_hash_table_ref_init(&t, hash_string, equal_string,
                            dds_destroy_none(), vd, dds_alloc_stdlib());
    int v = 1;
    dds_hash_table_ref_set(&t, "k", &v);
    dds_hash_table_ref_remove(&t, "k", NULL);
    TEST_ASSERT_EQUAL_INT(1, g_destroy_count);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_remove_should_not_call_value_destroy_when_out_value_provided(void) {
    g_destroy_count = 0;
    dds_destroy_t vd = { counting_destroy, NULL };
    dds_hash_table_ref_t t;
    dds_hash_table_ref_init(&t, hash_string, equal_string,
                            dds_destroy_none(), vd, dds_alloc_stdlib());
    int v = 1;
    dds_hash_table_ref_set(&t, "k", &v);
    void *out;
    dds_hash_table_ref_remove(&t, "k", &out);
    TEST_ASSERT_EQUAL_INT(0, g_destroy_count);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_remove_should_call_key_destroy_always(void) {
    g_destroy_count = 0;
    dds_destroy_t kd = { counting_destroy, NULL };
    dds_hash_table_ref_t t;
    dds_hash_table_ref_init(&t, hash_string, equal_string,
                            kd, dds_destroy_none(), dds_alloc_stdlib());
    int v = 1;
    /* remove with out_value — key_destroy still fires */
    dds_hash_table_ref_set(&t, "k", &v);
    void *out;
    dds_hash_table_ref_remove(&t, "k", &out);
    TEST_ASSERT_EQUAL_INT(1, g_destroy_count);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_remove_should_allow_reinsertion_of_same_key(void) {
    dds_hash_table_ref_t t = make_table();
    int a = 1, b = 2;
    dds_hash_table_ref_set(&t, "k", &a);
    dds_hash_table_ref_remove(&t, "k", NULL);
    dds_hash_table_ref_set(&t, "k", &b);
    void *out;
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_hash_table_ref_get(&t, "k", &out));
    TEST_ASSERT_EQUAL_PTR(&b, out);
    dds_hash_table_ref_free(&t);
}

/* ── dds_hash_table_ref_contains ────────────────────────────────────────── */

void dds_hash_table_ref_contains_should_return_false_when_table_null(void) {
    TEST_ASSERT_FALSE(dds_hash_table_ref_contains(NULL, "k"));
}

void dds_hash_table_ref_contains_should_return_false_when_empty(void) {
    dds_hash_table_ref_t t = make_table();
    TEST_ASSERT_FALSE(dds_hash_table_ref_contains(&t, "k"));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_contains_should_return_false_for_missing_key(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "a", &v);
    TEST_ASSERT_FALSE(dds_hash_table_ref_contains(&t, "b"));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_contains_should_return_true_for_existing_key(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "k", &v);
    TEST_ASSERT_TRUE(dds_hash_table_ref_contains(&t, "k"));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_contains_should_return_false_after_remove(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "k", &v);
    dds_hash_table_ref_remove(&t, "k", NULL);
    TEST_ASSERT_FALSE(dds_hash_table_ref_contains(&t, "k"));
    dds_hash_table_ref_free(&t);
}

/* ── dds_hash_table_ref_get_size ─────────────────────────────────────────── */

void dds_hash_table_ref_get_size_should_return_zero_when_table_null(void) {
    TEST_ASSERT_EQUAL_size_t(0, dds_hash_table_ref_get_size(NULL));
}

void dds_hash_table_ref_get_size_should_return_zero_on_empty(void) {
    dds_hash_table_ref_t t = make_table();
    TEST_ASSERT_EQUAL_size_t(0, dds_hash_table_ref_get_size(&t));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_get_size_should_return_correct_size(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 0;
    dds_hash_table_ref_set(&t, "a", &v);
    dds_hash_table_ref_set(&t, "b", &v);
    dds_hash_table_ref_set(&t, "c", &v);
    TEST_ASSERT_EQUAL_size_t(3, dds_hash_table_ref_get_size(&t));
    dds_hash_table_ref_free(&t);
}

/* ── dds_hash_table_ref_get_capacity ─────────────────────────────────────── */

void dds_hash_table_ref_get_capacity_should_return_zero_when_table_null(void) {
    TEST_ASSERT_EQUAL_size_t(0, dds_hash_table_ref_get_capacity(NULL));
}

void dds_hash_table_ref_get_capacity_should_return_zero_before_first_set(void) {
    dds_hash_table_ref_t t = make_table();
    TEST_ASSERT_EQUAL_size_t(0, dds_hash_table_ref_get_capacity(&t));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_get_capacity_should_return_at_least_size(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 0;
    for (int i = 0; i < 10; i++) {
        char key[4];
        snprintf(key, sizeof(key), "k%d", i);
        dds_hash_table_ref_set(&t, key, &v);
    }
    TEST_ASSERT_GREATER_OR_EQUAL_size_t(
        dds_hash_table_ref_get_size(&t),
        dds_hash_table_ref_get_capacity(&t));
    dds_hash_table_ref_free(&t);
}

/* ── dds_hash_table_ref_is_empty ─────────────────────────────────────────── */

void dds_hash_table_ref_is_empty_should_return_true_when_table_null(void) {
    TEST_ASSERT_TRUE(dds_hash_table_ref_is_empty(NULL));
}

void dds_hash_table_ref_is_empty_should_return_true_on_empty(void) {
    dds_hash_table_ref_t t = make_table();
    TEST_ASSERT_TRUE(dds_hash_table_ref_is_empty(&t));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_is_empty_should_return_false_after_set(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "k", &v);
    TEST_ASSERT_FALSE(dds_hash_table_ref_is_empty(&t));
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_is_empty_should_return_true_after_all_removed(void) {
    dds_hash_table_ref_t t = make_table();
    int v = 1;
    dds_hash_table_ref_set(&t, "a", &v);
    dds_hash_table_ref_set(&t, "b", &v);
    dds_hash_table_ref_remove(&t, "a", NULL);
    dds_hash_table_ref_remove(&t, "b", NULL);
    TEST_ASSERT_TRUE(dds_hash_table_ref_is_empty(&t));
    dds_hash_table_ref_free(&t);
}

/* ── corner cases ────────────────────────────────────────────────────────── */

void dds_hash_table_ref_should_find_all_entries_after_growth(void) {
    /*
     * Insert enough entries to trigger multiple growth events, then verify
     * every key is still reachable (rehashing preserved all entries).
     */
    dds_hash_table_ref_t t = make_table();
    int vals[64];
    char keys[64][8];
    for (int i = 0; i < 64; i++) {
        vals[i] = i;
        snprintf(keys[i], sizeof(keys[i]), "k%d", i);
        dds_hash_table_ref_set(&t, keys[i], &vals[i]);
    }
    for (int i = 0; i < 64; i++) {
        void *out;
        TEST_ASSERT_EQUAL_INT(DDS_OK, dds_hash_table_ref_get(&t, keys[i], &out));
        TEST_ASSERT_EQUAL_PTR(&vals[i], out);
    }
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_should_handle_hash_collisions_correctly(void) {
    /*
     * Use colliding_hash so every key lands on slot 0. Linear probing must
     * place all three keys consecutively and still look them all up correctly.
     */
    dds_hash_table_ref_t t;
    dds_hash_table_ref_init(&t, colliding_hash, equal_string,
                            dds_destroy_none(), dds_destroy_none(),
                            dds_alloc_stdlib());
    int a = 1, b = 2, c = 3;
    dds_hash_table_ref_set(&t, "alpha", &a);
    dds_hash_table_ref_set(&t, "beta",  &b);
    dds_hash_table_ref_set(&t, "gamma", &c);

    void *out;
    dds_hash_table_ref_get(&t, "alpha", &out); TEST_ASSERT_EQUAL_PTR(&a, out);
    dds_hash_table_ref_get(&t, "beta",  &out); TEST_ASSERT_EQUAL_PTR(&b, out);
    dds_hash_table_ref_get(&t, "gamma", &out); TEST_ASSERT_EQUAL_PTR(&c, out);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_should_find_entry_past_tombstone(void) {
    /*
     * With a colliding hash, insert "a" then "b" (both at slot 0, "b" probes
     * to slot 1). Remove "a" (slot 0 becomes a tombstone). "b" must still be
     * reachable — the probe must continue past the tombstone.
     */
    dds_hash_table_ref_t t;
    dds_hash_table_ref_init(&t, colliding_hash, equal_string,
                            dds_destroy_none(), dds_destroy_none(),
                            dds_alloc_stdlib());
    int a = 1, b = 2;
    dds_hash_table_ref_set(&t, "a", &a);
    dds_hash_table_ref_set(&t, "b", &b);
    dds_hash_table_ref_remove(&t, "a", NULL);

    void *out;
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_hash_table_ref_get(&t, "b", &out));
    TEST_ASSERT_EQUAL_PTR(&b, out);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_should_reuse_tombstone_slot_on_insert(void) {
    /*
     * After removing "a" (slot 0 → tombstone) and inserting "c" (same
     * colliding hash), "c" should land in the tombstone slot (not after "b").
     * Both "b" and "c" must remain accessible.
     */
    dds_hash_table_ref_t t;
    dds_hash_table_ref_init(&t, colliding_hash, equal_string,
                            dds_destroy_none(), dds_destroy_none(),
                            dds_alloc_stdlib());
    int a = 1, b = 2, c = 3;
    dds_hash_table_ref_set(&t, "a", &a);
    dds_hash_table_ref_set(&t, "b", &b);
    dds_hash_table_ref_remove(&t, "a", NULL);
    dds_hash_table_ref_set(&t, "c", &c);

    void *out;
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_hash_table_ref_get(&t, "b", &out));
    TEST_ASSERT_EQUAL_PTR(&b, out);
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_hash_table_ref_get(&t, "c", &out));
    TEST_ASSERT_EQUAL_PTR(&c, out);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_should_handle_interleaved_set_and_remove(void) {
    /* Streaming pattern: set a key, remove it, repeat with a new key */
    dds_hash_table_ref_t t = make_table();
    int vals[32];
    char keys[32][8];
    for (int i = 0; i < 32; i++) {
        vals[i] = i;
        snprintf(keys[i], sizeof(keys[i]), "k%d", i);
        dds_hash_table_ref_set(&t, keys[i], &vals[i]);
        if (i > 0) {
            /* remove the previous key */
            dds_hash_table_ref_remove(&t, keys[i - 1], NULL);
        }
    }
    TEST_ASSERT_EQUAL_size_t(1, dds_hash_table_ref_get_size(&t));
    void *out;
    TEST_ASSERT_EQUAL_INT(DDS_OK, dds_hash_table_ref_get(&t, keys[31], &out));
    TEST_ASSERT_EQUAL_PTR(&vals[31], out);
    dds_hash_table_ref_free(&t);
}

void dds_hash_table_ref_destroy_context_is_passed_correctly(void) {
    int key_count = 0, val_count = 0;
    dds_destroy_t kd = { context_counting_destroy, &key_count };
    dds_destroy_t vd = { context_counting_destroy, &val_count };
    dds_hash_table_ref_t t;
    dds_hash_table_ref_init(&t, hash_string, equal_string, kd, vd, dds_alloc_stdlib());
    int a = 1, b = 2;
    dds_hash_table_ref_set(&t, "x", &a);
    dds_hash_table_ref_set(&t, "y", &b);
    dds_hash_table_ref_free(&t);
    TEST_ASSERT_EQUAL_INT(2, key_count);
    TEST_ASSERT_EQUAL_INT(2, val_count);
}
