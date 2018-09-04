#include <stdlib.h>

#include "renderer/multires.h"
#include "renderer/transition.h"
#include "context.h"
#include "config.h"
#include "utils/log.h"

void context_init(context_t* c, int width, int height){
    c->array = NULL; // OK because if realloc(NULL, size) will allocate
    c->length = c->allocated_length = 0;
    c->height = height;
    c->width = width;
    c->current_context = 0;
}

void context_delete_elem(context_t* c, size_t i){
    context_elem_t* e = &c->array[i];
    switch (e->type) {
        case CONFIG_MULTIRES:
            multires_delete(e->context);
            break;
        case CONFIG_TRANSITION:
            transition_delete(e->context);
            break;
    }
    free(e->context);

}

void context_delete(context_t* c){

    for (int i = 0; i < c->length; ++i) {
        context_delete_elem(c, i);
    }

    if(c->array != NULL)
        free(c->array);

}

size_t context_add_elem(context_t* c, int type, void* cfg){
    if(c->length == c->allocated_length){
        c->array = realloc(c->array, sizeof(context_elem_t) * (c->allocated_length + CONTEXT_ALLOCATE_STEP));
        c->allocated_length += CONTEXT_ALLOCATE_STEP;
    }
    int id = c->length;
    c->length++;

    c->array[id].type = type;

    switch (type) {
        case CONFIG_MULTIRES:
            c->array[id].context = malloc(sizeof(multires_data_t));
             multires_init_from_cfg(c->array[id].context, (config_multires_t*)cfg);
            break;
        case CONFIG_TRANSITION:
            c->array[id].context = malloc(sizeof(transition_t));
             transition_init(c->array[id].context, ""); /* TODO ! */
            break;
        default:
            error("Unknow context type");
            exit(-1);
    }
    return id;
}

void context_notify_resize(context_t* c){
    context_elem_t* e = &c->array[c->current_context];
    switch (e->type) {
        case CONFIG_MULTIRES:
            multires_set_viewport(e->context, c->width, c->height);
            break;
        case CONFIG_TRANSITION:
            transition_set_viewport(e->context, c->width, c->height);
            break;
    }
}

void context_resize(context_t* c, int width, int height){
    c->width = width;
    c->height = height;
    context_notify_resize(c);
}

void context_unload(context_t* c, size_t context_id){
    context_elem_t* e = &c->array[context_id];
    switch (e->type) {
        case CONFIG_MULTIRES:
            multires_unload(e->context);
            break;
        case CONFIG_TRANSITION:
            transition_unload(e->context);
            break;
        default:
            error("NotImplementedError\n");
    }
}
void context_load(context_t* c, size_t context_id){
    context_elem_t* e = &c->array[context_id];
    switch (e->type) {
        case CONFIG_MULTIRES:
            multires_load(e->context);
            break;
        case CONFIG_TRANSITION:
            transition_load(e->context);
            break;
        default:
            error("NotImplementedError\n");
    }
}

context_renderer_fct context_renderer(context_t* c, size_t context_id){
    switch (c->array[c->current_context].type) {
        case CONFIG_MULTIRES:
            return (context_renderer_fct) multires_render;
        case CONFIG_TRANSITION:
            return (context_renderer_fct) transition_render;
        default:
            error("NotImplementedError\n");
    }
    return NULL;
}

void context_switch(context_t* c, size_t context_id){
    if(context_id > c->length){
        error("Context id don't exist");
        return;
    }

    context_unload(c, c->current_context);
    c->current_context = context_id;
    context_load(c, c->current_context);
    c->current_renderer = context_renderer(c, c->current_context);
    context_notify_resize(c);
}

void context_render(context_t* c, float hfov, float pitch, float yaw){
    if(c->current_renderer == NULL)
        return;

    int ret = c->current_renderer(c->array[c->current_context].context, hfov, pitch, yaw);
    switch(ret){
        case 0:
            break;
        case 1: // Go to next pano
            context_switch(c, 3);
            break;
    }
}
