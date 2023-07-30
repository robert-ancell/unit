#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "ut.h"

typedef struct {
  UtObject object;
  float value;
} UtFloat32;

static char *ut_float32_to_string(UtObject *object) {
  UtFloat32 *self = (UtFloat32 *)object;
  return ut_cstring_new_printf("<float32>(%g)", self->value);
}

static bool ut_float32_equal(UtObject *object, UtObject *other) {
  UtFloat32 *self = (UtFloat32 *)object;
  if (!ut_object_is_float32(other)) {
    return false;
  }
  UtFloat32 *other_self = (UtFloat32 *)other;
  return self->value == other_self->value;
}

static int ut_float32_hash(UtObject *object) {
  UtFloat32 *self = (UtFloat32 *)object;
  return self->value;
}

static UtObjectInterface object_interface = {.type_name = "UtFloat32",
                                             .to_string = ut_float32_to_string,
                                             .equal = ut_float32_equal,
                                             .hash = ut_float32_hash};

UtObject *ut_float32_new(float value) {
  UtObject *object = ut_object_new(sizeof(UtFloat32), &object_interface);
  UtFloat32 *self = (UtFloat32 *)object;
  self->value = value;
  return object;
}

UtObject *ut_float32_new_plus_infinity() { return ut_float32_new(INFINITY); }

UtObject *ut_float32_new_minus_infinity() { return ut_float32_new(-INFINITY); }

UtObject *ut_float32_new_not_a_number() { return ut_float32_new(NAN); }

float ut_float32_get_value(UtObject *object) {
  assert(ut_object_is_float32(object));
  UtFloat32 *self = (UtFloat32 *)object;
  return self->value;
}

bool ut_object_is_float32(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
