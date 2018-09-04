#ifndef MULTIRES_H
#define MULTIRES_H

#include <stddef.h>

#include "containers/list.h"
#include "renderer/node.h"

#include "texture_loader.h"
#include "config.h"

#define MULTIRES_NODE_FACE_FRONT  0
#define MULTIRES_NODE_FACE_BACK   1
#define MULTIRES_NODE_FACE_TOP    2
#define MULTIRES_NODE_FACE_BOTTOM 3
#define MULTIRES_NODE_FACE_LEFT   4
#define MULTIRES_NODE_FACE_RIGHT  5

typedef struct{
    unsigned int tile_size, cube_size,
                 screen_width, screen_height,
                 current_level;
    float roll;
    unsigned char max_level;

    texture_loader_t* texture_loader;

    unsigned int shader;

    union {
        unsigned int VAOs[1];
        struct {
            unsigned int VAO;
        };
    };
    union{
        unsigned int buffers[2];
        struct{
            unsigned int texCoords, EBO;
        };
    };

    list_t* nodes;
} multires_data_t;

/**
 * initialise the multires_draw_t structure with arguments and initialise needed openGl ressources (shaders...)
 */
void multires_init(multires_data_t* d, unsigned char max_level,
                   int tile_size, int cube_size,
                   char* base_path, char* path_format, float roll);
static inline void multires_init_from_cfg(multires_data_t* d, config_multires_t* a){
    multires_init(d, a->max_level, a->tile_size, a->cube_size,
                     a->base_path, a->path_format, 0);
}
void multires_set_viewport(multires_data_t* d, int width, int height);
int multires_render(multires_data_t* d, float fov, float pitch, float yaw);
void multires_delete(multires_data_t* d);

// Private //TODO: hide
node_t* multires_add_node(multires_data_t* d, int level, int face, int x, int y);
void multires_delete_node(list_t*, int);
void multires_delete_nodes(multires_data_t*);

void multires_draw(multires_data_t*);
void multires_generate_gl_stuff(multires_data_t* d);
void multires_set_current_level(multires_data_t* d, int level);
void multires_unload(multires_data_t* d);
void multires_load(multires_data_t* d);
#endif /* ifndef MULTIRES_H */
