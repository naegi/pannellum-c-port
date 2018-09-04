#include <stdlib.h>
#include <time.h>
#include <glad/glad.h>

#include "utils/log.h"
#include "gl_utils/shader.h"
#include "gl_utils/texture.h"
#include "linmath.h"

#include <renderer/transition.h>

void transition_init(transition_t* t, char* reconstruct_path){
    t->EBO_length = 268;
    t->roll = 0;
    t->screen_height = 0;
    t->screen_width = 0;

    timespec_get(&t->t0, TIME_UTC);

    struct shader_file shaders_data_transition[] = {
        {GL_FRAGMENT_SHADER, "ressources/shaders/transition.frag"},
        {GL_VERTEX_SHADER,   "ressources/shaders/transition.vert"}
    };
    t->shader = create_program_from_shader_files(shaders_data_transition, 2);
    transition_generate_gl_stuff(t);
}

void transition_delete(transition_t* t){
    glDeleteProgram(t->shader);
    glDeleteVertexArrays(1, t->VAOs);
    glDeleteBuffers(2, t->buffers);
}

void transition_load(transition_t* t){
    timespec_get(&t->t0, TIME_UTC);
}
void transition_unload(transition_t* _){
}

void transition_set_viewport(transition_t* t, int width, int height){
    t->screen_width = width;
    t->screen_height = height;
}

void transition_draw_transition(transition_t* t){
    glClearColor(0.4f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(t->VAO);
    glDrawElements(GL_TRIANGLES, t->EBO_length,  GL_UNSIGNED_SHORT, 0);
}

void transition_generate_gl_stuff(transition_t* t){
    glGenVertexArrays(1, t->VAOs);
    glGenBuffers(2, t->buffers);

    glBindVertexArray(t->VAO);

#include "../trans.h"

    glBindBuffer(GL_ARRAY_BUFFER, t->mesh);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    glEnableVertexAttribArray(0);

#include "../transEBO.h"

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(EBO), EBO, GL_STATIC_DRAW);

    load_img_into_texture("10.jpg", true);
}

int transition_render(transition_t* t, float fov, float pitch, float yaw){
    struct timespec t1;
    timespec_get(&t1, TIME_UTC);

    glUseProgram(t->shader);
    mat4x4 persp;
    mat4x4_perspective(persp, fov, (float)t->screen_width/t->screen_height, .0001, 10000);

    double elapsed = (t1.tv_nsec - t->t0.tv_nsec) / 1e6 + (t1.tv_sec - t->t0.tv_sec) * 1000; // ms
    double factor = elapsed / 500;//t->time_trans; // factor is between 0 and 1. When at 0, pos is at 1st pano, when at 1, pos is the snd pano


    vec3 pano1 = {7.5158057522478385, 6.468029153169482, 1.9999922970309854};
    vec3 pano2 = {9.589123959360444, 10.915957251665532, 1.9999834513291717};

    vec3 pos;
    vec3_sub(pos, pano2, pano1);
    vec3_scale(pos, pos, factor);
    vec3_add(pos, pos, pano1);

    mat4x4 transf;
    mat4x4_translate(transf, pos[0], pos[1], pos[2]);
    mat4x4_rotate_Z(transf, transf, -t->roll);
    mat4x4_rotate_X(transf, transf, -pitch);
    mat4x4_rotate_Y(transf, transf, -yaw);

    UNIFORM_SET(t->shader, "transform", Matrix4fv, 1, GL_FALSE, (float*)transf);
    UNIFORM_SET(t->shader, "projection", Matrix4fv, 1, GL_FALSE, (float*)persp);

    transition_draw_transition(t);
    if(factor > 1.0)
        return 1;
    return 0;
}
