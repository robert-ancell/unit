#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
} UtX11NoExpose;

static UtX11EventInterface x11_event_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtX11NoExpose",
    .interfaces = {{&ut_x11_event_id, &x11_event_interface}}};

UtObject *ut_x11_no_expose_new() {
  return ut_object_new(sizeof(UtX11NoExpose), &object_interface);
}

bool ut_object_is_x11_no_expose(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
