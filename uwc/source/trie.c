#include "trie.h"

#include <stdlib.h>
#include <string.h>

typedef struct
{
    size_t str_idx;
    size_t len;
} trie_key_t;

#include <stdio.h>

#define LOG_LN() printf("%s:%d\n", __FILE__, __LINE__)
#define LOG_MSG(msg) printf("%s:%d %s\n", __FILE__, __LINE__, msg)
#define LOG_NUM(num) printf("%s:%d %d\n", __FILE__, __LINE__, num)
#define LOG_ADDR(addr) printf("%s:%d %p\n", __FILE__, __LINE__, addr)

static size_t inner_trie_get(trie_t *t, void *key, size_t key_size);
static size_t inner_trie_insert(trie_t *t, void *key, size_t key_size);
static size_t get_node(trie_t *t, void *key, size_t key_size);

static uint64_t jenkins_hash(const uint8_t *key, size_t length) __attribute((hot));
static uint64_t jenkins_hash(const uint8_t *key, size_t length)
{
    size_t i = 0;
    uint64_t hash = 0;
    while (i != length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

int trie_init(trie_t *t, unsigned int initial_size, int store_keys, int hash_keys)
{
    t->elements = malloc(initial_size * sizeof(trie_elem_t));
    if (t->elements == NULL) {
        return -1;
    }
    t->elem_count = initial_size;
    t->position = 1;
    t->hash_keys = hash_keys;

    if (store_keys) {
        vector_init(&t->keys, sizeof(trie_key_t), initial_size);
        vector_init(&t->key_contents, sizeof(char), initial_size * 4);
    } else {
        t->keys.data = NULL;
        t->key_contents.data = NULL;
    }

    memset(t->elements, 0, t->elem_count * sizeof(trie_elem_t));

    return 0;
}

static int tire_grow(trie_t *t)
{
    size_t old_size = t->elem_count;
    size_t new_size = t->elem_count * 2;
    trie_elem_t *old_elems = t->elements;
    trie_elem_t *new_elems = NULL;

    new_elems = malloc(new_size * sizeof(trie_elem_t));

    if (new_elems == NULL) {
        return -1;
    }

    memcpy(new_elems, old_elems, old_size * sizeof(trie_elem_t));
    memset(new_elems + old_size, 0, (new_size - old_size) * sizeof(trie_elem_t));

    free(old_elems);
    t->elements = new_elems;
    t->elem_count = new_size;
    return 0;
}

static size_t inner_trie_insert(trie_t *t, void *key, size_t key_size)
{
    size_t position = 1;

    for (size_t key_it = 0; key_it < key_size; ++key_it) {
        char byte = *(char *)(key + key_it);

        for (char byte_it = 0; byte_it < 8; ++byte_it) {
            int one_or_zero = (byte & (1 << byte_it)) ? 1 : 0;

            if (t->elements[position].children[one_or_zero] == 0) {
                t->elements[position].children[one_or_zero] = ++t->position;
                position = t->position;
            } else {
                position = t->elements[position].children[one_or_zero];
            }

            if (position >= t->elem_count) {
                if (tire_grow(t)) {
                    fprintf(stderr, "out of memory");
                    return SIZE_MAX;
                }
            }
        }
    }

    return ++t->elements[position].count;
}

size_t trie_insert(trie_t *t, void *key, size_t key_size)
{
    size_t count = 0;

    if (t->hash_keys) {
        uint64_t hashed = jenkins_hash(key, key_size);
        count = inner_trie_insert(t, &hashed, sizeof(uint64_t));
    } else {
        count = inner_trie_insert(t, key, key_size);
    }

    if (count == SIZE_MAX) {
        fprintf(stderr, "out of memory");
        return SIZE_MAX;
    }

    if (count == 1 && t->keys.data != NULL) {
        trie_key_t tkey = { .len = key_size };
        tkey.str_idx = vector_push_many(&t->key_contents, key, key_size);
        if (tkey.str_idx == SIZE_MAX) {
            fprintf(stderr, "out of memory");
            return SIZE_MAX;
        }
        if (vector_push(&t->keys, &tkey) == SIZE_MAX) {
            fprintf(stderr, "out of memory");
            return SIZE_MAX;
        }
    }

    return count;
}

static size_t inner_trie_get(trie_t *t, void *key, size_t key_size)
{
    size_t position = 1;

    position = get_node(t, key, key_size);
    if (position != 0) {
        return t->elements[position].count;
    } else {
        return 0;
    }
}

static size_t get_node(trie_t *t, void *key, size_t key_size)
{
    size_t position = 1;

    for (size_t key_it = 0; key_it < key_size; ++key_it) {
        char byte = *(char *)(key + key_it);

        for (int byte_it = 0; byte_it < 8; ++byte_it) {
            int one_or_zero = (byte & (1 << byte_it)) ? 1 : 0;

            position = t->elements[position].children[one_or_zero];

            if (position == 0) {
                return 0;
            }
        }
    }

    return position;
}

size_t trie_get(trie_t *t, void *key, size_t key_size)
{
    if (t->hash_keys) {
        uint64_t hashed = jenkins_hash(key, key_size);
        return inner_trie_get(t, &hashed, sizeof(uint64_t));
    } else {
        return inner_trie_get(t, key, key_size);
    }
}

void trie_clear(trie_t *t)
{
    free(t->elements);
    vector_clear(&t->key_contents);
    vector_clear(&t->keys);
}

void trie_foreach_key(trie_t *t, void functor(trie_t *, void *, size_t))
{
    if (t->keys.data == NULL) {
        return;
    }
    for (size_t i = 0; i < t->keys.count; ++i) {
        trie_key_t *k = (trie_key_t *)(t->keys.data + i * t->keys.elem_size);
        functor(t, t->key_contents.data + k->str_idx, k->len);
    }
}

static void inner_for_each_elem(trie_t *t, vector_t *key_vector, void functor(void *, size_t, size_t), size_t position)
{
    trie_elem_t current0 = t->elements[position];

    for (int bit_0 = 0; bit_0 < 2; ++bit_0) {

        for (int bit_1 = 0; current0.children[bit_0] && bit_1 < 2; ++bit_1) {
            trie_elem_t current1 = t->elements[current0.children[bit_0]];

            for (int bit_2 = 0; current1.children[bit_1] && bit_2 < 2; ++bit_2) {
                trie_elem_t current2 = t->elements[current1.children[bit_1]];

                for (int bit_3 = 0; current2.children[bit_2] && bit_3 < 2; ++bit_3) {
                    trie_elem_t current3 = t->elements[current2.children[bit_2]];

                    for (int bit_4 = 0; current3.children[bit_3] && bit_4 < 2; ++bit_4) {
                        trie_elem_t current4 = t->elements[current3.children[bit_3]];

                        for (int bit_5 = 0; current4.children[bit_4] && bit_5 < 2; ++bit_5) {
                            trie_elem_t current5 = t->elements[current4.children[bit_4]];

                            for (int bit_6 = 0; current5.children[bit_5] && bit_6 < 2; ++bit_6) {
                                trie_elem_t current6 = t->elements[current5.children[bit_5]];

                                for (int bit_7 = 0; current6.children[bit_6] && bit_7 < 2; ++bit_7) {
                                    trie_elem_t current7 = t->elements[current6.children[bit_6]];
                                    size_t current_position = current7.children[bit_7];

                                    if (current_position) {
                                        char byte = bit_7 << 7 | bit_6 << 6 | bit_5 << 5 | bit_4 << 4 | bit_3 << 3 | bit_2 << 2 | bit_1 << 1 | bit_0;
                                        trie_elem_t current8 = t->elements[current_position];
                                        vector_push(key_vector, &byte);

                                        if (current8.count) {
                                            functor(key_vector->data, key_vector->count, current8.count);
                                        }

                                        inner_for_each_elem(t, key_vector, functor, current_position);
                                        vector_pop(key_vector);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void trie_foreach_elem(trie_t *t, void functor(void *, size_t, size_t))
{
    vector_t key_vector = { 0 };
    vector_init(&key_vector, sizeof(char), 32);
    inner_for_each_elem(t, &key_vector, functor, 1);
    vector_clear(&key_vector);
}

void trie_foreach_with_prefix(trie_t *t, void *prefix, size_t prefix_len, void functor(void *, size_t, size_t))
{
    size_t position = get_node(t, prefix, prefix_len);

    if (position != 0) {
        if (t->elements[position].count) {
            functor(prefix, prefix_len, t->elements[position].count);
        }

        vector_t key_vector;
        vector_init(&key_vector, sizeof(char), 32);
        vector_push_many(&key_vector, prefix, prefix_len);
        inner_for_each_elem(t, &key_vector, functor, position);
        vector_clear(&key_vector);
    }
}
