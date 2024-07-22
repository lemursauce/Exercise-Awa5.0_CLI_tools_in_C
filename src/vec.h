#ifndef _CUSTOM_VEC_H_
#define _CUSTOM_VEC_H_

// This is highly based on https://eddmann.com/posts/implementing-a-dynamic-vector-array-in-c/ with some slight modifications

#include <stdlib.h>

#define VECTOR_INIT_CAPACITY 4

typedef struct {
    void **items;
    size_t capacity;
    size_t size;
} vector;

/////////////

#define VECTOR_INIT(v) _vector_init(&v)

#define VECTOR_SIZE(v) _vector_size(&v)
#define VECTOR_RESIZE(v,s) _vector_resize(&v,s)

#define VECTOR_PUSH(v,d) _vector_push(&v,(void*)d)
#define VECTOR_POP(v) _vector_pop(&v)

#define VECTOR_INSERT(v,i,d) _vector_insert(&v,i,(void*)d)
#define VECTOR_ERASE(v,i) _vector_erase(&v,i)

#define VECTOR_SET(v,i,d) _vector_set(&v,i,(void*)d)
#define VECTOR_GET(v,i) _vector_get(&v,i)

#define VECTOR_FREE(v) _vector_free(&v)

/////////////

void _vector_init(vector * v);


size_t _vector_size(vector * v);
void _vector_resize(vector * v, size_t _newsize);

void _vector_push(vector * v, void * data);
void _vector_pop(vector * v);

void _vector_insert(vector * v, size_t index, void * data);
void _vector_erase(vector * v, size_t index);

void _vector_set(vector * v, size_t index, void * data);
void * _vector_get(vector * v, size_t index);

void _vector_free(vector * v);

#endif