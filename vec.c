#include <stdlib.h>
#include <stdio.h>

#include "vec.h"

void _vector_init(vector * v) {
    v->capacity = VECTOR_INIT_CAPACITY;
    v->size = 0;
    v->items = malloc(sizeof(void*) * v->capacity);
}


size_t _vector_size(vector * v) {
    return v->size;
}
void _vector_resize(vector * v, size_t _newsize) {
    void **items = realloc(v->items, sizeof(void *) * _newsize);
    if (items) {
        v->items = items;
        v->capacity = _newsize;
        if (v->size > v->capacity) v->size = v->capacity;
    }
}

void _vector_push(vector * v, void * data) {
    if (v->capacity == v->size) _vector_resize(v, v->capacity<<1);
    v->items[v->size++] = data;
}
void _vector_pop(vector * v) {
    if (v->size == 0) return;
    v->items[--v->size] = NULL;
    if (v->size > 0 && v->size == v->capacity >> 2) {
        _vector_resize(v, v->capacity>>1);
    }
}

void _vector_insert(vector * v, size_t index, void * data) {
    if (v->size < index) {
        fprintf(stderr, "Vector out of bounds access (insert)\n");
        exit(EXIT_FAILURE);
    }
    if (v->capacity == v->size) _vector_resize(v, v->capacity<<1);
    
    if (index == v->size) {
        v->items[v->size++] = data; // same as push
        return;
    }

    for (size_t i = v->size++; i > index; --i) {
        v->items[i] = v->items[i-1];
        v->items[i-1] = NULL;
    }
    v->items[index] = data;
}
void _vector_erase(vector * v, size_t index) {
    if (v->size <= index) {
        fprintf(stderr, "Vector out of bounds access (erase)\n");
        exit(EXIT_FAILURE);
    }

    v->items[index] = NULL;
    --v->size;
    for (size_t i = index; i < v->size; ++i) {
        v->items[i] = v->items[i+1];
        v->items[i+1] = NULL;
    }

    if (v->size > 0 && v->size == v->capacity >> 2) {
        _vector_resize(v, v->capacity>>1);
    }
}

void _vector_set(vector * v, size_t index, void * data) {
    if (v->size <= index) {
        fprintf(stderr, "Vector out of bounds access (set)\n");
        exit(EXIT_FAILURE);
    }
    v->items[index] = data;
}
void * _vector_get(vector * v, size_t index) {
    if (v->size <= index) {
        fprintf(stderr, "Vector out of bounds access (get)\n");
        exit(EXIT_FAILURE);
    }
    return v->items[index];
}

void _vector_free(vector * v) {
    free(v->items);
}