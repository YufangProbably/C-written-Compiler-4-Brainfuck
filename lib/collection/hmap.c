#include <stddef.h>
#include "hmap.h"

size_t rpow2(size_t t) {
    if (t == 0) return 1;
    if ((t & t - 1) == 0) return t;

    t--;
    t |= t >>  1;
    t |= t >>  2;
    t |= t >>  4;
    t |= t >>  8;
    t |= t >> 16;
    t |= t >> 32;
    t++;

    return t;
}