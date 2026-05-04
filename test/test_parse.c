#define TEST_HERE
#include "test.h"

#include "debug/dump.h"
#include "parse.h"

TEST(parse_simple) {
    Result res;

    CC4BParseState state;
    res = cc4b_parseinit(&state);
    assert(res == RES_OK);

    const unsigned char *source = "+[[<+>->++>-<<]>+]";
    for (const unsigned char *ch = source; *ch != '\0'; ch++) {
        res = cc4b_parsestep(&state, *ch);
        assert(res == RES_OK);
    }
    CC4BIRNode *node = cc4b_parseend(&state);

    cc4b_irnodedump(node, 0);
    cc4b_irnodekill(node);
}