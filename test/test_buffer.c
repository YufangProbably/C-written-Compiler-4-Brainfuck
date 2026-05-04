#define TEST_HERE
#include "test.h"

#include <string.h>
#include "debug/dump.h"
#include "collection/buffer.h"

TEST(buffer) {
    Buffer *buf = bufnew(256);

    bufwrite_u8(buf, 114);
    bufwrite_i16(buf, 514);
    bufwrite_i32(buf, 1919810);
    bufwrite_u32(buf, 10);
    bufwrite_nstr(buf, "YAJUSENPAI", 10);

    buf->offset = 0;
    int32_t value;

    value = bufread_u8(buf);
    assert(value == 114);
    printf("\"%d", value);

    value = bufread_i16(buf);
    assert(value == 514);
    printf("%d", value);

    value = bufread_i32(buf);
    assert(value == 1919810);
    printf(", %d. \"\n", value);

    size_t length = bufread_u32(buf);
    assert(length == 10);
    uint8_t *recv = malloc(length + 1);
    bufread_nstr(buf, recv, length);
    recv[length] = 0;
    assert(strcmp(recv, "YAJUSENPAI") == 0);
    printf("    -- %s\n", recv);

    free(recv);
    bufkill(buf);
}