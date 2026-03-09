#ifndef DOLPHIN_DSA_HASH_TABLE_REF_H
#define DOLPHIN_DSA_HASH_TABLE_REF_H

#include <stdbool.h>
#include <stdint.h>

#include "alloc.h"
#include "types.h"

#define DDS_HASH_TABLE_REF_INITIAL_CAPACITY 16
#define DDS_HASH_TABLE_REF_GROWTH_FACTOR 2

/**
 * Hash function type.
 *
 * Maps a key to a non-negative integer. Keys considered equal by
 * dds_key_equal_fn_t must produce the same hash value.
 *
 * @param key Pointer to the key. Never NULL.
 * @return Hash value for the key.
 */
typedef size_t (*dds_hash_fn_t)(const void *key);

/**
 * Key equality function type.
 *
 * Returns true if the two keys are considered equal.
 *
 * @param a Pointer to the first key. Never NULL.
 * @param b Pointer to the second key. Never NULL.
 * @return true if the keys are equal, false otherwise.
 */
typedef bool (*dds_key_equal_fn_t)(const void *a, const void *b);

/**
 * Hash table storing pointer-based key-value pairs.
 *
 * Uses open addressing with linear probing and tombstone deletion.
 * Grows automatically at 75% load factor.
 *
 * Keys and values are stored as pointers — the table does not copy or
 * own the data. The caller is responsible for the lifetime of all
 * keys and values.
 *
 * Must be initialized with dds_hash_table_ref_init() before use
 * and released with dds_hash_table_ref_free() when no longer needed.
 *
 * @note Not thread-safe. External synchronization is required
 *       when a table instance is shared across threads.
 */
typedef struct dds_hash_table_ref {
    void *keys;                    /**< Flat array of key pointers. */
    void *values;                  /**< Flat array of value pointers. */
    void *states;                  /**< Flat array of slot states (EMPTY/OCCUPIED/DELETED). */
    size_t size;                   /**< Number of key-value pairs currently stored. */
    size_t capacity;               /**< Total number of slots allocated. */
    dds_hash_fn_t hash_fn;           /**< Hash function provided at initialization. */
    dds_key_equal_fn_t key_equal_fn; /**< Key equality function provided at initialization. */
    dds_destroy_t key_destroy;   /**< Called when a key is removed or overwritten. */
    dds_destroy_t value_destroy; /**< Called when a value is removed or overwritten. */
    dds_alloc_t alloc;               /**< Allocator used for all memory operations. */
} dds_hash_table_ref_t;

/**
 * Initialize a hash table.
 *
 * Must be called before any other operation on the table.
 *
 * @param table         Pointer to an uninitialized hash table.
 * @param hash          Hash function; must not be NULL.
 * @param key_equal     Key equality function; must not be NULL.
 * @param key_destroy   Destroy struct for keys; use dds_destroy_none() for no-op.
 * @param value_destroy Destroy struct for values; use dds_destroy_none() for no-op.
 * @param alloc         Allocator to use; malloc and free must not be NULL.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if any argument is NULL.
 */
dds_result_t dds_hash_table_ref_init(dds_hash_table_ref_t *table, dds_hash_fn_t hash, dds_key_equal_fn_t key_equal,
                                     dds_destroy_t key_destroy, dds_destroy_t value_destroy,
                                     dds_alloc_t alloc);

/**
 * Free all resources held by a hash table.
 *
 * Frees internal buffers and leaves the table in a zeroed state.
 * Safe to call with NULL. Does not free the keys or values themselves —
 * the caller owns that memory.
 *
 * @param table Pointer to the hash table to free.
 */
void dds_hash_table_ref_free(dds_hash_table_ref_t *table);

/**
 * Remove all entries from the table without freeing internal buffers.
 *
 * Resets the table to an empty state while preserving allocated capacity,
 * allowing the table to be reused without re-initialization.
 * Does not free the keys or values themselves.
 *
 * @param table Pointer to an initialized hash table.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if table is NULL.
 */
dds_result_t dds_hash_table_ref_clear(dds_hash_table_ref_t *table);

/**
 * Insert or update a key-value pair.
 *
 * If the key already exists, its associated value pointer is replaced.
 * Both the key and value are stored as pointers; the table does not copy
 * the data. The caller must ensure the pointed-to data remains valid for
 * the lifetime of the entry.
 *
 * @param table Pointer to an initialized hash table.
 * @param key   Pointer to the key; must not be NULL.
 * @param value Pointer to the value; must not be NULL.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if any argument is NULL,
 *         DDS_OUT_OF_MEMORY if allocation fails during growth,
 *         DDS_OVERFLOW if the table capacity cannot grow further.
 */
dds_result_t dds_hash_table_ref_set(dds_hash_table_ref_t *table, const void *key, const void *value);

/**
 * Look up a value by key.
 *
 * @param table     Pointer to an initialized hash table.
 * @param key       Pointer to the key to look up; must not be NULL.
 * @param out_value Pointer to a pointer that will receive the stored value,
 *                  or NULL to only check existence.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if table or key is NULL,
 *         DDS_NOT_FOUND if the key does not exist.
 */
dds_result_t dds_hash_table_ref_get(const dds_hash_table_ref_t *table, const void *key, void *out_value);

/**
 * Remove a key-value pair from the table.
 *
 * @param table     Pointer to an initialized hash table.
 * @param key       Pointer to the key to remove; must not be NULL.
 * @param out_value Pointer to a pointer that will receive the removed value,
 *                  or NULL to discard. When non-NULL, value_destroy_fn is NOT
 *                  called — the caller takes ownership of the value.
 * @return DDS_OK on success, DDS_INVALID_PARAMETER if table or key is NULL,
 *         DDS_NOT_FOUND if the key does not exist.
 */
dds_result_t dds_hash_table_ref_remove(dds_hash_table_ref_t *table, const void *key, void *out_value);

/**
 * Check whether a key exists in the table.
 *
 * @param table Pointer to an initialized hash table.
 * @param key   Pointer to the key to check; must not be NULL.
 * @return true if the key exists, false otherwise or if table is NULL.
 */
bool dds_hash_table_ref_contains(const dds_hash_table_ref_t *table, const void *key);

/**
 * Return the number of key-value pairs currently stored.
 *
 * @param table Pointer to an initialized hash table.
 * @return Number of entries, or 0 if table is NULL.
 */
size_t dds_hash_table_ref_get_size(const dds_hash_table_ref_t *table);

/**
 * Return the total number of slots currently allocated.
 *
 * @param table Pointer to an initialized hash table.
 * @return Allocated capacity, or 0 if table is NULL.
 */
size_t dds_hash_table_ref_get_capacity(const dds_hash_table_ref_t *table);

/**
 * Check whether the table contains no entries.
 *
 * @param table Pointer to an initialized hash table.
 * @return true if the table is empty or NULL, false otherwise.
 */
bool dds_hash_table_ref_is_empty(const dds_hash_table_ref_t *table);

#endif //DOLPHIN_DSA_HASH_TABLE_REF_H
