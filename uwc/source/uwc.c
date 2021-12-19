#include "config.h"
#include "program.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    struct mwc_program_config config = { 0 };

    if (config_parse(&config, argc, argv) != 0) {
        config_clear(&config);
        printf("%s\n", config_get_message());
        return 1;
    }

    start(config);
    config_clear(&config);
    return 0;
}
