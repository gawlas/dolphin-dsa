#include "dds/types.h"
#include "dds/hash_table_ref.h"

#include <string.h>

enum SLOT_STATE {
    EMPTY = 0,
    OCCUPIED = 1,
    DELETED = 2
};

static dds_result_t create_initial_data_buffer(dds_hash_table_ref_t *table) {
    void *keys = table->alloc.malloc(table->alloc.context, DDS_HASH_TABLE_REF_INITIAL_CAPACITY * sizeof(void *));
    if (keys == NULL) return DDS_OUT_OF_MEMORY;

    void *values = table->alloc.malloc(table->alloc.context, DDS_HASH_TABLE_REF_INITIAL_CAPACITY * sizeof(void *));
    if (values == NULL) {
        table->alloc.free(table->alloc.context, keys);
        return DDS_OUT_OF_MEMORY;
    }

    void *states = table->alloc.malloc(table->alloc.context, DDS_HASH_TABLE_REF_INITIAL_CAPACITY * sizeof(enum SLOT_STATE));
    if (states == NULL) {
        table->alloc.free(table->alloc.context, keys);
        table->alloc.free(table->alloc.context, values);
        return DDS_OUT_OF_MEMORY;
    }

    // zero-initialize the states array (EMPTY)
    memset(states, 0, DDS_HASH_TABLE_REF_INITIAL_CAPACITY * sizeof(enum SLOT_STATE));

    // update hash table struct
    table->keys = keys;
    table->values = values;
    table->states = states;
    table->capacity = DDS_HASH_TABLE_REF_INITIAL_CAPACITY;

    return DDS_OK;
}

static dds_result_t grow_hash_table(dds_hash_table_ref_t *table) {
    if (table->capacity > SIZE_MAX / DDS_HASH_TABLE_REF_GROWTH_FACTOR) return DDS_OVERFLOW;

    const size_t new_capacity = table->capacity * DDS_HASH_TABLE_REF_GROWTH_FACTOR;

    // allocate new buffers
    void *new_keys = table->alloc.malloc(table->alloc.context, new_capacity * sizeof(void *));
    if (new_keys == NULL) return DDS_OUT_OF_MEMORY;

    void *new_values = table->alloc.malloc(table->alloc.context, new_capacity * sizeof(void *));
    if (new_values == NULL) {
        table->alloc.free(table->alloc.context, new_keys);
        return DDS_OUT_OF_MEMORY;
    }

    void *new_states = table->alloc.malloc(table->alloc.context, new_capacity * sizeof(enum SLOT_STATE));
    if (new_states == NULL) {
        table->alloc.free(table->alloc.context, new_keys);
        table->alloc.free(table->alloc.context, new_values);

        return DDS_OUT_OF_MEMORY;
    }
    memset(new_states, 0, new_capacity * sizeof(enum SLOT_STATE));

    // calculate new hashes for all elements
    for (size_t index = 0; index < table->capacity; index++) {
        if (((enum SLOT_STATE *) table->states)[index] == OCCUPIED) {
            const void *key = ((const void **) table->keys)[index];
            const void *value = ((const void **) table->values)[index];
            const size_t hash = table->hash_fn(key);
            size_t slot_index = hash % new_capacity;

            // find an empty slot
            while (((enum SLOT_STATE *) new_states)[slot_index] == OCCUPIED) {
                slot_index = (slot_index + 1) % new_capacity;
            }

            // add an element to the hash table
            ((enum SLOT_STATE *) new_states)[slot_index] = OCCUPIED;
            ((const void **) new_keys)[slot_index] = key;
            ((const void **) new_values)[slot_index] = value;
        }
    }

    // free old buffers
    table->alloc.free(table->alloc.context, table->keys);
    table->alloc.free(table->alloc.context, table->values);
    table->alloc.free(table->alloc.context, table->states);

    // update hash table
    table->keys = new_keys;
    table->values = new_values;
    table->states = new_states;
    table->capacity = new_capacity;

    return DDS_OK;
}

dds_result_t dds_hash_table_ref_init(dds_hash_table_ref_t *table, dds_hash_fn_t hash, dds_key_equal_fn_t key_equal,
                                     dds_alloc_t alloc) {
    if (table == NULL) return DDS_INVALID_PARAMETER;
    if (hash == NULL) return DDS_INVALID_PARAMETER;
    if (key_equal == NULL) return DDS_INVALID_PARAMETER;
    if (alloc.malloc == NULL || alloc.free == NULL) return DDS_INVALID_PARAMETER;

    table->keys = NULL;
    table->values = NULL;
    table->states = NULL;
    table->size = 0;
    table->capacity = 0;
    table->hash_fn = hash;
    table->key_equal_fn = key_equal;
    table->alloc = alloc;

    return DDS_OK;
}

void dds_hash_table_ref_free(dds_hash_table_ref_t *table) {
    if (table == NULL) return;

    // free buffers
    table->alloc.free(table->alloc.context, table->keys);
    table->alloc.free(table->alloc.context, table->values);
    table->alloc.free(table->alloc.context, table->states);

    // clear struct
    table->keys = NULL;
    table->values = NULL;
    table->states = NULL;
    table->size = 0;
    table->capacity = 0;
    table->hash_fn = NULL;
    table->key_equal_fn = NULL;
    memset(&table->alloc, 0, sizeof(table->alloc));
}

