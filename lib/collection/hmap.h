#ifndef HMAP_YCC_H
#define HMAP_YCC_H

#include <stdint.h>
#include <stddef.h>
#include "result.h"

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t) -1)
#endif

#define HMAP_MINCAP 16

#define HTAG_EMPTY      ((HashEntryTag) 0)
#define HTAG_USED       ((HashEntryTag) 1)
#define HTAG_DEAD       ((HashEntryTag) 2)

typedef uint8_t HashEntryTag;

typedef uint32_t (*HashFunc)(const void *value);
typedef Result (*HashAssertFunc)(const void *left, const void *right);

size_t rpow2(size_t t);

#endif