#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtX11ErrorCode code;
  uint32_t value;
  uint8_t major_opcode;
  uint16_t minor_opcode;
} UtX11Error;

static UtObjectInterface object_interface = {.type_name = "UtX11Error",
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_x11_error_new(UtX11ErrorCode code, uint32_t value,
                           uint8_t major_opcode, uint16_t minor_opcode) {
  UtObject *object = ut_object_new(sizeof(UtX11Error), &object_interface);
  UtX11Error *self = (UtX11Error *)object;
  self->code = code;
  self->value = value;
  self->major_opcode = major_opcode;
  self->minor_opcode = minor_opcode;
  return object;
}

bool ut_object_is_x11_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
