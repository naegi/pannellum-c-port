
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "glad/glad.h"
#include "linmath.h"

#include "utils/log.h"

#include "renderer/multires.h"
#include "renderer/node.h"

#include "texture_loader.h"

#include "gl_utils/texture.h"
#include "gl_utils/shader.h"
#include "gl_utils/cube_vertices.h"

#define max(x, y) (x) > (y) ? (x) : (y)
#define min(x, y) (x) < (y) ? (x) : (y)


/**
 * Compute the level for face
 *
 * This function try to determine which level is the more appropriated to
 * display. For this, we project 2 opposed points onto the screen and we watch
 * the distance between x coords and y coords. Further more, we want to scale this
 * to screen size. So, we just need to scale it by a factor of cube_size/screen_size.
 * We obtain a 2^level because of the very nature of level. So, we just need to do
 * a log2 on it to get level
 */
static int get_current_level(multires_data_t* d, mat4x4 persp, mat4x4 rot, int face){
    vec4 TL, BR,
         r1, r2, // After rotation
         r3, r4; // After rotation and projection

    int max_wh = d->screen_height > d->screen_width ?
        d->screen_height : d->screen_width;

    memcpy(TL, vertices[face] + 3*0, sizeof(float) * 3);
    memcpy(BR, vertices[face] + 3*2, sizeof(float) * 3);
    TL[3] = 1;
    BR[3] = 1;

    mat4x4_mul_vec4(r1, rot, TL);
    mat4x4_mul_vec4(r2, rot, BR);

    mat4x4_mul_vec4(r3, persp, r1);
    mat4x4_mul_vec4(r4, persp, r2);

    float dx = fabsf(r3[0] - r4[0]);
    float dy = fabsf(r3[1] - r4[1]);

    int level = ceilf(log2f(min(dx, dy) * (float)d->cube_size/(float)max_wh)) + 1;

    // Make sure it is between correct bounds
    return max(min(d->max_level, level), 1);
}

static float get_max_tile(multires_data_t* d, int level){
    float actual_cube_size = (float)d->cube_size / (float)(1<<(d->max_level - level));
    return actual_cube_size/(float)d->tile_size;
}

static void generate_face(multires_data_t* d, unsigned int face, unsigned int level){
    info("Generate face %d at level%d\n", face, level);

    int max_tiles = ceilf(get_max_tile(d, level));
    for (int x = 0; x < max_tiles; ++x) {
        for (int y = 0; y < max_tiles; ++y) {
            node_t *n = multires_add_node(d, level, face, x, y);
            texture_loader_add_node(d->texture_loader, n);
        }
    }
}

void multires_load(multires_data_t*d){
    multires_set_current_level(d, 1);
}
void multires_unload(multires_data_t* d){
    if(d->nodes == NULL)
        return;
    texture_loader_delete(d->texture_loader);
    list_map(d->nodes, multires_delete_node);
    list_delete(d->nodes);
    d->nodes = NULL;
}

void multires_set_current_level(multires_data_t* d, int level){
    info("Setting level to %d\n", level);
    multires_unload(d);
    d->current_level = level;

    for (int i = 0; i < 6; ++i) {
        generate_face(d, i, level);
    }
}

void multires_init(multires_data_t* d, unsigned char max_level,
                   int tile_size, int cube_size, char* base_path,
                   char* path_format, float roll){
    d->screen_width = 0;
    d->screen_height = 0;
    d->roll = roll;
    d->cube_size = cube_size;
    d->tile_size = tile_size;
    d->max_level = max_level;
    d->nodes = NULL;

    d->texture_loader = malloc(sizeof(texture_loader_t));

    texture_loader_init(d->texture_loader, 1./60./*60Hz*/,
            base_path, path_format);

    struct shader_file shaders_data[] = {
        {GL_FRAGMENT_SHADER, "ressources/shaders/multires.frag"},
        {GL_VERTEX_SHADER,   "ressources/shaders/multires.vert"}
    };
    d->shader = create_program_from_shader_files(shaders_data, 2);
    multires_generate_gl_stuff(d);
    glUseProgram(d->shader);

    multires_set_current_level(d, 1);
}

void multires_delete(multires_data_t* d){
    info("Deleting multires\n");
    texture_loader_delete(d->texture_loader);

    multires_unload(d);
    free(d->texture_loader);

    glDeleteProgram(d->shader);
    glDeleteVertexArrays(1, &(d->VAO));
    glDeleteBuffers(2, d->buffers);
}


