#ifndef DOLPHIN_DSA_TEST_STRUCTS_H
#define DOLPHIN_DSA_TEST_STRUCTS_H

#include <string.h>

typedef struct {
    int      id;
    char     name[64];
    double   value;
    int      tags[8];
} test_record_t;

static test_record_t make_record(int id, const char* name, double value) {
    test_record_t r;
    r.id = id;
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.name[sizeof(r.name) - 1] = '\0';
    r.value = value;
    for (int i = 0; i < 8; i++) r.tags[i] = id * 10 + i;
    return r;
}

static int records_equal(const test_record_t* a, const test_record_t* b) {
    return a->id == b->id
        && strncmp(a->name, b->name, sizeof(a->name)) == 0
        && a->value == b->value
        && memcmp(a->tags, b->tags, sizeof(a->tags)) == 0;
}

#endif /* DOLPHIN_DSA_TEST_STRUCTS_H */
