#ifndef BUFFER_YCC_H
#define BUFFER_YCC_H

#include <stdint.h>
#include <stddef.h>
#include "result.h"

#define BUF_MINCAP 256

#define BUFEND(buf) ((buf)->length >= (buf)->length ? RES_OK : RES_ERR)

typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t length;
    size_t offset;
} Buffer;

Buffer *bufnew(size_t cap);
void bufkill(Buffer *buf);
Result bufres(Buffer *buf, size_t cap);
Buffer *bufdup(Buffer *origin);

Result bufwrite_u8(Buffer *buf, uint8_t data);
Result bufwrite_u16(Buffer *buf, uint16_t data);
Result bufwrite_u32(Buffer *buf, uint32_t data);
Result bufwrite_i8(Buffer *buf, int8_t data);
Result bufwrite_i16(Buffer *buf, int16_t data);
Result bufwrite_i32(Buffer *buf, int32_t data);
Result bufwrite_nstr(Buffer *buf, const uint8_t *data, size_t count);

uint8_t bufread_u8(Buffer *buf);
uint16_t bufread_u16(Buffer *buf);
uint32_t bufread_u32(Buffer *buf);
int8_t bufread_i8(Buffer *buf);
int16_t bufread_i16(Buffer *buf);
int32_t bufread_i32(Buffer *buf);
void bufread_nstr(Buffer *buf, uint8_t *data, size_t count);

#endif