#ifndef TRANSITION_H
#define TRANSITION_H

#include <time.h>

typedef struct{
    struct timespec t0;
    long time_trans; // In ms

    float roll;
    unsigned int screen_width, screen_height;
    size_t EBO_length;
    unsigned int shader;
    union {
        unsigned int VAOs[1];
        unsigned int VAO;
    };
    union {
        unsigned int buffers[2];
        struct{
            unsigned int EBO, mesh;
        };
    };

} transition_t;

void transition_init(transition_t*, char*);
void transition_delete(transition_t*);
void transition_load(transition_t*);
void transition_set_viewport(transition_t*, int width, int height);
void transition_unload(transition_t*);
int transition_render(transition_t*, float hfov, float pitch, float yaw);

#endif /* ifndef TRANSITION_H */
