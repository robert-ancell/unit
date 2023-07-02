#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *keys;
} UtX11KeyClass;

static void ut_x11_key_class_cleanup(UtObject *object) {
  UtX11KeyClass *self = (UtX11KeyClass *)object;
  ut_object_unref(self->keys);
}

static UtObjectInterface object_interface = {
    .type_name = "UtX11KeyClass", .cleanup = ut_x11_key_class_cleanup};

UtObject *ut_x11_key_class_new(UtObject *keys) {
  UtObject *object = ut_object_new(sizeof(UtX11KeyClass), &object_interface);
  UtX11KeyClass *self = (UtX11KeyClass *)object;
  self->keys = ut_object_ref(keys);
  return object;
}

bool ut_object_is_x11_key_class(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
