#include "vector.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

int vector_grow(vector_t *v)
{
    void *old_data = v->data;
    void *new_data = malloc(v->data_size * 2);

    if (new_data == NULL) {
        return -1;
    }
    if (old_data != new_data) {
        memcpy(new_data, old_data, v->data_size);
        free(old_data);
    }
    v->data_size *= 2;
    v->data = new_data;

    return 0;
}

int vector_init(vector_t *v, size_t elem_size, size_t initial_capacity)
{
    if (v == NULL)
        return 1;

    v->empty = 1;
    v->count = 0;
    v->elem_size = elem_size;
    v->top = 0;
    v->data = calloc(initial_capacity, v->elem_size);
    v->data_size = initial_capacity * v->elem_size;

    if (v->data == NULL)
        return 1;
    else
        return 0;
}

void vector_clear(vector_t *v)
{
    if (v == NULL || v->data == NULL)
        return;
    free(v->data);
}

void *vector_top(vector_t *v)
{
    return v == NULL || v->empty ? NULL : v->data + v->top;
}

void vector_pop(vector_t *v)
{
    if (v->top == 0) {
        if (v->count > 0) {
            v->count -= 1;
            v->empty = 1;
        }
        return;
    }

    v->top -= v->elem_size;
    v->count -= 1;

    if (v->count == 0) {
        v->empty = 1;
    }
}
size_t vector_push(vector_t *v, void *restrict item)
{
    if (v->empty) {
        v->empty = 0;
    } else {
        v->top += v->elem_size;
        if (v->top >= v->data_size) {
            if (vector_grow(v)) {
                fprintf(stderr, "out of memory");
                return SIZE_MAX;
            }
        }
    }
    memcpy(v->data + v->top, item, v->elem_size);
    ++v->count;
    return v->top;
}

size_t vector_push_many(vector_t *v, void *items, size_t item_count)
{
    size_t first_top = 0;
    size_t last_top = 0;
    if (v->empty) {
        v->empty = 0;
        last_top = v->top + v->elem_size * item_count - 1;
        first_top = v->top;
    } else {
        last_top = v->top + v->elem_size * item_count;
        first_top = v->top + v->elem_size;
    }
    while (last_top >= v->data_size) {
        if (vector_grow(v)) {
            fprintf(stderr, "out of memory");
            return SIZE_MAX;
        }
    }
    memcpy(v->data + first_top, items, v->elem_size * item_count);
    v->top = last_top;
    v->count += item_count;
    return first_top;
}
