#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

typedef void (*TestFunction)(void);

typedef struct {
    const char *name;
    TestFunction func;
} TestCase;

#define MAX_TESTS 256
extern TestCase test_regis[MAX_TESTS];
extern size_t test_count;

#define TEST(NAME) \
    void test_##NAME(void); \
    __attribute__((constructor)) void regis_##NAME(void) { \
        test_regis[test_count].name = #NAME; \
        test_regis[test_count].func = test_##NAME; \
        test_count++; \
    } \
    void test_##NAME(void)

#ifdef TEST_HERE
TestCase test_regis[MAX_TESTS];
size_t test_count = 0;

int main(void) {
    for (int i = 0; i < test_count; i++)
        test_regis[i].func();
    return 0;
}
#endif

#endif