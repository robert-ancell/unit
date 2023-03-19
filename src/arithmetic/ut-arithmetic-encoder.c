#include <assert.h>
#include <stdlib.h>

#include "ut.h"

typedef struct {
  UtObject object;
} UtArithmeticEncoder;

static UtObjectInterface object_interface = {.type_name = "UtArithmeticEncoder",
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_arithmetic_encoder_new(uint8_t value) {
  UtObject *object =
      ut_object_new(sizeof(UtArithmeticEncoder), &object_interface);
  UtArithmeticEncoder *self = (UtArithmeticEncoder *)object;
  return object;
}

void ut_arithmetic_encoder_get_code(UtObject *object, uint16_t symbol,
                                    uint16_t *code, size_t *code_width) {
  assert(ut_object_is_arithmetic_encoder(object));
  UtArithmeticEncoder *self = (UtArithmeticEncoder *)object;
}

bool ut_object_is_arithmetic_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
