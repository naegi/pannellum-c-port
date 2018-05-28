#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <time.h>

#include "renderer/multires.h"

typedef struct{
    float max_time;
    list_t* nodes;
    char* base_path;
    char* path_format;
} texture_loader_t;

void texture_loader_init(texture_loader_t* t, float max_time,
        char* base_path, char* path_format);
void texture_loader_delete(texture_loader_t* t);
void texture_loader_add_node(texture_loader_t* t, node_t* n);
void texture_loader_process(texture_loader_t* t, struct timespec t1);

#endif /* ifndef TEXTURE_LOADER_H */
