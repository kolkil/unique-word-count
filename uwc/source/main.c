#include "config.h"
#include "program.h"

#include <stdio.h>

int main(int argc, char const *argv[])
{
    struct mwc_program_config config = { 0 };

    if (config_parse(&config, argc, argv) != 0) {
        printf("%s\n", config_get_message());
        return 1;
    }

    start(config);
    return 0;
}
