#ifndef DOLPHIN_DSA_TYPES_H
#define DOLPHIN_DSA_TYPES_H

typedef enum dds_result {
    DDS_OK = 0,
    DDS_ERROR = 1,
    DDS_INVALID_PARAMETER = 2,
    DDS_OUT_OF_MEMORY = 3,
    DDS_OUT_OF_RANGE = 4,
    DDS_OVERFLOW = 5
} dds_result_t;

#endif //DOLPHIN_DSA_TYPES_H
