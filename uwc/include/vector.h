#ifndef _mwc_vector_h_
#define _mwc_vector_h_ 1

#include <stdlib.h>

typedef struct
{
    void *data;
    volatile size_t data_size;
    size_t elem_size;
    size_t count;
    size_t top;
    char empty;
} vector_t;

inline size_t vector_top_idx(vector_t *v)
{
    return v->top;
}

int vector_init(vector_t *v, size_t elem_size, size_t initial_capacity);
void vector_clear(vector_t *);
void *vector_top(vector_t *);
void vector_pop(vector_t *);
size_t vector_push(vector_t *, void *);
size_t vector_push_many(vector_t *, void *, size_t);
int vector_grow(vector_t *);

#endif
