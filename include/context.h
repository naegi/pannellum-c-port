#ifndef CONTEXT_H
#define CONTEXT_H

#define CONTEXT_MULTIRES 0

#define CONTEXT_ALLOCATE_STEP 0x3 // Allocate context_elem_t by lot of 0x3

typedef struct{
    int type;
    void* context;
}context_elem_t;

typedef struct{
    int width, height;
    size_t length;
    size_t allocated_length;
    context_elem_t* array; // Store all contexes

    size_t current_context;
}context_t;

void context_init(context_t* c, int width, int height);
void context_delete(context_t* c);
void context_delete_elem(context_t* c, size_t i);
size_t context_add_elem(context_t* c, int type, void* context);
void context_resize(context_t* c, int width, int height);
void context_switch(context_t* c, size_t context_id);
void context_render(context_t* c, float hfov, float pitch, float yaw);

#endif /* ifndef CONTEXT_H */
