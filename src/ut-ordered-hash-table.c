#include <assert.h>
#include <stdlib.h>

#include "ut-map-private.h"
#include "ut.h"

typedef struct _UtOrderedHashTableItem UtOrderedHashTableItem;

typedef struct {
  UtObject object;
  UtOrderedHashTableItem *items;
} UtOrderedHashTable;

struct _UtOrderedHashTableItem {
  UtObject object;
  UtObject *key;
  UtObject *value;
  UtOrderedHashTableItem *next;
};

static UtObject *ut_ordered_hash_table_item_get_key(UtObject *object) {
  UtOrderedHashTableItem *self = (UtOrderedHashTableItem *)object;
  return self->key;
}

static UtObject *ut_ordered_hash_table_item_get_value(UtObject *object) {
  UtOrderedHashTableItem *self = (UtOrderedHashTableItem *)object;
  return self->value;
}

static UtMapItemInterface map_item_interface = {
    .get_key = ut_ordered_hash_table_item_get_key,
    .get_value = ut_ordered_hash_table_item_get_value};

static void ut_ordered_hash_table_item_cleanup(UtObject *object) {
  UtOrderedHashTableItem *self = (UtOrderedHashTableItem *)object;
  ut_object_unref(self->key);
  ut_object_unref(self->value);
  self->next = NULL;
}

static UtObjectInterface item_object_interface = {
    .type_name = "UtOrderedHashTableItem",
    .cleanup = ut_ordered_hash_table_item_cleanup,
    .interfaces = {{&ut_map_item_id, &map_item_interface}, {NULL, NULL}}};

static UtOrderedHashTableItem *item_new(UtObject *key, UtObject *value) {
  UtOrderedHashTableItem *item = (UtOrderedHashTableItem *)ut_object_new(
      sizeof(UtOrderedHashTableItem), &item_object_interface);
  item->key = ut_object_ref(key);
  item->value = ut_object_ref(value);
  return item;
}

static UtOrderedHashTableItem *lookup(UtOrderedHashTable *self, UtObject *key,
                                      UtOrderedHashTableItem **prev_item) {
  UtOrderedHashTableItem *prev_item_ = NULL;
  for (UtOrderedHashTableItem *item = self->items; item != NULL;
       item = item->next) {
    if (ut_object_equal(item->key, key)) {
      if (prev_item != NULL) {
        *prev_item = prev_item_;
      }

      return item;
    }
    prev_item_ = item;
  }
  return NULL;
}

size_t ut_ordered_hash_table_get_length(UtObject *object) {
  UtOrderedHashTable *self = (UtOrderedHashTable *)object;
  size_t length = 0;
  for (UtOrderedHashTableItem *item = self->items; item != NULL;
       item = item->next) {
    length++;
  }
  return length;
}

static void ut_ordered_hash_table_insert(UtObject *object, UtObject *key,
                                         UtObject *value) {
  UtOrderedHashTable *self = (UtOrderedHashTable *)object;

  UtOrderedHashTableItem *prev_item;
  UtOrderedHashTableItem *existing_item = lookup(self, key, &prev_item);

  UtOrderedHashTableItem *item = item_new(key, value);
  if (existing_item == NULL) {
    item->next = self->items;
    self->items = item;
  } else {
    item->next = existing_item->next;
    if (prev_item != NULL) {
      prev_item->next = item;
    } else {
      self->items = item;
    }

    existing_item->next = NULL;
    ut_object_unref((UtObject *)existing_item);
  }
}

static UtObject *ut_ordered_hash_table_lookup(UtObject *object, UtObject *key) {
  UtOrderedHashTable *self = (UtOrderedHashTable *)object;
  UtOrderedHashTableItem *item = lookup(self, key, NULL);
  return item != NULL ? item->value : NULL;
}

static void ut_ordered_hash_table_remove(UtObject *object, UtObject *key) {
  UtOrderedHashTable *self = (UtOrderedHashTable *)object;
  UtOrderedHashTableItem *prev_item;
  UtOrderedHashTableItem *item = lookup(self, key, &prev_item);
  if (item == NULL) {
    return;
  }
  if (prev_item != NULL) {
    prev_item->next = item->next;
  } else {
    self->items = item->next;
  }

  item->next = NULL;
  ut_object_unref((UtObject *)item);
}

static UtObject *ut_ordered_hash_table_get_items(UtObject *object) {
  UtOrderedHashTable *self = (UtOrderedHashTable *)object;
  UtObject *items = ut_object_array_new();
  for (UtOrderedHashTableItem *item = self->items; item != NULL;
       item = item->next) {
    ut_list_prepend(items, (UtObject *)item);
  }
  return items;
}

static UtObject *ut_ordered_hash_table_get_keys(UtObject *object) {
  UtOrderedHashTable *self = (UtOrderedHashTable *)object;
  UtObject *keys = ut_object_array_new();
  for (UtOrderedHashTableItem *item = self->items; item != NULL;
       item = item->next) {
    ut_list_prepend(keys, item->key);
  }
  return keys;
}

static UtObject *ut_ordered_hash_table_get_values(UtObject *object) {
  UtOrderedHashTable *self = (UtOrderedHashTable *)object;
  UtObject *values = ut_object_array_new();
  for (UtOrderedHashTableItem *item = self->items; item != NULL;
       item = item->next) {
    ut_list_prepend(values, item->value);
  }
  return values;
}

static UtMapInterface map_interface = {
    .get_length = ut_ordered_hash_table_get_length,
    .insert = ut_ordered_hash_table_insert,
    .lookup = ut_ordered_hash_table_lookup,
    .remove = ut_ordered_hash_table_remove,
    .get_items = ut_ordered_hash_table_get_items,
    .get_keys = ut_ordered_hash_table_get_keys,
    .get_values = ut_ordered_hash_table_get_values};

static void ut_ordered_hash_table_cleanup(UtObject *object) {
  UtOrderedHashTable *self = (UtOrderedHashTable *)object;
  UtOrderedHashTableItem *next_item;
  for (UtOrderedHashTableItem *item = self->items; item != NULL;
       item = next_item) {
    next_item = item->next;
    item->next = NULL;
    ut_object_unref((UtObject *)item);
  }
  self->items = NULL;
}

static UtObjectInterface object_interface = {
    .type_name = "UtOrderedHashTable",
    .to_string = _ut_map_to_string,
    .equal = _ut_map_equal,
    .cleanup = ut_ordered_hash_table_cleanup,
    .interfaces = {{&ut_map_id, &map_interface}, {NULL, NULL}}};

UtObject *ut_ordered_hash_table_new() {
  return ut_object_new(sizeof(UtOrderedHashTable), &object_interface);
}

bool ut_object_is_ordered_hash_table_string(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
