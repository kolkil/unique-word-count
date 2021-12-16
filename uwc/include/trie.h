#ifndef _trie_set_h_
#define _trie_set_h_ 1

#include "stdint.h"
#include "vector.h"

typedef struct
{
    size_t count;
    size_t children[2];
} trie_elem_t;

typedef struct
{
    vector_t keys;
    vector_t key_contents;
    trie_elem_t *elements;
    size_t position;
    size_t elem_count;
    char hash_keys;
} trie_t;

int trie_init(trie_t *t, unsigned int initial_size, int store_keys, int hash_keys);

void trie_clear(trie_t *t);

size_t trie_insert(trie_t *t, void *key, size_t key_size);

size_t trie_get(trie_t *t, void *key, size_t key_size);

void trie_foreach_key(trie_t *t, void functor(trie_t *, void *, size_t));

void trie_foreach_elem(trie_t *t, void functor(void *, size_t, size_t));

#endif
