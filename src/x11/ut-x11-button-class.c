#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *state;
  UtObject *labels;
} UtX11ButtonClass;

static void ut_x11_button_class_cleanup(UtObject *object) {
  UtX11ButtonClass *self = (UtX11ButtonClass *)object;
  ut_object_unref(self->state);
  ut_object_unref(self->labels);
}

static UtObjectInterface object_interface = {
    .type_name = "UtX11ButtonClass", .cleanup = ut_x11_button_class_cleanup};

UtObject *ut_x11_button_class_new(UtObject *state, UtObject *labels) {
  UtObject *object = ut_object_new(sizeof(UtX11ButtonClass), &object_interface);
  UtX11ButtonClass *self = (UtX11ButtonClass *)object;
  self->state = ut_object_ref(state);
  self->labels = ut_object_ref(labels);
  return object;
}

bool ut_object_is_x11_button_class(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
