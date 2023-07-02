#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint16_t type;
} UtX11UnknownInputClass;

static UtObjectInterface object_interface = {.type_name =
                                                 "UtX11UnknownInputClass"};

UtObject *ut_x11_unknown_input_class_new(uint16_t type) {
  UtObject *object =
      ut_object_new(sizeof(UtX11UnknownInputClass), &object_interface);
  UtX11UnknownInputClass *self = (UtX11UnknownInputClass *)object;
  self->type = type;
  return object;
}

bool ut_object_is_x11_unknown_input_class(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
