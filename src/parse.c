#include <stdlib.h>
#include "parse.h"

static const CC4BParseStateType _state_map[256] = {
    ['+'] = CC4B_PST_TAPEINC,
    [','] = CC4B_PST_IO,
    ['-'] = CC4B_PST_TAPEINC,
    ['.'] = CC4B_PST_IO,
    ['>'] = CC4B_PST_TAPEINC,
    ['<'] = CC4B_PST_TAPEINC,
    ['['] = CC4B_PST_OPEN,
    [']'] = CC4B_PST_END,
};

static uint32_t _offset_hash(const void *value) {
    return (uint32_t) (*(const ptrdiff_t *) value);
}
static Result _offset_as(const void *left, const void *right) {
    ptrdiff_t
        left_val = *(const ptrdiff_t *) left;
        right_val = *(const ptrdiff_t *) right;
    return left_val == right_val;
}

static void _tapemodify(CC4BParseState *state, unsigned char ch) {
    if (state->type != CC4B_PST_TAPEINC) {
        state->ctx.tape.pending = fhmapnew(
            sizeof(ptrdiff_t),
            sizeof(CC4BTapeOperation),
            8,
            _offset_hash,
            _offset_as
        );
        state->ctx.tape.offset = 0;
    }

    switch (ch) {
    case '+':
    case '-': {
        CC4BTapeOperation slot;
        fhmapat(state->ctx.tape.pending, &state->ctx.tape.offset, &slot);
        slot.value += ch == '+' ? 1 : -1;
        fhmapset(state->ctx.tape.pending, &state->ctx.tape.offset, &slot);
        break;
    }
    case '>':
        state->ctx.tape.offset++;
        break;
    case '<':
        state->ctx.tape.offset--;
        break;
    }
}

static Result _tapeflush(CC4BParseState *state) {
    CC4BIRNode *node_ti = cc4b_irnodenew(state->this, CC4B_IK_TAPEINC);
    if (node_ti == NULL) goto err;

    node_ti->data.pending = state->ctx.tape.pending;

    CC4BIRNode *node_mov = cc4b_irnodenew(node_ti, CC4B_IK_MOV);
    if (node_mov == NULL) goto err_ti;

    node_mov->data.offset = state->ctx.tape.offset;
    state->this = node_mov;

    return RES_OK;

err_ti:
    cc4b_irnodekill(node_ti);
err:
    return RES_ERR;
}

Result cc4b_parseinit(CC4BParseState *state) {
    Vector *stack = vecnew(sizeof(CC4BIRNode *), 8);
    if (stack == NULL) goto err;

    CC4BIRNode *head = cc4b_irnodenew(NULL, IK_NOP);
    if (head == NULL) goto err_stack;

    state->head = head;
    state->this = head;
    state->brackets = stack;
    state->type = ICT_NOP;

    return RES_OK;

err_stack:
    free(stack);
err:
    return RES_ERR;
}

Result cc4b_parsestep(CC4BParseState *state, unsigned char ch) {
    CC4BParseStateType
        this = _state_map[ch],
        next = state->ctx;

    if (this == CC4B_PST_NOP)
        return RES_OK;
    if (state->type == CC4B_PST_IGNORE) {
        if (this == CC4B_PST_OPEN)
            state->ctx.skip_depth++;
        else if (this == CC4B_PST_END)
            if (ctx.skip_depth != 0) state->ctx.skip_depth--;
            else next = CC4B_PST_END
        return RES_OK;
    }

    if (this == CC4B_PST_TAPEINC) {
        _tapemodify(state, ch);
    } else if (state->type != CC4B_PST_TAPEINC) {
        Result res = _tapeflush(state);
        if (res) return res;
    }

    if (this == CC4B_PST_IO) {
        CC4BIRNodeKind kind = ch == ',' ? CC4B_IK_GET : CC4B_IK_PUT;
        CC4BIRNode *node = cc4b_irnodenew(state->this, kind);
        if (node == NULL) return RES_ERR;

        node->data.offset = 0;
        state->this = node;
        next = CC4B_PST_IO;
    } else if (this == CC4B_PST_OPEN) {
        // TODO: complete this

        CC4BIRNode *node = cc4b_irnodenew(state->this, CC4B_IK_LOOP);
        if (node == NULL) return RES_ERR;

        node->data.offset = 0;
        state->this = node;
    }

    return RES_OK;
}