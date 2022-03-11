#include "program.h"
#include "trie.h"
#include "vector.h"

#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

// debug
#include <stdio.h>
#define LOG_LN() printf("%s:%d\n", __FILE__, __LINE__)
#define LOG_MSG(msg) printf("%s:%d %s\n", __FILE__, __LINE__, msg)
#define LOG_NUM(num) printf("%s:%d %d\n", __FILE__, __LINE__, num)
#define LOG_ADDR(addr) printf("%s:%d %p\n", __FILE__, __LINE__, addr)

static struct mwc_program_config *config = NULL;

static int fill_trie_with_input(trie_t *t, int (*is_separator)(unsigned char), int interactive);
void print_key(trie_t *t, void *key, size_t len);
void print_count(trie_t *t, void *key, size_t len);
void print_count_filter(trie_t *t, void *key, size_t len);
void print_elem(void *key, size_t key_len, size_t count);
void print_elem_filter(void *key, size_t key_len, size_t count);

void print_key(trie_t *t, void *key, size_t len)
{
    printf("%p, %ld, '%.*s'\n", key, len, (int)len, (char *)key);
}

void print_count(trie_t *t, void *key, size_t len)
{
    if (config->dont_print_count) {
        printf("%.*s\n", (int)len, (char *)key);
    } else {
        printf("%.*s\t%ld\n", (int)len, (char *)key, trie_get(t, key, len));
    }
}

void print_count_filter(trie_t *t, void *key, size_t len)
{
    struct mwc_program_config conf = *config;
    size_t count = trie_get(t, key, len);
    if ((conf.filter_le_than && count <= conf.le_than) || (conf.filter_ge_than && count >= conf.ge_than)) {
        print_elem(key, len, count);
    }
}

void print_elem(void *key, size_t key_len, size_t count)
{
    if (config->dont_print_count) {
        printf("%.*s\n", (int)key_len, (char *)key);
    } else {
        printf("%.*s\t%ld\n", (int)key_len, (char *)key, count);
    }
}

static inline int is_newline(unsigned char c)
{
    return c == '\n';
}

static inline int is_space(unsigned char c)
{
    return c == ' ' || c == '\t' || c == '\n';
}

void print_elem_filter(void *key, size_t key_len, size_t count)
{
    struct mwc_program_config conf = *config;
    if ((conf.filter_le_than && count <= conf.le_than) || (conf.filter_ge_than && count >= conf.ge_than)) {
        print_elem(key, key_len, count);
    }
}

void start(struct mwc_program_config conf)
{
    config = &conf;
    trie_t t;
    trie_init(&t, 16, !conf.dont_save_keys_explicitly, conf.hash_keys);

    if (conf.lines) {
        fill_trie_with_input(&t, is_newline, conf.interactive);
    } else {
        fill_trie_with_input(&t, is_space, conf.interactive);
    }

    if (!conf.interactive) {
        if (conf.dont_save_keys_explicitly) {
            void (*functor)(void *, size_t, size_t);
            if (conf.filter_ge_than || conf.filter_le_than) {
                functor = print_elem_filter;
            } else {
                functor = print_elem;
            }
            if (conf.use_prefixes) {
                for (size_t i = 0; i < conf.prefixes.count; ++i) {
                    char *prefix = (char *)((size_t *)conf.prefixes.data + i);
                    trie_foreach_with_prefix(&t, prefix, strlen(prefix), functor);
                }
            } else {
                trie_foreach_elem(&t, functor);
            }
        } else {
            void (*functor)(trie_t *, void *, size_t);
            if (conf.filter_ge_than || conf.filter_le_than) {
                functor = print_count_filter;
            } else {
                functor = print_count;
            }

            if (conf.use_prefixes) {
                for (size_t i = 0; i < conf.prefixes.count; ++i) {
                    char *prefix = (char *)((size_t *)conf.prefixes.data + i);
                    trie_foreach_with_prefix(&t, prefix, strlen(prefix), functor == print_count ? print_elem : print_elem_filter);
                }
            } else {
                trie_foreach_key(&t, functor);
            }
        }
    }
    trie_clear(&t);
}

static inline int read_input_fill_buffer(void *buffer, size_t read_up_to)
{
    return read(STDIN_FILENO, buffer, read_up_to);
}

static inline int64_t get_first_str_len(unsigned char *buffer, size_t buffer_size, size_t offset, int (*is_separator)(unsigned char))
{
    size_t i = 0;
    for (i = 0; i < buffer_size - offset && !is_separator(buffer[offset + i]); ++i)
        ;
    return i;
}

static int fill_trie_with_input(trie_t *t, int (*is_separator)(unsigned char), int interactive)
{
    vector_t input_buffer;
    size_t v_initial_size = 8;
    size_t fill_size = 0;
    size_t offset = 0;

    vector_init(&input_buffer, sizeof(char), v_initial_size);
    while (fill_size = read_input_fill_buffer(input_buffer.data + offset, input_buffer.data_size - offset) + offset, fill_size != 0) {
        for (size_t idx = 0; idx < fill_size - 1; ++idx) {
            // skip all the whitespaces at the beginning
            for (; isspace(*(unsigned char *)(input_buffer.data + idx)); ++idx)
                ;
            if (idx < input_buffer.data_size) {
                char *str = NULL;
                size_t str_len = 0;

                str = (char *)(input_buffer.data + idx);
                str_len = get_first_str_len(input_buffer.data, input_buffer.data_size, idx, is_separator);

                // the case where string is as long or longer than buffer - grow the buffer
                if (str_len == input_buffer.data_size && idx == 0) {
                    if (vector_grow(&input_buffer)) {
                        fprintf(stderr, "out of memory");
                        vector_clear(&input_buffer);
                        return 1;
                    }
                    offset = str_len;
                }
                // roll the buffer because there may be more in next go
                else if (idx + str_len >= input_buffer.data_size && idx > 0) {
                    memmove(input_buffer.data, input_buffer.data + idx, str_len);
                    offset = str_len;
                }
                // the default normal case
                else {
                    size_t key_count = trie_insert(t, str, str_len);
                    if (key_count == SIZE_MAX) {
                        fprintf(stderr, "out of memory");
                        vector_clear(&input_buffer);
                        return 1;
                    }
                    if (interactive) {
                        printf("%.*s\t%ld\n", (int)str_len, (char *)str, key_count);
                    }
                    offset = 0;
                }
                idx += str_len;
            }
        }
    }

    vector_clear(&input_buffer);
    return 0;
}
