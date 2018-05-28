#include <time.h>

#include "glad/glad.h"

#include "gl_utils/texture.h"

#include "renderer/multires.h"
#include "containers/list.h"
#include "utils/format_string.h"
#include "utils/log.h"

#include "texture_loader.h"

void texture_loader_init(texture_loader_t* t, float max_time,
        char* base_path, char* path_format){
    t->max_time = max_time;
    t->path_format = path_format;
    t->base_path = base_path;
    t->nodes = NULL;
}

void texture_loader_delete(texture_loader_t* t){
    if(t->nodes == NULL)
        return;

    list_delete(t->nodes);
    t->nodes=NULL;
}

void texture_loader_add_node(texture_loader_t* t, node_t* n){
    if(t->nodes == NULL){
        t->nodes = list_create_from_data(n);
    }
    else if(t->nodes->data == NULL){
        t->nodes->data = n;
    }
    else {
        list_push(t->nodes, n);
    }
}

void texture_loader_process_node(texture_loader_t* t, node_t* n){
    glBindTexture(GL_TEXTURE_2D, n->texture);
    char path[1024];
    format_string_img_multires(path, 1024, t->path_format,
             t->base_path, n->face, n->x, n->y, n->level);
    load_img_into_texture(path, false);
    n->status = NODE_STATUS_LOADED;
}

void texture_loader_process(texture_loader_t* t, struct timespec t1){
    struct timespec t2;
    long elapsed;
    do{
        if(t->nodes == NULL || t->nodes->data == NULL)
            return;

        texture_loader_process_node(t, list_pop_front(t->nodes));

        timespec_get(&t2, TIME_UTC);
        elapsed = labs(t2.tv_nsec - t1.tv_nsec);
    }while(elapsed < (t->max_time * 1e6));
}
