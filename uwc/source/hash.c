#include "input.h"
#include <stdint.h>
#include <stdio.h>

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

void to_do(char *str, size_t len)
{
    printf("%lu\n", jenkins_hash((uint8_t *)str, len));
}

int main()
{
    foreach_input(to_do);
    return 0;
}
