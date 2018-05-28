#ifndef ARGS_H
#define ARGS_H
#include <stdbool.h>
#include "utils/utils.h"

struct args{
    bool debug;
    char* json;
};

int renderer_str_to_enum(char* renderer_str);

void help(char* program_name);

void parse_args(struct args* a, int argc, char **argv);

#endif /* ifndef ARGS_H */
