#include <stdlib.h>
#include <string.h>
#include "buffer.h"

Buffer *bufnew(size_t cap) {
    Buffer *buf = malloc(sizeof(Buffer));
    if (buf == NULL) goto err;

    if (cap < BUF_MINCAP) cap = BUF_MINCAP;
    void *data = calloc(cap, 1);
    if (data == NULL) goto err_buf;

    buf->data = data;
    buf->capacity = cap;
    buf->length = 0;
    buf->offset = 0;

    return buf;

err_buf:
    free(buf);
err:
    return NULL;
}

void bufkill(Buffer *buf) {
    if (buf == NULL) return;

    free(buf->data);
    free(buf);
}

Result bufres(Buffer *buf, size_t cap) {
    if (cap <= buf->capacity) return RES_OK;

    void *data = realloc(buf->data, cap);
    if (data == NULL) return RES_ERR;

    buf->data = data;
    buf->capacity = cap;

    return RES_OK;
}

static Result _bufexpand(Buffer *buf) {
    if (buf->length < buf->capacity) return RES_OK;

    size_t cap = buf->capacity / 2 * 3;
    if (cap < BUF_MINCAP) cap = BUF_MINCAP;
    return bufres(buf, cap);
}

static Result _bufexpands(Buffer *buf, size_t growth) {
    size_t cap = buf->capacity;
    while (buf->length + growth > cap)
        cap = cap / 2 * 3;

    if (cap < BUF_MINCAP) cap = BUF_MINCAP;
    return bufres(buf, cap);
}

Buffer *bufdup(Buffer *origin) {
    Buffer *dup = bufnew(origin->capacity);
    if (dup == NULL) return NULL;

    dup->length = origin->length;
    dup->offset = origin->offset;
    memcpy(dup->data, origin->data, origin->length);

    return dup;
}

Result bufwrite_u8(Buffer *buf, uint8_t data) {
    Result res = _bufexpand(buf);
    if (res) return res;

    buf->data[buf->length++] = data;
    return RES_OK;
}

Result bufwrite_u16(Buffer *buf, uint16_t data) {
    Result res = _bufexpands(buf, 2);
    if (res) return res;

    buf->data[buf->length++] = 0xFF & (data >> 0);
    buf->data[buf->length++] = 0xFF & (data >> 8);
    return RES_OK;
}

Result bufwrite_u32(Buffer *buf, uint32_t data) {
    Result res = _bufexpands(buf, 4);
    if (res) return res;

    buf->data[buf->length++] = 0xFF & (data >>  0);
    buf->data[buf->length++] = 0xFF & (data >>  8);
    buf->data[buf->length++] = 0xFF & (data >> 16);
    buf->data[buf->length++] = 0xFF & (data >> 24);
    return RES_OK;
}

Result bufwrite_i8(Buffer *buf, int8_t data) {
    return bufwrite_u8(buf, (uint8_t) data);
}

Result bufwrite_i16(Buffer *buf, int16_t data) {
    return bufwrite_u16(buf, (uint16_t) data);
}

Result bufwrite_i32(Buffer *buf, int32_t data) {
    return bufwrite_u32(buf, (uint32_t) data);
}

Result bufwrite_nstr(Buffer *buf, const uint8_t *data, size_t count) {
    Result res = _bufexpands(buf, count);
    if (res) return res;

    memcpy(buf->data + buf->length, data, count);
    buf->length += count;
    return RES_OK;
}

uint8_t bufread_u8(Buffer *buf) {
    return buf->data[buf->offset++];
}

uint16_t bufread_u16(Buffer *buf) {
    uint16_t res = 0;
    res |= (uint16_t)buf->data[buf->offset++] << 0;
    res |= (uint16_t)buf->data[buf->offset++] << 8;
    return res;
}

uint32_t bufread_u32(Buffer *buf) {
    uint32_t res = 0;
    res |= (uint32_t)buf->data[buf->offset++] <<  0;
    res |= (uint32_t)buf->data[buf->offset++] <<  8;
    res |= (uint32_t)buf->data[buf->offset++] << 16;
    res |= (uint32_t)buf->data[buf->offset++] << 24;
    return res;
}

int8_t bufread_i8(Buffer *buf) {
    return (int8_t) bufread_u8(buf);
}

int16_t bufread_i16(Buffer *buf) {
    return (int16_t) bufread_u16(buf);
}

int32_t bufread_i32(Buffer *buf) {
    return (int32_t) bufread_u32(buf);
}

void bufread_nstr(Buffer *buf, uint8_t *data, size_t count) {
    memcpy(data, buf->data + buf->offset, count);
    buf->offset += count;
}