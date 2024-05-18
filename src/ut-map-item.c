#include <assert.h>

#include "ut.h"

int ut_map_item_id = 0;

UtObject *ut_map_item_new(UtObject *key, UtObject *value) {
  return ut_object_pair_new(key, value);
}

UtObject *ut_map_item_get_key(UtObject *object) {
  UtMapItemInterface *map_item_interface =
      ut_object_get_interface(object, &ut_map_item_id);
  assert(map_item_interface != NULL);
  return map_item_interface->get_key(object);
}

UtObject *ut_map_item_get_value(UtObject *object) {
  UtMapItemInterface *map_item_interface =
      ut_object_get_interface(object, &ut_map_item_id);
  assert(map_item_interface != NULL);
  return map_item_interface->get_value(object);
}

bool ut_object_implements_map_item(UtObject *object) {
  return ut_object_get_interface(object, &ut_map_item_id) != NULL;
}
