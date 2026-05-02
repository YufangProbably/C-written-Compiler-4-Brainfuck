#ifndef VECTOR_YCC_H
#define VECTOR_YCC_H

#include <stddef.h>
#include "result.h"

#define VEC_MINCAP 8

typedef struct {
    void *data;
    size_t capacity;
    size_t length;
    size_t elem_size;
} Vector;

Vector *vecnew(size_t elem_size, size_t cap);
void veckill(Vector *vec);
Result vecres(Vector *vec, size_t cap);
void *vecindex(Vector *vec, ptrdiff_t index);
Result vecat(Vector *vec, ptrdiff_t index, void *elem);
Result vecset(Vector *vec, ptrdiff_t index, void *elem);
Result vecpush(Vector *vec, void *elem);
Result vecpop(Vector *vec, void *elem);
Vector *vecdup(Vector *vec);
Vector *vecsub(Vector *vec, ptrdiff_t start, size_t length);
Result vecext(Vector *source, Vector *target);
Vector *veccat(Vector *left, Vector *right);

#endif