#include <assert.h>

#include "ut-cstring.h"
#include "ut-error.h"
#include "ut-ipv4-address.h"

typedef struct {
  UtObject object;
  uint32_t address;
} UtIPv4Address;

static char *address_to_string(uint32_t address) {
  return ut_cstring_new_printf("%d.%d.%d.%d", address >> 24,
                               (address >> 16) & 0xff, (address >> 8) & 0xff,
                               address & 0xff);
}

static char *ut_ipv4_address_to_object_string(UtObject *object) {
  UtIPv4Address *self = (UtIPv4Address *)object;
  ut_cstring_ref address_text = address_to_string(self->address);
  return ut_cstring_new_printf("<UtIPv4Address>(\"%s\")", address_text);
}

static void ut_ipv4_address_init(UtObject *object) {
  UtIPv4Address *self = (UtIPv4Address *)object;
  self->address = 0;
}

static UtObjectInterface object_interface = {
    .type_name = "UtIPv4Address",
    .to_string = ut_ipv4_address_to_object_string,
    .init = ut_ipv4_address_init,
    .interfaces = {{NULL, NULL}}};

UtObject *ut_ipv4_address_new(uint32_t address) {
  UtObject *object = ut_object_new(sizeof(UtIPv4Address), &object_interface);
  UtIPv4Address *self = (UtIPv4Address *)object;
  self->address = address;
  return object;
}

UtObject *ut_ipv4_address_new_from_quad(uint8_t a0, uint8_t a1, uint8_t a2,
                                        uint8_t a3) {
  return ut_ipv4_address_new(a0 << 24 | a1 << 16 | a2 << 8 | a3);
}

UtObject *ut_ipv4_address_new_from_string(const char *address) {
  int quads[4] = {-1, -1, -1, -1};
  size_t quad_index = 0;

  assert(address != NULL);

  for (const char *c = address;; c++) {
    if (*c >= '0' && *c <= '9') {
      int d = *c - '0';
      if (quads[quad_index] == -1) {
        quads[quad_index] = d;
      } else {
        quads[quad_index] *= 10;
        quads[quad_index] += d;
        if (quads[quad_index] > 255) {
          return ut_error_new("Value too large in IPv4 address");
        }
      }
    } else if (*c == '.' || *c == '\0') {
      if (quads[quad_index] == -1) {
        return ut_error_new("Empty value in IPv4 address");
      }
      if (*c == '\0') {
        if (quad_index != 3) {
          return ut_error_new("Insufficient number of values in IPv4 address");
        }
        return ut_ipv4_address_new_from_quad(quads[0], quads[1], quads[2],
                                             quads[3]);
      }
      quad_index++;
      if (quad_index > 3) {
        return ut_error_new("Too many values in IPv4 address");
      }
    } else {
      return ut_error_new("Invalid character in IPv4 address");
    }
  }
}

uint32_t ut_ipv4_address_get_address(UtObject *object) {
  assert(ut_object_is_ipv4_address(object));
  UtIPv4Address *self = (UtIPv4Address *)object;
  return self->address;
}

char *ut_ipv4_address_to_string(UtObject *object) {
  assert(ut_object_is_ipv4_address(object));
  UtIPv4Address *self = (UtIPv4Address *)object;
  return address_to_string(self->address);
}

bool ut_object_is_ipv4_address(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
