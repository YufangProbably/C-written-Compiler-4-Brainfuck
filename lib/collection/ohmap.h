#ifndef OHMAP_YCC_H
#define OHMAP_YCC_H

#include <stdint.h>
#include <stddef.h>
#include "result.h"
#include "hmap.h"

typedef struct {
    void *key;
    void *value;
    size_t prev;
    size_t next;
    HashEntryTag tag;
} OrderedHashEntry;

typedef struct {
    OrderedHashEntry *entries;
    size_t capacity;
    size_t length;
    size_t head;
    size_t last;

    HashFunc hash;
    HashAssertFunc as;
} OrderedHashMap;

OrderedHashMap *ohmapnew(size_t cap, HashFunc hash, HashAssertFunc as);
void ohmapkill(OrderedHashMap *ohmap);
size_t ohmapindex(OrderedHashMap *ohmap, void *key, uint8_t tag_mask);
OrderedHashEntry *ohmapentry(OrderedHashMap *ohmap, void *key, uint8_t tag_mask);
void *ohmapat(OrderedHashMap *ohmap, void *key);
Result ohmapdel(OrderedHashMap *ohmap, void *key);
Result ohmapres(OrderedHashMap *ohmap, size_t cap);
Result ohmapset(OrderedHashMap *ohmap, void *key, void *value);
OrderedHashMap *ohmapdup(OrderedHashMap *origin);

#endif