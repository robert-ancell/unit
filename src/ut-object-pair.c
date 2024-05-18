#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *item0;
  UtObject *item1;
} UtObjectPair;

static UtObject *get_element(UtObjectPair *self, size_t index) {
  return index == 0 ? self->item0 : self->item1;
}

size_t ut_object_pair_get_length(UtObject *object) { return 2; }

static UtObject *ut_object_pair_get_element_ref(UtObject *object,
                                                size_t index) {
  UtObjectPair *self = (UtObjectPair *)object;
  return ut_object_ref(get_element(self, index));
}

static UtObject *ut_object_pair_get_element(UtObject *object, size_t index) {
  UtObjectPair *self = (UtObjectPair *)object;
  return get_element(self, index);
}

static UtObject *ut_object_pair_get_key(UtObject *object) {
  UtObjectPair *self = (UtObjectPair *)object;
  return self->item0;
}

static UtObject *ut_object_pair_get_value(UtObject *object) {
  UtObjectPair *self = (UtObjectPair *)object;
  return self->item1;
}

static UtListInterface list_interface = {
    .is_mutable = false,
    .get_length = ut_object_pair_get_length,
    .get_element = ut_object_pair_get_element_ref};

static UtObjectListInterface object_list_interface = {
    .get_element = ut_object_pair_get_element};

static UtMapItemInterface map_item_interface = {
    .get_key = ut_object_pair_get_key, .get_value = ut_object_pair_get_value};

static void ut_object_pair_cleanup(UtObject *object) {
  UtObjectPair *self = (UtObjectPair *)object;
  ut_object_unref(self->item0);
  ut_object_unref(self->item1);
}

static UtObjectInterface object_interface = {
    .type_name = "UtObjectPair",
    .cleanup = ut_object_pair_cleanup,
    .interfaces = {{&ut_list_id, &list_interface},
                   {&ut_object_list_id, &object_list_interface},
                   {&ut_map_item_id, &map_item_interface},
                   {NULL, NULL}}};

UtObject *ut_object_pair_new(UtObject *item0, UtObject *item1) {
  UtObject *object = ut_object_new(sizeof(UtObjectPair), &object_interface);
  UtObjectPair *self = (UtObjectPair *)object;
  self->item0 = ut_object_ref(item0);
  self->item1 = ut_object_ref(item1);
  return object;
}

bool ut_object_is_object_pair_string(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
