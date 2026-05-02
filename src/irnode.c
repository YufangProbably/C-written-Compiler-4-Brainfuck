#include <stdlib.h>
#include "irnode.h"

IRNode *irnodenew(IRNode *after, IRNodeKind kind) {
    IRNode *irnode = malloc(sizeof(IRNode));
    if (irnode == NULL) return NULL;

    irnode->kind = kind;
    if (after != NULL) after->next = irnode;

    return irnode;
}

void irnodekill(IRNode *head) {
    IRNode *this = head, *next;
    while (this != NULL) {
        next = this->next;

        if (this->kind == IK_LOOP)
            irnodekill(this->block);
        else if (this->kind == IK_TAPEINC)
            uhmapkill(this->tape);
        free(this);

        this = next;
    }
}