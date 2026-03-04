#include <stdint.h>
#include <string.h>
#include "dds/linked_list.h"
#include "dds/types.h"

dds_result_t dds_linked_list_init(dds_linked_list_t* linked_list, const size_t element_size, const dds_alloc_t alloc) {
    if (linked_list == NULL) return DDS_INVALID_PARAMETER;
    if (element_size == 0) return DDS_INVALID_PARAMETER;
    if (alloc.malloc == NULL || alloc.free == NULL) return DDS_INVALID_PARAMETER;

    linked_list->head = NULL;
    linked_list->tail = NULL;
    linked_list->size = 0;
    linked_list->element_size = element_size;
    linked_list->alloc = alloc;

    return DDS_OK;
}

dds_result_t dds_linked_list_clear(dds_linked_list_t* linked_list) {
    if (linked_list == NULL) return DDS_INVALID_PARAMETER;

    dds_linked_list_node_t* current_node = linked_list->head;

    while (current_node != NULL) {
        dds_linked_list_node_t* next_node = current_node->next;
        linked_list->alloc.free(linked_list->alloc.context, current_node);
        current_node = next_node;
    }

    linked_list->head = NULL;
    linked_list->tail = NULL;
    linked_list->size = 0;

    return DDS_OK;
}

void dds_linked_list_free(dds_linked_list_t* linked_list) {
    if (linked_list == NULL) return;

    dds_linked_list_clear(linked_list);

    linked_list->element_size = 0;
    memset(&linked_list->alloc, 0, sizeof(linked_list->alloc));
}


dds_result_t dds_linked_list_push_back(dds_linked_list_t* list, const void* element) {
    if (list == NULL) return DDS_INVALID_PARAMETER;
    if (element == NULL) return DDS_INVALID_PARAMETER;

    dds_linked_list_node_t* new_node = list->alloc.malloc(list->alloc.context, sizeof(dds_linked_list_node_t) + list->element_size);
    if (new_node == NULL) return DDS_OUT_OF_MEMORY;

    memcpy(new_node->data, element, list->element_size);

    if (list->tail == NULL) {
        list->head = new_node;
        list->tail = new_node;
        new_node->next = NULL;
        new_node->prev = NULL;
    } else {
        list->tail->next = new_node;
        new_node->prev = list->tail;
        new_node->next = NULL;
        list->tail = new_node;
    }

    list->size++;

    return DDS_OK;
}
dds_result_t dds_linked_list_push_front(dds_linked_list_t* list, const void* element) {
    if (list == NULL) return DDS_INVALID_PARAMETER;
    if (element == NULL) return DDS_INVALID_PARAMETER;

    dds_linked_list_node_t* new_node = list->alloc.malloc(list->alloc.context, sizeof(dds_linked_list_node_t) + list->element_size);
    if (new_node == NULL) return DDS_OUT_OF_MEMORY;

    memcpy(new_node->data, element, list->element_size);

    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
        new_node->next = NULL;
        new_node->prev = NULL;
    } else {
        list->head->prev = new_node;
        new_node->prev = NULL;
        new_node->next = list->head;
        list->head = new_node;
    }

    list->size++;

    return DDS_OK;
}


dds_result_t dds_linked_list_pop_back(dds_linked_list_t* list, void* out_element) {
    if (list == NULL) return DDS_INVALID_PARAMETER;
    if (list->tail == NULL) return DDS_OUT_OF_RANGE;

    // if the element is not NULL, copy the last element from the list to it, otherwise discard
    if (out_element != NULL) {
        memcpy(out_element, list->tail->data, list->element_size);
    }

    dds_linked_list_node_t* prev = list->tail->prev;
    list->alloc.free(list->alloc.context, list->tail);
    list->tail = prev;

    if (list->tail == NULL) {
        list->head = NULL;
    } else {
        list->tail->next = NULL;
    }

    list->size--;

    return DDS_OK;
}

