#include <stdlib.h>
#include <string.h>
#include "fhmap.h"

FlatHashMap *fhmapnew(size_t key_size, size_t value_size, size_t cap, HashFunc hash, HashAssertFunc as) {
    FlatHashMap *fhmap = malloc(sizeof(FlatHashMap));
    if (fhmap == NULL) goto err;

    cap = cap < HMAP_MINCAP ? HMAP_MINCAP : rpow2(cap);
    size_t entry_size = key_size + value_size + sizeof(HashEntryTag);
    void *entries = calloc(cap, entry_size);
    if (entries == NULL) goto err_fhmap;

    fhmap->entries = entries;
    fhmap->capacity = cap;
    fhmap->length = 0;
    fhmap->key_size = key_size;
    fhmap->value_size = value_size;
    fhmap->hash = hash;
    fhmap->as = as;

    return fhmap;

err_fhmap:
    free(fhmap);
err:
    return NULL;
}

void fhmapkill(FlatHashMap *fhmap) {
    if (fhmap == NULL) return;

    free(fhmap->entries);
    free(fhmap);
}

Result fhmapentry_by(FlatHashMap *fhmap, size_t index, FlatHashEntryView *eview) {
    if (index >= fhmap->capacity) return RES_ERR;

    size_t kvpair_size = fhmap->key_size + fhmap->value_size;
    size_t entry_size = kvpair_size + sizeof(HashEntryTag);
    uint8_t *entry = (uint8_t *) fhmap->entries + (index * entry_size);

    eview->key = entry;
    eview->value = entry + fhmap->key_size;
    eview->tag = (HashEntryTag *) (entry + kvpair_size);

    eview->index = index;

    return RES_OK;
}

Result fhmapentry(FlatHashMap *fhmap, void *key, FlatHashEntryView *eview, uint8_t tag_mask) {
    uint32_t hash = fhmap->hash(key);

    size_t index = hash % fhmap->capacity;
    size_t start = index;
    for (;;) {
        Result res = fhmapentry_by(fhmap, index, eview);
        if (res) return res;

        if (1 << *eview->tag & tag_mask) break;
        if (*eview->tag == HTAG_USED && !fhmap->as(key, eview->key)) break;

        index = (index + 1) % fhmap->capacity;
        if (index == start) return RES_ERR;
    }

    return RES_OK;
}

size_t fhmapindex(FlatHashMap *fhmap, void *key, uint8_t tag_mask) {
    FlatHashEntryView eview;
    Result res = fhmapentry(fhmap, key, &eview, tag_mask);
    return res ? SIZE_MAX : eview.index;
}

Result fhmapat(FlatHashMap *fhmap, void *key, void *value) {
    FlatHashEntryView eview;

    Result res = fhmapentry(fhmap, key, &eview, 1 << HTAG_EMPTY);
    if (res) return res;

    if (*eview.tag != HTAG_USED) return RES_ERR;
    memcpy(value, eview.value, fhmap->value_size);
    return RES_OK;
}

Result fhmapdel(FlatHashMap *fhmap, void *key) {
    FlatHashEntryView eview;

    Result res = fhmapentry(fhmap, key, &eview, 1 << HTAG_EMPTY);
    if (res) return res;

    if (*eview.tag != HTAG_USED) return RES_OK;

    memset(eview.key, 0, fhmap->key_size);
    memset(eview.value, 0, fhmap->value_size);
    *eview.tag = HTAG_DEAD;
    fhmap->length--;
    return RES_OK;
}

static Result _fhmapinsert(FlatHashMap *fhmap, void *key, void *value) {
    FlatHashEntryView eview;
    Result res;

    res = fhmapentry(fhmap, key, &eview, 1 << HTAG_EMPTY | 1 << HTAG_DEAD);
    if (res) return res;

    if (*eview.tag != HTAG_USED) {
        memcpy(eview.key, key, fhmap->key_size);
        *eview.tag = HTAG_USED;
    }
    memcpy(eview.value, value, fhmap->value_size);

    return RES_OK;
}

Result fhmapres(FlatHashMap *fhmap, size_t cap) {
    cap = rpow2(cap);
    if (cap <= fhmap->capacity) goto err;

    size_t old_cap = fhmap->capacity;

    size_t entry_size = fhmap->key_size + fhmap->value_size + sizeof(HashEntryTag);
    void *entries = realloc(fhmap->entries, cap * entry_size);
    if (entries == NULL) goto err;

    fhmap->entries = entries;
    fhmap->capacity = cap;
    memset((uint8_t *) entries + old_cap * entry_size, 0, (cap - old_cap) * entry_size);

    void *key = malloc(fhmap->key_size);
    if (key == NULL) goto err;

    void *value = malloc(fhmap->value_size);
    if (value == NULL) goto err_key;

    FlatHashEntryView eview;
    for (size_t i = 0; i < fhmap->capacity; i++) {
        fhmapentry_by(fhmap, i, &eview);
        if (*eview.tag != HTAG_USED) continue;

        uint32_t hash = fhmap->hash(eview.key);
        if (hash % old_cap == hash % cap) continue;

        memcpy(key, eview.key, fhmap->key_size);
        memcpy(value, eview.value, fhmap->value_size);

        memset(eview.key, 0, fhmap->key_size);
        memset(eview.value, 0, fhmap->value_size);
        *eview.tag = HTAG_DEAD;
        _fhmapinsert(fhmap, key, value);
    }

    free(key);
    free(value);

    return RES_OK;

err_key:
    free(key);
err:
    return RES_ERR;
}

static Result _fhmapexpand(FlatHashMap *fhmap) {
    if (fhmap->length < fhmap->capacity / 4 * 3) return RES_OK;

    size_t cap = fhmap->capacity * 2;
    if (cap < HMAP_MINCAP) cap = HMAP_MINCAP;
    return fhmapres(fhmap, cap);
}

Result fhmapset(FlatHashMap *fhmap, void *key, void *value) {
    Result res = _fhmapexpand(fhmap);
    if (res) return res;

    res = _fhmapinsert(fhmap, key, value);
    if (res) return res;
    fhmap->length++;

    return RES_OK;
}

FlatHashMap *fhmapdup(FlatHashMap *origin) {
    FlatHashMap *dup = fhmapnew(
        origin->key_size,
        origin->value_size,
        origin->capacity,
        origin->hash,
        origin->as
    );
    if (dup == NULL) return NULL;

    dup->length = origin->length;
    size_t entry_size = origin->key_size + origin->value_size + sizeof(HashEntryTag);
    memcpy(dup->entries, origin->entries, origin->capacity * entry_size);

    return dup;
}