#ifndef UHMAP_YCC_H
#define UHMAP_YCC_H

#include <stdint.h>
#include <stddef.h>
#include "result.h"
#include "hmap.h"

typedef struct {
    void *key;
    void *value;
    HashEntryTag tag;
} UnorderedHashEntry;

typedef struct {
    UnorderedHashEntry *entries;
    size_t capacity;
    size_t length;

    HashFunc hash;
    HashAssertFunc as;
} UnorderedHashMap;

UnorderedHashMap *uhmapnew(size_t cap, HashFunc hash, HashAssertFunc as);
void uhmapkill(UnorderedHashMap *uhmap);
size_t uhmapindex(UnorderedHashMap *uhmap, void *key, uint8_t tag_mask);
UnorderedHashEntry *uhmapentry(UnorderedHashMap *uhmap, void *key, uint8_t tag_mask);
void *uhmapat(UnorderedHashMap *uhmap, void *key);
Result uhmapdel(UnorderedHashMap *uhmap, void *key);
Result uhmapres(UnorderedHashMap *uhmap, size_t cap);
Result uhmapset(UnorderedHashMap *uhmap, void *key, void *value);
UnorderedHashMap *uhmapdup(UnorderedHashMap *origin);

#endif