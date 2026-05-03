#include <stdlib.h>
#include "parse.h"

#include <stdio.h>

static const CC4BParseStateType _state_map[256] = {
    ['+'] = CC4B_PST_TAPE,
    [','] = CC4B_PST_IO,
    ['-'] = CC4B_PST_TAPE,
    ['.'] = CC4B_PST_IO,
    ['>'] = CC4B_PST_TAPE,
    ['<'] = CC4B_PST_TAPE,
    ['['] = CC4B_PST_OPEN,
    [']'] = CC4B_PST_END,
};

static uint32_t _offset_hash(const void *value) {
    return (uint32_t) (*(const ptrdiff_t *) value);
}
static Result _offset_as(const void *left, const void *right) {
    ptrdiff_t
        left_val = *(const ptrdiff_t *) left,
        right_val = *(const ptrdiff_t *) right;
    return left_val != right_val;
}

static void _tapemodify(CC4BParseState *state, unsigned char ch) {
    if (state->type != CC4B_PST_TAPE) {
        state->ctx.tape.pending = fhmapnew(
            sizeof(ptrdiff_t),
            sizeof(CC4BTapeSlot),
            8,
            _offset_hash,
            _offset_as
        );
        state->ctx.tape.offset = 0;
    }

    switch (ch) {
    case '+':
    case '-': {
        CC4BTapeSlot slot;
        Result res = fhmapat(
            state->ctx.tape.pending,
            &state->ctx.tape.offset,
            &slot
        );
        if (res) {
            slot.type = CC4B_TST_INC;
            slot.value = 0;
        }

        slot.value += ch == '+' ? 1 : -1;
        if (slot.value != 0) fhmapset(
            state->ctx.tape.pending,
            &state->ctx.tape.offset,
            &slot
        ); else fhmapdel(
            state->ctx.tape.pending,
            &state->ctx.tape.offset
        );
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
    CC4BIRNode
        *node_ti = NULL,
        *node_mov = NULL;

    if (state->ctx.tape.pending->length > 0) {
        node_ti = cc4b_irnodenew(state->this, CC4B_IK_TAPEINC);
        if (node_ti == NULL) goto err;

        node_ti->data.pending = state->ctx.tape.pending;
        state->this = node_ti;
    }

    if (state->ctx.tape.offset != 0) {
        node_mov = cc4b_irnodenew(state->this, CC4B_IK_MOV);
        if (node_mov == NULL) goto err_ti;

        node_mov->data.offset = state->ctx.tape.offset;
        state->this = node_mov;
    }

    return RES_OK;

err_ti:
    cc4b_irnodekill(node_ti);
err:
    return RES_ERR;
}

static Result _bracketpush(CC4BParseState *state) {
    CC4BIRNode *node_h = cc4b_irnodenew(state->this, CC4B_IK_NOP);
    if (node_h == NULL) goto err;

    CC4BIRNode *node_l = cc4b_irnodenew(state->this, CC4B_IK_LOOP);
    if (node_l == NULL) goto err_h;

    Result res = vecpush(state->brackets, &node_l);
    if (res) goto err_l;
    node_l->data.block = node_h;
    state->this = node_h;

    return RES_OK;

err_l:
    cc4b_irnodekill(node_l);
err_h:
    cc4b_irnodekill(node_h);
err:
    return RES_ERR;
}

static Result _bracketpop(CC4BParseState *state) {
    CC4BIRNode *node_l;
    Result res = vecpop(state->brackets, &node_l);
    if (res) return RES_ERR;

    state->this = node_l;

    CC4BIRNode *node_h = node_l->data.block;
    if (node_h->kind != CC4B_IK_NOP) return RES_ERR;

    node_l->data.block = node_h->next;
    free(node_h);

    return RES_OK;
}

Result cc4b_parseinit(CC4BParseState *state) {
    Vector *brackets = vecnew(sizeof(CC4BIRNode *), 8);
    if (brackets == NULL) goto err;

    CC4BIRNode *head = cc4b_irnodenew(NULL, CC4B_IK_NOP);
    if (head == NULL) goto err_brackets;

    state->head = head;
    state->this = head;
    state->brackets = brackets;
    state->type = CC4B_PST_NOP;

    return RES_OK;

err_brackets:
    veckill(brackets);
err:
    return RES_ERR;
}

Result cc4b_parsestep(CC4BParseState *state, unsigned char ch) {
    CC4BParseStateType
        this = _state_map[ch],
        next = state->type;

    if (this == CC4B_PST_NOP)
        return RES_OK;
    if (state->type == CC4B_PST_IGNORE) {
        if (this == CC4B_PST_OPEN)
            state->ctx.skip_depth++;
        else if (this == CC4B_PST_END)
            if (state->ctx.skip_depth != 0) state->ctx.skip_depth--;
            else next = CC4B_PST_END;
        return RES_OK;
    }

    if (this != CC4B_PST_TAPE && state->type == CC4B_PST_TAPE) {
        Result res = _tapeflush(state);
        if (res) return res;
    }

    if (this == CC4B_PST_TAPE) {
        _tapemodify(state, ch);
        next = CC4B_PST_TAPE;
    } else if (this == CC4B_PST_IO) {
        CC4BIRNodeKind kind = ch == ',' ? CC4B_IK_GET : CC4B_IK_PUT;
        CC4BIRNode *node = cc4b_irnodenew(state->this, kind);
        if (node == NULL) return RES_ERR;

        node->data.offset = 0;
        state->this = node;
        next = CC4B_PST_IO;
    } else if (this == CC4B_PST_OPEN) {
        Result res = _bracketpush(state);
        if (res) return res;
        next = CC4B_PST_OPEN;
    } else if (this == CC4B_PST_END) {
        Result res = _bracketpop(state);
        if (res) return res;
        next = CC4B_PST_END;
    }

    state->type = next;
    return RES_OK;
}

CC4BIRNode *cc4b_parseend(CC4BParseState *state) {
    if (state->type == CC4B_PST_TAPE) {
        Result res = _tapeflush(state);
        if (res) return NULL;
    }

    if (state->brackets->length > 0)
        return NULL;
    veckill(state->brackets);

    CC4BIRNode *next = state->head->next;
    free(state->head);
    state->head = NULL;
    return next;
}