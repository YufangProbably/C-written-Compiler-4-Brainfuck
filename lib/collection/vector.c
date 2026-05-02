#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "vector.h"

Vector *vecnew(size_t elem_size, size_t cap) {
    Vector *vec = malloc(sizeof(Vector));
    if (vec == NULL) goto err;

    if (cap < VEC_MINCAP) cap = VEC_MINCAP;
    void *data = calloc(cap, elem_size);
    if (data == NULL) goto err_vec;

    vec->data = data;
    vec->capacity = cap;
    vec->length = 0;
    vec->elem_size = elem_size;

    return vec;

err_vec:
    free(vec);
err:
    return NULL;
}

void veckill(Vector *vec) {
    if (vec == NULL) return;

    free(vec->data);
    free(vec);
}

Result vecres(Vector *vec, size_t cap) {
    if (cap <= vec->capacity) return RES_OK;

    void *data = realloc(vec->data, cap * vec->elem_size);
    if (data == NULL) return RES_ERR;

    vec->data = data;
    vec->capacity = cap;

    return RES_OK;
}

static Result _vecexpand(Vector *vec) {
    if (vec->length < vec->capacity) return RES_OK;

    size_t cap = vec->capacity / 2 * 3;
    if (cap < VEC_MINCAP) cap = VEC_MINCAP;
    return vecres(vec, cap);
}

static Result _vecexpands(Vector *vec, size_t growth) {
    size_t cap = vec->capacity;
    while (vec->length + growth > cap)
        cap = cap / 2 * 3;

    if (cap < VEC_MINCAP) cap = VEC_MINCAP;
    return vecres(vec, cap);
}

void *vecindex(Vector *vec, ptrdiff_t index) {
    if (index < 0) index %= vec->length;
    if (index >= vec->length) return NULL;

    return (uint8_t *) vec->data + (index * vec->elem_size);
}

Result vecat(Vector *vec, ptrdiff_t index, void *elem) {
    void *source = vecindex(vec, index);
    if (source == NULL) return RES_ERR;

    memcpy(elem, source, vec->elem_size);
    return RES_OK;
}

Result vecset(Vector *vec, ptrdiff_t index, void *elem) {
    void *target = vecindex(vec, index);
    if (target == NULL) return RES_ERR;

    memcpy(target, elem, vec->elem_size);
    return RES_OK;
}

Result vecpush(Vector *vec, void *elem) {
    Result res = _vecexpand(vec);
    if (res) return res;

    void *target = (uint8_t *) vec->data + (vec->length * vec->elem_size);
    memcpy(target, elem, vec->elem_size);
    vec->length++;
    return RES_OK;
}

Result vecpop(Vector *vec, void *elem) {
    if (vec->length == 0) return RES_ERR;

    vec->length--;
    void *source = (uint8_t *) vec->data + (vec->length * vec->elem_size);
    memcpy(elem, source, vec->elem_size);
    return RES_OK;
}

Vector *vecdup(Vector *origin) {
    Vector *dup = vecnew(origin->elem_size, origin->capacity);
    if (dup == NULL) return NULL;

    dup->length = origin->length;
    memcpy(dup->data, origin->data, origin->elem_size * origin->length);

    return dup;
}

Vector *vecsub(Vector *vec, ptrdiff_t start, size_t length) {
    Vector *sub = vecnew(vec->elem_size, length);
    if (sub == NULL) return NULL;

    sub->length = length;
    void *vec_start = (uint8_t *) vec->data + vec->elem_size * start;
    memcpy(sub->data, vec_start, vec->elem_size * length);

    return sub;
}

Result vecext(Vector *source, Vector *target) {
    if (source->elem_size != target->elem_size) return RES_ERR;
    size_t elem_size = source->elem_size;

    Result res = _vecexpands(source, target->length);
    if (res) return res;

    void *end = (uint8_t *) source->data + elem_size * source->length;
    memcpy(end, target->data, elem_size * target->length);
    source->length += target->length;

    return RES_OK;
}

Vector *veccat(Vector *left, Vector *right) {
    if (left->elem_size != right->elem_size) return NULL;
    size_t elem_size = left->elem_size;

    Vector *cat = vecnew(elem_size, left->length + right->length);
    if (cat == NULL) return NULL;

    void *end = (uint8_t *)cat->data + elem_size * left->length;

    cat->length = left->length + right->length;
    memcpy(cat->data, left->data, elem_size * left->length);
    memcpy(end, right->data, elem_size * right->length);

    return cat;
}