dds_result_t dds_hash_table_ref_clear(dds_hash_table_ref_t *table) {
    if (table == NULL) return DDS_INVALID_PARAMETER;

    table->size = 0;

    if (table->states != NULL) {
        memset(((enum SLOT_STATE *) table->states), 0, table->capacity * sizeof(enum SLOT_STATE));
    }

    return DDS_OK;
}

dds_result_t dds_hash_table_ref_set(dds_hash_table_ref_t *table, const void *key, const void *value) {
    if (table == NULL) return DDS_INVALID_PARAMETER;
    if (key == NULL) return DDS_INVALID_PARAMETER;
    if (value == NULL) return DDS_INVALID_PARAMETER;

    // create an initial data buffer if necessary
    if (table->capacity == 0) {
        const dds_result_t result = create_initial_data_buffer(table);
        if (result != DDS_OK) return result;
    }

    // regrow if needed
    if (table->size * 4 >= table->capacity * 3) {
        const dds_result_t result = grow_hash_table(table);
        if (result != DDS_OK) return result;
    }

    // calculate hash and index based on the function provided by the user
    const size_t hash = table->hash_fn(key);
    const size_t initial_slot_index = hash % table->capacity;
    bool deleted_found = false;
    size_t deleted_index = 0;
    size_t slot_index = 0;

    // find an empty slot
    for (size_t index = 0; index < table->capacity; index++) {
        slot_index = (initial_slot_index + index) % table->capacity;

        // if key already in hash table
        if (((enum SLOT_STATE *) table->states)[slot_index] == OCCUPIED) {
            if (table->key_equal_fn(key, ((void **) table->keys)[slot_index]) == true) {
                // TODO: IMPORTANT
                ((const void **) table->keys)[slot_index] = key;
                ((const void **) table->values)[slot_index] = value;
                return DDS_OK;
            }
        }

        // if empty slot, stop searching
        if (((enum SLOT_STATE *) table->states)[slot_index] == EMPTY) break;

        // if deleted slot, save the first deleted index and continue searching
        if (((enum SLOT_STATE *) table->states)[slot_index] == DELETED && deleted_found == false) {
            deleted_found = true;
            deleted_index = slot_index;
        }
    }

    if (deleted_found) {
        slot_index = deleted_index;
    }

    // add an element to the hash table
    ((enum SLOT_STATE *) table->states)[slot_index] = OCCUPIED;
    ((const void **) table->keys)[slot_index] = key;
    ((const void **) table->values)[slot_index] = value;
    table->size++;

    return DDS_OK;
}

dds_result_t dds_hash_table_ref_get(const dds_hash_table_ref_t *table, const void *key, void *out_value) {
    if (table == NULL) return DDS_INVALID_PARAMETER;
    if (key == NULL) return DDS_INVALID_PARAMETER;
    if (table->size == 0) return DDS_NOT_FOUND;

    // calculate hash and index
    const size_t hash = table->hash_fn(key);
    size_t slot_index = hash % table->capacity;
    size_t index = 0;

    while (((enum SLOT_STATE *) table->states)[slot_index] != EMPTY && index < table->capacity) {
        if (((enum SLOT_STATE *) table->states)[slot_index] == OCCUPIED) {
            if (table->key_equal_fn(key, ((void **) table->keys)[slot_index]) == true) {
                if (out_value != NULL) {
                    ((void **) out_value)[0] = ((void **) table->values)[slot_index];
                }

                return DDS_OK;
            }
        }

        slot_index = (slot_index + 1) % table->capacity;
        index++;
    }

    return DDS_NOT_FOUND;
}

dds_result_t dds_hash_table_ref_remove(dds_hash_table_ref_t *table, const void *key, void *out_value) {
    if (table == NULL) return DDS_INVALID_PARAMETER;
    if (key == NULL) return DDS_INVALID_PARAMETER;
    if (table->size == 0) return DDS_NOT_FOUND;

    // calculate hash and index
    const size_t hash = table->hash_fn(key);
    size_t slot_index = hash % table->capacity;
    size_t index = 0;

    while (((enum SLOT_STATE *) table->states)[slot_index] != EMPTY && index < table->capacity) {
        if (((enum SLOT_STATE *) table->states)[slot_index] == OCCUPIED) {
            if (table->key_equal_fn(key, ((void **) table->keys)[slot_index]) == true) {
                if (out_value != NULL) {
                    ((void **) out_value)[0] = ((void **) table->values)[slot_index];
                }

                ((enum SLOT_STATE *) table->states)[slot_index] = DELETED;
                table->size--;

                return DDS_OK;
            }
        }

        slot_index = (slot_index + 1) % table->capacity;
        index++;
    }

    return DDS_NOT_FOUND;
}

bool dds_hash_table_ref_contains(const dds_hash_table_ref_t *table, const void *key) {
    return dds_hash_table_ref_get(table, key, NULL) == DDS_OK;
}

size_t dds_hash_table_ref_get_size(const dds_hash_table_ref_t *table) {
    if (table == NULL) return 0;

    return table->size;
}

size_t dds_hash_table_ref_get_capacity(const dds_hash_table_ref_t *table) {
    if (table == NULL) return 0;

    return table->capacity;
}

bool dds_hash_table_ref_is_empty(const dds_hash_table_ref_t *table) {
    if (table == NULL) return true;

    return table->size == 0;
}
