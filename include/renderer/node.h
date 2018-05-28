#ifndef NODE_H
#define NODE_H

#define NODE_STATUS_LOADING 0
#define NODE_STATUS_LOADED  1

typedef struct{
    unsigned int status, level;
    unsigned int face, x, y;

    // gl Stuff
    unsigned int texture, pos;
} node_t;

#endif /* ifndef NODE_H */
