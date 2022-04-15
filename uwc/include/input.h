#ifndef _mwc_input_h_
#define _mwc_input_h_ 1

#include "config.h"

int foreach_input(void to_do(char *str, size_t len));
int foreach_input_with_separator(void to_do(char *str, size_t len), int (*is_separator)(unsigned char c));

#endif
