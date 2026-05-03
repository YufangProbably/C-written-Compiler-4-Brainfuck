#ifndef FHMAP_YCC_H
#define FHMAP_YCC_H

#include <stdint.h>
#include <stddef.h>
#include "result.h"
#include "hmap.h"

typedef struct {
    void *key;
    void *value;
    HashEntryTag *tag;
    size_t index;
} FlatHashEntryView;

typedef struct {
    void *entries;
    size_t capacity;
    size_t length;
    size_t key_size;
    size_t value_size;

    HashFunc hash;
    HashAssertFunc as;
} FlatHashMap;

FlatHashMap *fhmapnew(size_t key_size, size_t value_size, size_t cap, HashFunc hash, HashAssertFunc as);
void fhmapkill(FlatHashMap *fhmap);
Result fhmapentry_by(FlatHashMap *fhmap, size_t index, FlatHashEntryView *eview);
Result fhmapentry(FlatHashMap *fhmap, void *key, FlatHashEntryView *eview, uint8_t tag_mask);
size_t fhmapindex(FlatHashMap *fhmap, void *key, uint8_t tag_mask);
Result fhmapat(FlatHashMap *fhmap, void *key, void *value);
Result fhmapdel(FlatHashMap *fhmap, void *key);
Result fhmapres(FlatHashMap *fhmap, size_t cap);
Result fhmapset(FlatHashMap *fhmap, void *key, void *value);
FlatHashMap *fhmapdup(FlatHashMap *ori);

#endif