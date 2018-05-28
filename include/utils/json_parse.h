#ifndef JSON_PARSE_H
#define JSON_PARSE_H

#include "utils/args.h"

#include "config.h"

config_t* json_parse_main_config(char* filename);
config_elem_t* json_parse_elem_config(char* filename);

#endif /* ifndef JSON_PARSE_H */
