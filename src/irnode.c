#include <stdlib.h>
#include "irnode.h"

CC4BIRNode *cc4b_irnodenew(CC4BIRNode *after, CC4BIRNodeKind kind) {
    CC4BIRNode *node = malloc(sizeof(CC4BIRNode));
    if (node == NULL) return NULL;

    node->kind = kind;
    if (after != NULL) after->next = node;

    return node;
}

void cc4b_irnodekill(CC4BIRNode *head) {
    CC4BIRNode *this = head, *next;
    while (this != NULL) {
        next = this->next;

        if (this->kind == CC4B_IK_LOOP)
            cc4b_irnodekill(this->args.block);
        else if (this->kind == CC4B_IK_TAPEINC)
            fhmapkill(this->tape);
        free(this);

        this = next;
    }
}