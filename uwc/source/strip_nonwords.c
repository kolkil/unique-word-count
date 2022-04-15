#include "input.h"
#include "vector.h"

#include <ctype.h>
#include <iconv.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

void to_do(char *str, size_t len)
{
    // assume, that if at least single letter is present, this is word
    for (size_t i = 0; i < len; ++i) {
        if (str[i] >= 'a' && str[i] <= 'z' || str[i] >= 'A' && str[i] <= 'Z') {
            printf("%.*s\n", (int)len, str); // flush the output
            return;
        }
    }
}

int main()
{
    foreach_input(to_do);
    return 0;
}
