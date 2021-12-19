#include "input.h"
#include "vector.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static inline int is_space(unsigned char c)
{
    return c == ' ' || c == '\t' || c == '\n';
}

static inline int64_t get_first_str_len(unsigned char *buffer, size_t buffer_size, size_t offset)
{
    size_t i = 0;
    for (i = 0; i < buffer_size - offset && !isspace(buffer[offset + i]); ++i)
        ;
    return i;
}

int foreach_input(void to_do(char *str, size_t len))
{
    vector_t input_buffer;
    size_t v_initial_size = 8;
    size_t fill_size = 0;
    size_t offset = 0;

    vector_init(&input_buffer, sizeof(char), v_initial_size);
    while (fill_size = read(STDIN_FILENO, input_buffer.data + offset, input_buffer.data_size - offset) + offset, fill_size != 0) {
        for (size_t idx = 0; idx < fill_size - 1; ++idx) {
            // skip all the whitespaces at the beginning
            for (; isspace(*(unsigned char *)(input_buffer.data + idx)); ++idx)
                ;
            if (idx < input_buffer.data_size) {
                char *str = NULL;
                size_t str_len = 0;

                str = (char *)(input_buffer.data + idx);
                str_len = get_first_str_len(input_buffer.data, input_buffer.data_size, idx);

                // the case where string is as long or longer than buffer - grow the buffer
                if (str_len == input_buffer.data_size && idx == 0) {
                    vector_grow(&input_buffer);
                    offset = str_len;
                }
                // roll the buffer because there may be more in next go
                else if (idx + str_len >= input_buffer.data_size && idx > 0) {
                    memcpy(input_buffer.data, input_buffer.data + idx, str_len);
                    offset = str_len;
                }
                // the default normal case
                else {
                    to_do(str, str_len);
                    offset = 0;
                }
                idx += str_len;
            }
        }
    }

    vector_clear(&input_buffer);
    return 0;
}