dds_result_t dds_linked_list_pop_front(dds_linked_list_t* list, void* out_element) {
    if (list == NULL) return DDS_INVALID_PARAMETER;
    if (list->head == NULL) return DDS_OUT_OF_RANGE;

    // if the element is not NULL, copy the first element from the list to it, otherwise discard
    if (out_element != NULL) {
        memcpy(out_element, list->head->data, list->element_size);
    }

    dds_linked_list_node_t* next = list->head->next;
    list->alloc.free(list->alloc.context, list->head);
    list->head = next;

    if (list->head == NULL) {
        list->tail = NULL;
    } else {
        list->head->prev = NULL;
    }

    list->size--;

    return DDS_OK;
}

static dds_linked_list_node_t* node_at(const dds_linked_list_t* list, size_t index) {
    dds_linked_list_node_t* node;

    if (index < list->size / 2) {
        node = list->head;
        for (size_t i = 0; i < index; i++) node = node->next;
    } else {
        node = list->tail;
        for (size_t i = list->size - 1; i > index; i--) node = node->prev;
    }

    return node;
}

dds_result_t dds_linked_list_get(const dds_linked_list_t* list, size_t index, void* out_element) {
    if (list == NULL) return DDS_INVALID_PARAMETER;
    if (index >= list->size) return DDS_OUT_OF_RANGE;
    if (out_element == NULL) return DDS_INVALID_PARAMETER;

    memcpy(out_element, node_at(list, index)->data, list->element_size);

    return DDS_OK;
}

dds_result_t dds_linked_list_set(dds_linked_list_t* list, size_t index, const void* element) {
    if (list == NULL) return DDS_INVALID_PARAMETER;
    if (index >= list->size) return DDS_OUT_OF_RANGE;
    if (element == NULL) return DDS_INVALID_PARAMETER;

    memcpy(node_at(list, index)->data, element, list->element_size);

    return DDS_OK;
}

dds_result_t dds_linked_list_insert(dds_linked_list_t* list, size_t index, const void* element) {
    if (list == NULL) return DDS_INVALID_PARAMETER;
    if (element == NULL) return DDS_INVALID_PARAMETER;
    if (index > list->size) return DDS_OUT_OF_RANGE;

    if (index == 0) return dds_linked_list_push_front(list, element);
    if (index == list->size) return dds_linked_list_push_back(list, element);

    dds_linked_list_node_t* new_node = list->alloc.malloc(list->alloc.context, sizeof(dds_linked_list_node_t) + list->element_size);
    if (new_node == NULL) return DDS_OUT_OF_MEMORY;

    memcpy(new_node->data, element, list->element_size);

    dds_linked_list_node_t* next_node = node_at(list, index);

    dds_linked_list_node_t* prev_node = next_node->prev;

    new_node->prev = prev_node;
    new_node->next = next_node;
    prev_node->next = new_node;
    next_node->prev = new_node;

    list->size++;

    return DDS_OK;
}

dds_result_t dds_linked_list_remove(dds_linked_list_t* list, size_t index, void* out_element) {
    if (list == NULL) return DDS_INVALID_PARAMETER;
    if (index >= list->size) return DDS_OUT_OF_RANGE;

    dds_linked_list_node_t* current_node = node_at(list, index);

    if (out_element != NULL) {
        memcpy(out_element, current_node->data, list->element_size);
    }

    dds_linked_list_node_t* prev_node = current_node->prev;
    dds_linked_list_node_t* next_node = current_node->next;

    list->alloc.free(list->alloc.context, current_node);

    if (prev_node == NULL && next_node == NULL) {
        list->head = NULL;
        list->tail = NULL;
    } else if (prev_node == NULL) {
        list->head = next_node;
        next_node->prev = NULL;
    } else if (next_node == NULL) {
        list->tail = prev_node;
        prev_node->next = NULL;
    } else {
        prev_node->next = next_node;
        next_node->prev = prev_node;
    }

    list->size--;

    return DDS_OK;
}

size_t dds_linked_list_get_size(const dds_linked_list_t* list) {
    if (list == NULL) return 0;

    return list->size;
}

bool dds_linked_list_is_empty(const dds_linked_list_t* list) {
    if (list == NULL) return true;

    return list->size == 0;
}

