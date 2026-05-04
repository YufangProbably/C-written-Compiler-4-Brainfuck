#ifndef PARSE_CC4B_H
#define PARSE_CC4B_H

#include <stdint.h>
#include <stddef.h>
#include "collection/result.h"
#include "collection/vector.h"
#include "collection/fhmap.h"
#include "exports.h"
#include "irnode.h"

#define CC4B_PST_NOP    ((CC4BParseStateType) 0)
#define CC4B_PST_IGNORE ((CC4BParseStateType) 1)
#define CC4B_PST_TAPE   ((CC4BParseStateType) 2)
#define CC4B_PST_OPEN   ((CC4BParseStateType) 3)
#define CC4B_PST_END    ((CC4BParseStateType) 4)
#define CC4B_PST_IO     ((CC4BParseStateType) 5)

typedef uint8_t CC4BParseStateType;

#define CC4B_TST_INC    ((CC4BParseStateType) 0)
#define CC4B_TST_SET    ((CC4BParseStateType) 1)

typedef uint8_t CC4BTapeSlotType;

typedef struct {
    CC4BTapeSlotType type;
    uint8_t value;
} CC4BTapeSlot;

typedef struct {
    CC4BIRNode *head;
    CC4BIRNode *this;
    Vector *brackets;
    CC4BParseStateType type;

    union {
        struct {
            FlatHashMap *pending;
            ptrdiff_t offset;
        } tape;
        uint32_t skip_depth;
    } ctx;
} CC4BParseState;

CC4B_API Result cc4b_parseinit(CC4BParseState *state);
CC4B_API Result cc4b_parsestep(CC4BParseState *state, unsigned char ch);
CC4B_API CC4BIRNode *cc4b_parseend(CC4BParseState *state);

#endif