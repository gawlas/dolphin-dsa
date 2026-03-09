#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "dds/hash_table.h"

size_t hash_string_key(const void *key) {
    const unsigned char *str = (const unsigned char *)key;
    size_t hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c

    return hash;
}

bool equal_string_key(const void *key_a, const void *key_b) {
    const char *a = (const char *)key_a;
    const char *b = (const char *)key_b;

    bool result = strcmp(a, b) == 0;
    return result;
}

typedef struct person {
    char *name;
    int age;
} person_t;

void print_person(const person_t *person) {
    printf("Name: %s, Age: %d\n", person->name, person->age);
    fflush(stdout);
}

int main(void) {
    dds_hash_table_t hash_table;

    dds_hash_table_init(&hash_table, hash_string_key, equal_string_key, dds_alloc_stdlib());

    person_t alice;
    alice.name = "Alice";
    alice.age = 30;

    person_t bob;
    bob.name = "Bob";
    bob.age = 25;

    person_t stefan;
    stefan.name = "Stefan";
    stefan.age = 14;

    dds_hash_table_set(&hash_table, "alice", &alice);
    dds_hash_table_set(&hash_table, "bob", &bob);

    person_t *out;
    dds_result_t result;

    result = dds_hash_table_get(&hash_table, "carry", &out);
    if (result == DDS_OK) {
        print_person(out);
    }

    result = dds_hash_table_get(&hash_table, "alice", &out);
    if (result == DDS_OK) {
        print_person(out);
    }

    result = dds_hash_table_get(&hash_table, "bob", &out);
    if (result == DDS_OK) {
        print_person(out);
        out->age = 26;
    }

    result = dds_hash_table_get(&hash_table, "bob", &out);
    if (result == DDS_OK) {
        print_person(out);
    }

    dds_hash_table_set(&hash_table, "bob", &stefan);

    result = dds_hash_table_get(&hash_table, "bob", &out);
    if (result == DDS_OK) {
        print_person(out);
    }
}
