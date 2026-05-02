#ifndef IRNODE_CC4B_H
#define IRNODE_CC4B_H

#include <stdint.h>
#include "collection/uhmap.h"
#include "opcode.h"

#define IK_NOP          ((IRNodeKind) 0)
#define IK_LOOP         ((IRNodeKind) 1)
#define IK_TAPEINC      ((IRNodeKind) 2)
#define IK_MOV          ((IRNodeKind) 3)
#define IK_MAD          ((IRNodeKind) 4)
#define IK_GET          ((IRNodeKind) 5)
#define IK_PUT          ((IRNodeKind) 6)
#define IK_PATTERN      ((IRNodeKind) 7)

typedef uint8_t IRNodeKind;

typedef struct IRNode {
    IRNodeKind kind;
    struct IRNode *next;

    union {
        struct IRNode *block;
        UnorderedHashMap *tape;
        struct {
            uint8_t opcode;
            int32_t a, b, c, d;
        } args;
    };
} IRNode;

IRNode *irnodenew(IRNode *after, IRNodeKind kind);
void irnodekill(IRNode *irnode);

#endif