void gen_node_pos(multires_data_t*d, float *pos, int face, int x, int y, int level){
    memcpy(pos, vertices[face], sizeof(float)*3*4);
    if(level == 1)
        return; // EZ

    float max_tile = get_max_tile(d, level);

    float ratio_left_x = (float)x/max_tile;
    float ratio_right_x = fmin((float)(x + 1)/max_tile, 1.0);

    float ratio_top_y = (float)y/max_tile;
    float ratio_bottom_y = fmin((float)(y + 1)/max_tile, 1.0);

    // Cube face origin is top-left (TL)
    // We need vectors
    // - origin (O) -> TL
    // - TL -> top-right (TR)
    // - TL > Bottom-right (BL)

    float* O_TL = (vertices[face] + 0*3);
    float* O_TR = (vertices[face] + 1*3);
    float* O_BL = (vertices[face] + 3*3);
    vec3 TL_TR, TL_BL;
    vec3_sub(TL_TR, O_TR, O_TL);
    vec3_sub(TL_BL, O_BL, O_TL);

    // now, O_Node =O_TL + x*TL_TR + y*TL_BL

    vec3 xTL_TR, yTL_BL;

    vec3_scale(xTL_TR, TL_TR, ratio_left_x);
    vec3_scale(yTL_BL, TL_BL, ratio_top_y);
    // Top Left
    vec3_add(pos, (vec3){0}, O_TL);
    vec3_add(pos, pos, xTL_TR);
    vec3_add(pos, pos, yTL_BL);


    vec3_scale(xTL_TR, TL_TR, ratio_right_x);
    // Top Right
    vec3_add(pos + 3, (vec3){0}, O_TL);
    vec3_add(pos + 3, pos + 3, xTL_TR);
    vec3_add(pos + 3, pos + 3, yTL_BL);

    vec3_scale(yTL_BL, TL_BL, ratio_bottom_y);
    // Bottom Right
    vec3_add(pos + 6, (vec3){0}, O_TL);
    vec3_add(pos + 6, pos + 6, xTL_TR);
    vec3_add(pos + 6, pos + 6, yTL_BL);

    vec3_scale(xTL_TR, TL_TR, ratio_left_x);
    // Bottom Left
    vec3_add(pos + 9, (vec3){0}, O_TL);
    vec3_add(pos + 9, pos + 9, xTL_TR);
    vec3_add(pos + 9, pos + 9, yTL_BL);

    vec3_min(pos, (vec3){1, 1, 1}, pos);
    vec3_max(pos, (vec3){-1, -1, -1}, pos);
    vec3_min(pos + 3, (vec3){1, 1, 1},  pos + 3);
    vec3_max(pos + 3, (vec3){-1, -1, -1}, pos + 3);
    vec3_min(pos + 6, (vec3){1, 1, 1},  pos + 6);
    vec3_max(pos + 6, (vec3){-1, -1, -1}, pos + 6);
}

node_t* multires_add_node(multires_data_t* d, int level, int face, int x, int y){
    node_t* node = malloc(sizeof(node_t));
    node->status = NODE_STATUS_LOADING;
    node->level = level;
    node->face = face;
    node->x = x;
    node->y = y;

    glGenTextures(1, &(node->texture));
    glGenBuffers(1, &(node->pos));
    glBindBuffer(GL_ARRAY_BUFFER, node->pos);
    float pos[4*3]; // (xyz)*(TL+TR+BL+BR)

    debug("Generating node pos face:%d, level:%d x:%d y:%d\n", face, level, x, y);
    gen_node_pos(d, pos, face, x, y, level);

    debug("\n\tTL %f %f %f\tTR %f %f %f"
          "\n\tBL %f %f %f\tBR %f %f %f \n",
            pos[0], pos[1], pos[2],
            pos[3], pos[4], pos[5],
            pos[9], pos[10], pos[11],
            pos[6], pos[7], pos[8]
            );

    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*4, pos, GL_STATIC_DRAW);

    if(d->nodes == NULL){
        d->nodes = list_create_from_data(node);
    }
    else{
        list_push(d->nodes, node);
    }
    return node;
}

void multires_delete_node(list_t* l, int _){
    node_t *n = (node_t*)l->data;
    debug("Deleting node face:%d, level:%d, x:%d, y:%d\n", n->face, n->level, n->x, n->y);
    l->data = NULL;

    glDeleteTextures(1, &(n->texture));
    glDeleteBuffers(1, &(n->pos));

    free(n);
}

void _multires_draw(list_t* l, int _){
    node_t* node = (node_t*) l->data;

    if(node->status == NODE_STATUS_LOADED){
        // bind textures
        glBindTexture(GL_TEXTURE_2D, node->texture);

        // Pos
        glBindBuffer(GL_ARRAY_BUFFER, node->pos);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);

        // Draw !
        glDrawElements(GL_TRIANGLES, 6,  GL_UNSIGNED_SHORT, 0);
    }
}

void multires_draw(multires_data_t *d){
    glClearColor(0.4f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(d->VAO);

    list_map(d->nodes, _multires_draw);
}

void multires_set_viewport(multires_data_t* d, int width, int height){
    d->screen_width = width;
    d->screen_height = height;
}

void multires_render(multires_data_t* d, float fov, float pitch, float yaw){
    struct timespec t1;
    timespec_get(&t1, TIME_UTC);

    mat4x4 persp;
    mat4x4_perspective(persp, fov, (float)d->screen_width/d->screen_height, 0.1, 100);
    UNIFORM_SET(d->shader, "projection", Matrix4fv, 1, GL_FALSE, (float*)persp);

    mat4x4 transf;
    mat4x4_identity(transf);
    mat4x4_rotate_Z(transf, transf, -d->roll);
    mat4x4_rotate_X(transf, transf, -pitch);
    mat4x4_rotate_Y(transf, transf, -yaw);

    int level = get_current_level(d, persp, transf, 0);
    if(d->current_level < level || abs(d->current_level - level) >= 2){
        multires_set_current_level(d, level);
    }

    UNIFORM_SET(d->shader, "text", 1i, 0); // or with shader class
    UNIFORM_SET(d->shader, "transform", Matrix4fv, 1, GL_FALSE, (float*)transf);

    multires_draw(d);

    texture_loader_process(d->texture_loader, t1);
}

void multires_generate_gl_stuff(multires_data_t* d){
    glGenVertexArrays(1, &(d->VAO));
    glGenBuffers(2, d->buffers);

    glBindVertexArray(d->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, d->texCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8,
                 (float[]){0,0 ,1,0 ,1,1 ,0,1}, GL_STATIC_DRAW);

    // Tex coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, (void*)0);

    // Setup EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*6,
                 (unsigned short[]){0,1,2, 0,2,3},
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}
