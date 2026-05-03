#ifdef CC4B_DEBUG_DUMP

#include <stdio.h>
#include <stdlib.h>
#include "collection/fhmap.h"
#include "parse.h"
#include "dump.h"

static const char *_opname_map[256] = {
    [0x01] = "INC",
    [0x02] = "SET",
    [0x03] = "ADD",
    [0x04] = "MAD",
    [0x84] = "MAD16",
    [0x05] = "SAD",
    [0x06] = "XMUL",
    [0x07] = "LCP",
    [0x08] = "RCP",
    [0x09] = "MUL",
    [0x0A] = "SHL",
    [0x10] = "JZ",
    [0x11] = "JZ16",
    [0x90] = "JZ32",
    [0x12] = "JNZ",
    [0x13] = "JNZ16",
    [0x92] = "JNZ32",
    [0x20] = "MOV",
    [0x21] = "MOV16",
    [0xA0] = "MOV32",
    [0x22] = "SEEK",
    [0xA2] = "SEEK16",
    [0x30] = "GET",
    [0x31] = "GETS",
    [0xB1] = "GETS16",
    [0x32] = "PUT",
    [0x33] = "PUTC",
    [0x34] = "PUTS",
    [0xB4] = "PUTS16",
    [0x70] = "ERR",
    [0x71] = "ERP"
};

void cc4b_irnodedump(CC4BIRNode *head, size_t indent) {
    CC4BIRNode *this = head, *next;
    while (this != NULL) {
        next = this->next;

        printf("%*s", indent, "");
        if (this->kind == CC4B_IK_NOP) {
            printf("NOP\n");
        } else if (this->kind == CC4B_IK_LOOP) {
            printf("LOOP:\n");
            cc4b_irnodedump(this->data.block, indent + 2);
        } else if (this->kind == CC4B_IK_TAPEINC) {
            printf("TAPEINC:\n");
            FlatHashEntryView eview;
            for (size_t i = 0; i < this->data.pending->capacity; i++) {
                fhmapentry_by(this->data.pending, i, &eview);
                if (*eview.tag != HTAG_USED) continue;

                CC4BTapeSlot slot = *(CC4BTapeSlot *) eview.value;
                printf(
                    "%*s[%d] %s %d\n",
                    indent + 2, "",
                    *(ptrdiff_t *) eview.key,
                    slot.type == CC4B_TST_INC ? "+=" : "=",
                    slot.value
                );
            }
        } else if (this->kind == CC4B_IK_MOV) {
            printf("MOV [%d]\n", this->data.offset);
        } else if (this->kind == CC4B_IK_MAD) {
            printf(
                "MAD [%d] [%d] %d\n",
                this->data.mad.source,
                this->data.mad.target,
                this->data.mad.mult
            );
        } else if (this->kind == CC4B_IK_GET) {
            printf("GET [%d]\n", this->data.offset);
        } else if (this->kind == CC4B_IK_PUT) {
            printf("PUT [%d]\n", this->data.offset);
        } else if (this->kind == CC4B_IK_PATTERN) {
            printf(
                "PATTERN %s %d %d %d %d\n",
                _opname_map[this->data.pattern.code],
                this->data.pattern.a,
                this->data.pattern.b,
                this->data.pattern.c,
                this->data.pattern.d
            );
        }

        this = next;
    }
}

#endif