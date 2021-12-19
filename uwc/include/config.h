#ifndef _uwc_config_h_
#define _uwc_config_h_ 1

#include "vector.h"
#include <stdlib.h>

struct mwc_program_config {
    char interactive;
    char filter_ge_than;
    char filter_le_than;
    char lines;
    char dont_save_keys_explicitly;
    char dont_print_count;
    char hash_keys;
    char use_prefixes;
    size_t ge_than;
    size_t le_than;
    vector_t prefixes;
};

int config_parse(struct mwc_program_config *conf, int argc, char **argv);

void config_clear(struct mwc_program_config *conf);

char *config_get_message();

#endif
