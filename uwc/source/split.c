#include "input.h"
#include "vector.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

static char separator = 0;

void to_do(char *str, size_t len)
{
    printf("%.*s\n", (int)len, str);
}

int is_separator(unsigned char c)
{
    return c == (unsigned char)separator;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("one argument with separator character is required\n");
        return 1;
    } else {
        // for (int i = 0; i < argc; ++i) {
        // printf("%s\n", argv[i]);
        // }
        separator = argv[1][0];
        // foreach_input(to_do);
        foreach_input_with_separator(to_do, is_separator);
        return 0;
    }
}
