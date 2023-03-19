#include <assert.h>
#include <stdlib.h>

#include "ut.h"

typedef struct {
  UtObject object;
} UtArithmeticDecoder;

static UtObjectInterface object_interface = {.type_name = "UtArithmeticDecoder",
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_arithmetic_decoder_new(uint8_t value) {
  UtObject *object =
      ut_object_new(sizeof(UtArithmeticDecoder), &object_interface);
  UtArithmeticDecoder *self = (UtArithmeticDecoder *)object;
  return object;
}

bool ut_arithmetic_decoder_get_symbol(UtObject *object, uint16_t code,
                                      size_t code_width, uint16_t *symbol) {
  assert(ut_object_is_arithmetic_decoder(object));
  UtArithmeticDecoder *self = (UtArithmeticDecoder *)object;

  assert(code_width > 0);
  assert(code < 1 << code_width);

  return false;
}

bool ut_object_is_arithmetic_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
