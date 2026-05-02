#include <stdlib.h>
#include <string.h>
#include "uhmap.h"

UnorderedHashMap *uhmapnew(size_t cap, HashFunc hash, HashAssertFunc as) {
    UnorderedHashMap *uhmap = malloc(sizeof(UnorderedHashMap));
    if (uhmap == NULL) goto err;

    cap = cap < HMAP_MINCAP ? HMAP_MINCAP : rpow2(cap);
    void *entries = calloc(cap, sizeof(UnorderedHashEntry));
    if (entries == NULL) goto err_uhmap;

    uhmap->entries = entries;
    uhmap->capacity = cap;
    uhmap->length = 0;
    uhmap->hash = hash;
    uhmap->as = as;

    return uhmap;

err_uhmap:
    free(uhmap);
err:
    return NULL;
}

void uhmapkill(UnorderedHashMap *uhmap) {
    if (uhmap == NULL) return;

    free(uhmap->entries);
    free(uhmap);
}

size_t uhmapindex(UnorderedHashMap *uhmap, void *key, uint8_t tag_mask) {
    uint32_t hash = uhmap->hash(key);

    size_t index = hash % uhmap->capacity;
    size_t start = index;
    for (;;) {
        UnorderedHashEntry entry = uhmap->entries[index];

        if (1 << entry.tag & tag_mask) break;
        if (entry.tag == HTAG_USED && !uhmap->as(key, entry.key)) break;

        index = (index + 1) % uhmap->capacity;
        if (index == start) return SIZE_MAX;
    }

    return index;
}

UnorderedHashEntry *uhmapentry(UnorderedHashMap *uhmap, void *key, uint8_t tag_mask) {
    size_t index = uhmapindex(uhmap, key, tag_mask);
    return index == SIZE_MAX ? NULL : uhmap->entries + index;
}

void *uhmapat(UnorderedHashMap *uhmap, void *key) {
    UnorderedHashEntry *entry = uhmapentry(uhmap, key, 1 << HTAG_EMPTY);
    return entry == NULL ? NULL : entry->value;
}

Result uhmapdel(UnorderedHashMap *uhmap, void *key) {
    UnorderedHashEntry *entry = uhmapentry(uhmap, key, 1 << HTAG_EMPTY);
    if (entry == NULL) return RES_ERR;

    if (entry->tag != HTAG_USED) return RES_OK;

    entry->key = NULL;
    entry->value = NULL;
    entry->tag = HTAG_DEAD;
    uhmap->length--;
    return RES_OK;
}

static size_t _uhmapinsert(UnorderedHashMap *uhmap, void *key, void *value) {
    size_t index = uhmapindex(uhmap, key, 1 << HTAG_EMPTY | 1 << HTAG_DEAD);
    if (index == SIZE_MAX) return SIZE_MAX;
    UnorderedHashEntry *entry = uhmap->entries + index;

    entry->key = key;
    entry->value = value;
    entry->tag = HTAG_USED;

    return index;
}

Result uhmapres(UnorderedHashMap *uhmap, size_t cap) {
    cap = rpow2(cap);
    if (cap <= uhmap->capacity) return RES_ERR;

    size_t old_cap = uhmap->capacity;

    void *entries = realloc(uhmap->entries, cap * sizeof(UnorderedHashEntry));
    if (entries == NULL) return RES_ERR;

    uhmap->entries = entries;
    uhmap->capacity = cap;
    memset((uint8_t *) entries + old_cap * sizeof(UnorderedHashEntry), 0, (cap - old_cap) * sizeof(UnorderedHashEntry));

    for (size_t i = 0; i < old_cap; i++) {
        UnorderedHashEntry *entry = uhmap->entries + i;
        if (entry->tag != HTAG_USED) continue;

        void *key = entry->key;
        void *value = entry->value;

        entry->key = NULL;
        entry->value = NULL;
        entry->tag = HTAG_DEAD;
        size_t index = _uhmapinsert(uhmap, key, value);

        if (index == SIZE_MAX) return RES_ERR;
    }

    return RES_OK;
}

static Result _uhmapexpand(UnorderedHashMap *uhmap) {
    if (uhmap->length < uhmap->capacity / 4 * 3) return RES_OK;

    size_t cap = uhmap->capacity * 2;
    if (cap < HMAP_MINCAP) cap = HMAP_MINCAP;
    return uhmapres(uhmap, cap);
}

Result uhmapset(UnorderedHashMap *uhmap, void *key, void *value) {
    Result res = _uhmapexpand(uhmap);
    if (res) return res;

    size_t index = _uhmapinsert(uhmap, key, value);
    if (index == SIZE_MAX) return RES_ERR;
    uhmap->length++;

    return RES_OK;
}

UnorderedHashMap *uhmapdup(UnorderedHashMap *origin) {
    UnorderedHashMap *dup = uhmapnew(
        origin->capacity,
        origin->hash,
        origin->as
    );
    if (dup == NULL) return NULL;

    memcpy(dup->entries, origin->entries, origin->capacity * sizeof(UnorderedHashEntry));
    dup->length = origin->length;

    return dup;
}