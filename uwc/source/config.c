#include "config.h"

#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char message[1024] = { 0 };
extern int optind;

#define SET_ERR_MSG(msg) snprintf(message, 1024, "%s", msg)
#define LOG_ADDR(addr) printf("%s:%d %p\n", __FILE__, __LINE__, addr)

int config_parse(struct mwc_program_config *conf, int argc, char **argv)
{
    char c = 0;
    int option_index = 0;
    static struct option long_options[] = {
        { "less-than", required_argument, 0, 1 },
        { "more-than", required_argument, 0, 2 },
        { "prefix", required_argument, 0, 3 },
        { 0, 0, 0, 0 }
    };

    vector_init(&conf->prefixes, sizeof(char *), 2);

    while ((c = getopt_long(argc, argv, "limdh", long_options, &option_index)) != -1) {
        int64_t value = -1;
        switch (c) {
        case 1: {
            // less than
            if ((value = atoll(optarg)) < 2) {
                SET_ERR_MSG("less-than has to be more than 1");
                return -1;
            } else {
                conf->le_than = value;
                conf->filter_le_than = 1;
            }
            break;
        }

        case 2: {
            // more than
            if ((value = atoll(optarg)) < 0) {
                SET_ERR_MSG("more-than cannot be less than 0");
                return -1;
            } else {
                conf->ge_than = value;
                conf->filter_ge_than = 1;
            }
            break;
        }

        case 3: {
            // use prefix, --prefix
            conf->use_prefixes = 1;
            vector_push(&conf->prefixes, optarg);
            break;
        }

        case 'i': {
            /*  interactive mode - print count while reading input */
            conf->interactive = 1;
            break;
        }

        case 'l': {
            /*  use whole lines as keys */
            conf->lines = 1;
            break;
        }

        case 'm': {
            /*  this option will make program use a little less memory for random input case
                but for input with long identical prefixes it will make a difference */
            conf->dont_save_keys_explicitly = 1;
            break;
        }

        case 'h': {
            /*  this option will make program hash keys to save memory to constant size for each key
                can't be combined with -m because it has to store keys somwere */
            conf->hash_keys = 1;
            break;
        }

        case 'd': {
            conf->dont_print_count = 1;
            break;
        }

        default: {
            SET_ERR_MSG("unknown option");
            return -1;
            break;
        }
        }
    }

    if (conf->interactive && conf->dont_save_keys_explicitly) {
        SET_ERR_MSG("-i and -m don't work together");
        return -1;
    } else if (conf->hash_keys && conf->use_prefixes) {
        SET_ERR_MSG("you can't hash keys and use prefixes, -h and --prefix don't work together");
        return -1;
    } else if (conf->interactive && conf->use_prefixes) {
        SET_ERR_MSG("you can't use interactive mode and use prefixes, -i and --prefix don't work together");
        return -1;
    } else if (conf->hash_keys && conf->dont_save_keys_explicitly) {
        SET_ERR_MSG("-h and -m don't work together");
        return -1;
    }

    return 0;
}

char *config_get_message()
{
    return message;
}

void config_clear(struct mwc_program_config *conf)
{
    vector_clear(&conf->prefixes);
}
