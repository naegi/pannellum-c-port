#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

#include "context.h"

#define CONFIG_MULTIRES        0
#define CONFIG_TRANSITION      1

typedef struct {
    int type;
    void* config;
}config_elem_t;

typedef struct{
    char* path_format;
    char* base_path;
    int tile_size, cube_size, max_level;
}config_multires_t;

typedef struct{
    char* base_path;
    char* config_path;

    size_t valid_ids_length;
    int* valid_ids;

    size_t links_length;
    //link_t* links;

    size_t start;
}config_t;

size_t configure_context(context_t* ctx, config_t* cfg);

#endif /* ifndef CONFIG_H */
