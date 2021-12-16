#ifndef _uwc_config_h_
#define _uwc_config_h_ 1

#include <stdlib.h>

struct mwc_program_config {
    char interactive;
    char filter_ge_than;
    char filter_le_than;
    char lines;
    char dont_save_keys_explicitly;
    char dont_print_count;
    char hash_keys;
    size_t ge_than;
    size_t le_than;
};

int config_parse(struct mwc_program_config *conf, int argc, char *argv[]);

char *config_get_message();

#endif
