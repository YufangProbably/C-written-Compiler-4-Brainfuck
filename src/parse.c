#include <stdlib.h>
#include "parse.h"

Result cc4bparse_init(CC4BParseState *state) {
    Vector *stack = vecnew(sizeof(IRNode *), 8);
    if (stack == NULL) goto err;

    IRNode *head = irnodenew(NULL, IK_NOP);
    if (head == NULL) goto err_stack;

    state->head = head;
    state->this = head;
    state->loop_head = head;
    state->loop_stack = stack;
    state->last_type = ICT_NOP;
    state->tape = NULL;
    state->offset = 0;

    return RES_OK;

err_stack:
    free(stack);
err:
    return RES_ERR;
}

Result cc4bparse_step(CC4BParseState *state, char ch) {
    return RES_ERR;
}