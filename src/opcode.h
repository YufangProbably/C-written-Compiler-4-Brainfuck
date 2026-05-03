#ifndef OPCODE_CC4B_H
#define OPCODE_CC4B_H

#include <stdint.h>

#define CC4B_OP_INC     ((uint8_t) 0x01)
#define CC4B_OP_SET     ((uint8_t) 0x02)
#define CC4B_OP_ADD     ((uint8_t) 0x03)
#define CC4B_OP_MAD     ((uint8_t) 0x04)
#define CC4B_OP_MAD16   ((uint8_t) 0x84)
#define CC4B_OP_SAD     ((uint8_t) 0x05)
#define CC4B_OP_XMUL    ((uint8_t) 0x06)
#define CC4B_OP_LCP     ((uint8_t) 0x07)
#define CC4B_OP_RCP     ((uint8_t) 0x08)
#define CC4B_OP_MUL     ((uint8_t) 0x09)
#define CC4B_OP_SHL     ((uint8_t) 0x0A)

#define CC4B_OP_JZ      ((uint8_t) 0x10)
#define CC4B_OP_JZ16    ((uint8_t) 0x11)
#define CC4B_OP_JZ32    ((uint8_t) 0x90)
#define CC4B_OP_JNZ     ((uint8_t) 0x12)
#define CC4B_OP_JNZ16   ((uint8_t) 0x13)
#define CC4B_OP_JNZ32   ((uint8_t) 0x92)

#define CC4B_OP_MOV     ((uint8_t) 0x20)
#define CC4B_OP_MOV16   ((uint8_t) 0x21)
#define CC4B_OP_MOV32   ((uint8_t) 0xA0)
#define CC4B_OP_SEEK    ((uint8_t) 0x22)
#define CC4B_OP_SEEK16  ((uint8_t) 0xA2)

#define CC4B_OP_GET     ((uint8_t) 0x30)
#define CC4B_OP_GETS    ((uint8_t) 0x31)
#define CC4B_OP_GETS16  ((uint8_t) 0xB1)
#define CC4B_OP_PUT     ((uint8_t) 0x32)
#define CC4B_OP_PUTC    ((uint8_t) 0x33)
#define CC4B_OP_PUTS    ((uint8_t) 0x34)
#define CC4B_OP_PUTS16  ((uint8_t) 0xB4)

#define CC4B_OP_ERR     ((uint8_t) 0x70)
#define CC4B_OP_ERP     ((uint8_t) 0x71)

#endif