#ifndef OPCODE_CC4B_H
#define OPCODE_CC4B_H

#include <stdint.h>

#define OP_INC          ((uint8_t) 0x01)
#define OP_SET          ((uint8_t) 0x02)
#define OP_ADD          ((uint8_t) 0x03)
#define OP_MAD          ((uint8_t) 0x04)
#define OP_MAD16        ((uint8_t) 0x84)
#define OP_SAD          ((uint8_t) 0x05)
#define OP_XMUL         ((uint8_t) 0x06)
#define OP_LCP          ((uint8_t) 0x07)
#define OP_RCP          ((uint8_t) 0x08)
#define OP_MUL          ((uint8_t) 0x09)
#define OP_SHL          ((uint8_t) 0x0A)

#define OP_JZ           ((uint8_t) 0x10)
#define OP_JZ16         ((uint8_t) 0x11)
#define OP_JZ32         ((uint8_t) 0x90)
#define OP_JNZ          ((uint8_t) 0x12)
#define OP_JNZ16        ((uint8_t) 0x13)
#define OP_JNZ32        ((uint8_t) 0x92)

#define OP_MOV          ((uint8_t) 0x20)
#define OP_MOV16        ((uint8_t) 0x21)
#define OP_MOV32        ((uint8_t) 0xA0)
#define OP_SEEK         ((uint8_t) 0x22)
#define OP_SEEK16       ((uint8_t) 0xA2)

#define OP_GET          ((uint8_t) 0x30)
#define OP_GETS         ((uint8_t) 0x31)
#define OP_GETS16       ((uint8_t) 0xB1)
#define OP_PUT          ((uint8_t) 0x32)
#define OP_PUTC         ((uint8_t) 0x33)
#define OP_PUTS         ((uint8_t) 0x34)
#define OP_PUTS16       ((uint8_t) 0xB4)

#define OP_ERR          ((uint8_t) 0x70)
#define OP_ERP          ((uint8_t) 0x71)

#endif