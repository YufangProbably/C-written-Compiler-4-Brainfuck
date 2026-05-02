#ifndef PARSE_CC4B_H
#define PARSE_CC4B_H

#include "collection/result.h"
#include "collection/vector.h"
#include "collection/uhmap.h"
#include "exports.h"
#include "irnode.h"

#define ICT_NOP         ((InstrCharType) 0)
#define ICT_TAPEINC     ((InstrCharType) 1)
#define ICT_OPEN        ((InstrCharType) 2)
#define ICT_END         ((InstrCharType) 3)
#define ICT_IO          ((InstrCharType) 4)

typedef uint8_t InstrCharType;

typedef struct {
    IRNode *head;
    IRNode *this;
    IRNode *loop_head;
    Vector *loop_stack;

    InstrCharType last_type;

    UnorderedHashMap *tape;
    ptrdiff_t offset;
} CC4BParseState;

CC4B_API Result cc4bparse_init(CC4BParseState *state);
CC4B_API Result cc4bparse_step(CC4BParseState *state, char ch);

#endif