#include <stdlib.h>
#include <string.h>
#include "ohmap.h"

OrderedHashMap *ohmapnew(size_t cap, HashFunc hash, HashAssertFunc as) {
    OrderedHashMap *ohmap = malloc(sizeof(OrderedHashMap));
    if (ohmap == NULL) goto err;

    cap = cap < HMAP_MINCAP ? HMAP_MINCAP : rpow2(cap);
    void *entries = calloc(cap, sizeof(OrderedHashEntry));
    if (entries == NULL) goto err_ohmap;

    ohmap->entries = entries;
    ohmap->capacity = cap;
    ohmap->length = 0;
    ohmap->head = SIZE_MAX;
    ohmap->last = SIZE_MAX;
    ohmap->hash = hash;
    ohmap->as = as;

    return ohmap;

err_ohmap:
    free(ohmap);
err:
    return NULL;
}

void ohmapkill(OrderedHashMap *ohmap) {
    if (ohmap == NULL) return;

    free(ohmap->entries);
    free(ohmap);
}

size_t ohmapindex(OrderedHashMap *ohmap, void *key, uint8_t tag_mask) {
    uint32_t hash = ohmap->hash(key);

    size_t index = hash % ohmap->capacity;
    size_t start = index;
    for (;;) {
        OrderedHashEntry entry = ohmap->entries[index];

        if (1 << entry.tag & tag_mask) break;
        if (entry.tag == HTAG_USED && !ohmap->as(key, entry.key)) break;

        index = (index + 1) % ohmap->capacity;
        if (index == start) return SIZE_MAX;
    }

    return index;
}

OrderedHashEntry *ohmapentry(OrderedHashMap *ohmap, void *key, uint8_t tag_mask) {
    size_t index = ohmapindex(ohmap, key, tag_mask);
    return index == SIZE_MAX ? NULL : ohmap->entries + index;
}

void *ohmapat(OrderedHashMap *ohmap, void *key) {
    OrderedHashEntry *entry = ohmapentry(ohmap, key, 1 << HTAG_EMPTY);
    return entry == NULL ? NULL : entry->value;
}

Result ohmapdel(OrderedHashMap *ohmap, void *key) {
    OrderedHashEntry *entry = ohmapentry(ohmap, key, 1 << HTAG_EMPTY);
    if (entry == NULL) return RES_ERR;

    if (entry->tag != HTAG_USED) return RES_OK;

    OrderedHashEntry *neighbour;
    if (entry->prev == SIZE_MAX) {
        ohmap->head = entry->next;
    } else {
        neighbour = ohmap->entries + entry->prev;
        neighbour->next = entry->next;
    }
    if (entry->next == SIZE_MAX) {
        ohmap->last = entry->prev;
    } else {
        neighbour = ohmap->entries + entry->next;
        neighbour->prev = entry->prev;
    }

    entry->key = NULL;
    entry->value = NULL;
    entry->prev = 0;
    entry->next = 0;
    entry->tag = HTAG_DEAD;
    ohmap->length--;
    return RES_OK;
}

static size_t _ohmapinsert(OrderedHashMap *ohmap, void *key, void *value) {
    size_t index = ohmapindex(ohmap, key, 1 << HTAG_EMPTY | 1 << HTAG_DEAD);
    if (index == SIZE_MAX) return SIZE_MAX;
    OrderedHashEntry *entry = ohmap->entries + index;

    entry->value = value;
    if (entry->tag == HTAG_USED) return index;

    entry->key = key;
    entry->tag = HTAG_USED;

    if (ohmap->last != SIZE_MAX)
        (ohmap->entries + ohmap->last)->next = index;
    if (ohmap->head == SIZE_MAX)
        ohmap->head = index;

    entry->next = SIZE_MAX;
    entry->prev = ohmap->last;
    ohmap->last = index;

    return index;
}

Result ohmapres(OrderedHashMap *ohmap, size_t cap) {
    cap = rpow2(cap);
    if (cap <= ohmap->capacity) return RES_ERR;

    size_t old_cap = ohmap->capacity;

    void *entries = realloc(ohmap->entries, cap * sizeof(OrderedHashEntry));
    if (entries == NULL) return RES_ERR;

    ohmap->entries = entries;
    ohmap->capacity = cap;
    memset((uint8_t *) entries + old_cap * sizeof(OrderedHashEntry), 0, (cap - old_cap) * sizeof(OrderedHashEntry));

    OrderedHashEntry
        *last = NULL,
        *entry = ohmap->entries + ohmap->head;
    for (size_t count = 0; count < ohmap->length; count++) {
        uint32_t hash = ohmap->hash(entry->key);
        size_t next = entry->next;
        if (hash % old_cap != hash % cap) {
            void *key = entry->key;
            void *value = entry->value;

            entry->key = NULL;
            entry->value = NULL;
            entry->prev = 0;
            entry->next = 0;
            entry->tag = HTAG_DEAD;
            size_t index = _ohmapinsert(ohmap, key, value);

            if (last != NULL) last->next = index;
        }

        last = entry;
        if (next == SIZE_MAX) break;
        entry = ohmap->entries + next;
    }

    return RES_OK;
}

static Result _ohmapexpand(OrderedHashMap *ohmap) {
    if (ohmap->length < ohmap->capacity / 4 * 3) return RES_OK;

    size_t cap = ohmap->capacity * 2;
    if (cap < HMAP_MINCAP) cap = HMAP_MINCAP;
    return ohmapres(ohmap, cap);
}

Result ohmapset(OrderedHashMap *ohmap, void *key, void *value) {
    Result res = _ohmapexpand(ohmap);
    if (res) return res;

    size_t index = _ohmapinsert(ohmap, key, value);
    if (index == SIZE_MAX) return RES_ERR;
    ohmap->length++;

    return RES_OK;
}

OrderedHashMap *ohmapdup(OrderedHashMap *origin) {
    OrderedHashMap *dup = ohmapnew(
        origin->capacity,
        origin->hash,
        origin->as
    );
    if (dup == NULL) return NULL;

    memcpy(dup->entries, origin->entries, origin->capacity * sizeof(OrderedHashEntry));
    dup->length = origin->length;
    dup->head = origin->head;
    dup->last = origin->last;

    return dup;
}
