#ifndef IRNODE_CC4B_H
#define IRNODE_CC4B_H

#include <stdint.h>
#include "collection/fhmap.h"
#include "opcode.h"

#define CC4B_IK_NOP          ((CC4BIRNodeKind) 0)
#define CC4B_IK_LOOP         ((CC4BIRNodeKind) 1)
#define CC4B_IK_TAPEINC      ((CC4BIRNodeKind) 2)
#define CC4B_IK_MOV          ((CC4BIRNodeKind) 3)
#define CC4B_IK_MAD          ((CC4BIRNodeKind) 4)
#define CC4B_IK_GET          ((CC4BIRNodeKind) 5)
#define CC4B_IK_PUT          ((CC4BIRNodeKind) 6)
#define CC4B_IK_PATTERN      ((CC4BIRNodeKind) 7)

typedef uint8_t CC4BIRNodeKind;

typedef struct CC4BIRNode {
    CC4BIRNodeKind kind;
    struct CC4BIRNode *next;

    union {
        struct CC4BIRNode *block;
        FlatHashMap *pending;
        ptfdiff_t offset;
        struct {
            uint8_t code;
            int32_t a, b, c, d;
        } pattern;
    } data;
} CC4BIRNode;

CC4BIRNode *cc4b_irnodenew(CC4BIRNode *after, CC4BIRNodeKind kind);
void cc4b_irnodekill(CC4BIRNode *irnode);

#endif