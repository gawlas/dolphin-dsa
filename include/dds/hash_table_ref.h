#ifndef DOLPHIN_DSA_HASH_TABLE_REF_H
#define DOLPHIN_DSA_HASH_TABLE_REF_H

#include <stdbool.h>
#include <stdint.h>

#include "alloc.h"
#include "types.h"

#define DDS_HASH_TABLE_REF_INITIAL_CAPACITY 16
#define DDS_HASH_TABLE_REF_GROWTH_FACTOR 2

typedef size_t (*dds_hash_fn_t)(const void *key);

typedef bool (*dds_key_equal_fn_t)(const void *a, const void *b);

typedef struct dds_hash_table_ref {
    void *keys;
    void *values;
    void *states;
    size_t size;
    size_t capacity;
    dds_hash_fn_t hash_fn;
    dds_key_equal_fn_t key_equal_fn;
    dds_alloc_t alloc;
} dds_hash_table_ref_t;


dds_result_t dds_hash_table_ref_init(dds_hash_table_ref_t *table, dds_hash_fn_t hash, dds_key_equal_fn_t key_equal,
                                     dds_alloc_t alloc);

void dds_hash_table_ref_free(dds_hash_table_ref_t *table);

dds_result_t dds_hash_table_ref_clear(dds_hash_table_ref_t *table);

dds_result_t dds_hash_table_ref_set(dds_hash_table_ref_t *table, const void *key, const void *value);

dds_result_t dds_hash_table_ref_get(const dds_hash_table_ref_t *table, const void *key, void *out_value);

dds_result_t dds_hash_table_ref_remove(dds_hash_table_ref_t *table, const void *key, void *out_value);

bool dds_hash_table_ref_contains(const dds_hash_table_ref_t *table, const void *key);

size_t dds_hash_table_ref_get_size(const dds_hash_table_ref_t *table);

size_t dds_hash_table_ref_get_capacity(const dds_hash_table_ref_t *table);

bool dds_hash_table_ref_is_empty(const dds_hash_table_ref_t *table);

#endif //DOLPHIN_DSA_HASH_TABLE_REF_H
