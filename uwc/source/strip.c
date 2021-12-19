#include "input.h"
#include "vector.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

void to_do(char *str, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        if (isalnum(str[i])) {
            printf("%c", str[i]);
        }
    }
    printf("\n"); // flush the output
}

int main()
{
    foreach_input(to_do);
    return 0;
}
