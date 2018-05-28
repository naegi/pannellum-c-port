#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stddef.h>

typedef struct node {
    void* data;
    struct node * before;
    struct node * next;
} list_t;

static list_t* list_create(void);
static list_t* list_create_from_data(void* data);
static list_t* list_create_from_array(void* datas, size_t length, size_t sizeof_data);

// DON'T pop next in map. But you can pop current or before if index > 1
static void list_map(list_t *l, void (*f)(list_t* l, int index));
static void list_delete(list_t *l);
static list_t* list_search(list_t *l, void* data);
static void* list_pop_front(list_t *l);

static void* list_pop_elem(list_t *l, void* data);
static void list_push(list_t *l, void* data);



static inline list_t* list_create(void){
    return list_create_from_data(NULL);
}

static inline list_t* list_create_from_data(void* data){
    list_t* l = malloc(sizeof(list_t));
    l->next = l->before = l;
    l->data = data;
    return l;
}

static inline list_t* list_create_from_array(void* datas, size_t length, size_t sizeof_data){
    if(length == 0)
        return NULL;

    list_t* l = list_create_from_data(datas);

    // Yeah 1, 0 already done thanks to list_create_from_data
    for (int i = 1; i < length; ++i) {
        list_push(l, datas + i*sizeof_data);
    }
    return l;
}


static inline void list_delete(list_t *l){
    if(l == NULL)
        return;
    while(l->next != l)
        list_pop_front(l);
    free(l);
}


static inline void list_map(list_t *l, void (*f)(list_t* l, int index)){
    if(l == NULL)
        return;
    list_t* c = l;
    size_t i = 0;
    do{
        f(c, i);
        i++;
        c = c->next;
    }while(l != c);
}

static inline list_t* list_search(list_t *l, void* data){
    if(l == NULL)
        return NULL;

    list_t* c = l;
    do{
        if(c->data == data)
            return c;
        c = c->next;
    }while(l != c);
    return NULL;
}

static inline void* list_pop_front(list_t *l){
    if(l == NULL)
        return NULL;
    void* data = l->data;
    l->data = NULL;

    if(l->next == l && l->before == l->next)
        return data;

    // To not free l, we will move l->next's data into l and free l->next
    list_t *tmp = l->next;
    l->data = tmp->data;

    tmp->next->before = l;
    l->next = tmp->next;

    free(tmp);
    return data;

}

static inline void* list_pop_elem(list_t *l, void* data){
    list_t* c = list_search(l, data);
    if(c == NULL)
        return NULL;

    return(list_pop_front(c));
}

static inline void list_push(list_t *l, void* data){
    if(l == NULL)
        return;
    list_t* current = l->before;
    list_t* next = list_create();

    current->next = next;
    next->before = current;
    next->next = l;
    l->before = next;

    next->data = data;
}
#endif /* ifndef LIST_H */
