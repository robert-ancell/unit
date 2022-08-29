#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint8_t address[16];
} UtIPv6Address;

static int hex_digit(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  } else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  } else {
    return -1;
  }
}

static char *address_to_string(uint8_t *address) {
  uint16_t groups[8];
  size_t zero_start = 0, zero_length = 0;
  size_t best_zero_start = 0, best_zero_length = 0;
  for (size_t i = 0; i < 8; i++) {
    groups[i] = address[i * 2] << 8 | address[i * 2 + 1];
    if (zero_length > 0) {
      if (groups[i] == 0) {
        zero_length++;
        if (zero_length > best_zero_length) {
          best_zero_start = zero_start;
          best_zero_length = zero_length;
        }
      } else {
        zero_length = 0;
      }
    } else if (groups[i] == 0) {
      zero_start = i;
      zero_length = 1;
    }
  }

  UtObjectRef text = ut_string_new("");
  for (size_t i = 0; i < best_zero_start; i++) {
    if (i != 0) {
      ut_string_append(text, ":");
    }
    ut_string_append_printf(text, "%x", groups[i]);
  }
  if (best_zero_length > 0) {
    ut_string_append(text, "::");
  }
  size_t suffix_start = best_zero_start + best_zero_length;
  for (size_t i = suffix_start; i < 8; i++) {
    if (i != suffix_start) {
      ut_string_append(text, ":");
    }
    ut_string_append_printf(text, "%x", groups[i]);
  }

  return ut_string_take_text(text);
}

static char *ut_ipv6_address_to_string(UtObject *object) {
  UtIPv6Address *self = (UtIPv6Address *)object;
  return address_to_string(self->address);
}

static char *ut_ipv6_address_to_object_string(UtObject *object) {
  UtIPv6Address *self = (UtIPv6Address *)object;
  ut_cstring_ref address_text = address_to_string(self->address);
  return ut_cstring_new_printf("<UtIPv6Address>(\"%s\")", address_text);
}

static UtIPAddressInterface ip_address_interface = {
    .to_string = ut_ipv6_address_to_string};

static UtObjectInterface object_interface = {
    .type_name = "UtIPv6Address",
    .to_string = ut_ipv6_address_to_object_string,
    .interfaces = {{&ut_ip_address_id, &ip_address_interface}, {NULL, NULL}}};

UtObject *ut_ipv6_address_new(uint8_t *address) {
  UtObject *object = ut_object_new(sizeof(UtIPv6Address), &object_interface);
  UtIPv6Address *self = (UtIPv6Address *)object;
  for (size_t i = 0; i < 16; i++) {
    self->address[i] = address[i];
  }
  return object;
}

UtObject *ut_ipv6_address_new_from_string(const char *address) {
  int groups[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
  size_t index = 0, duplicate_index = -1;

  assert(address != NULL);

  for (const char *c = address;; c++) {
    int d = hex_digit(*c);
    if (d != -1) {
      if (groups[index] == -1) {
        groups[index] = d;
      } else {
        groups[index] <<= 4;
        groups[index] += d;
        if (groups[index] > 0xffff) {
          return ut_error_new("Value too large in IPv6 address");
        }
      }
    } else if (*c == ':' && *(c + 1) == ':') {
      if (*(c + 2) == ':') {
        return ut_error_new("Invalid colon sequence in IPv6 address");
      }
      c++;
      if (duplicate_index == -1) {
        if (groups[index] != -1) {
          index++;
        }
        duplicate_index = index;
      } else {
        return ut_error_new("Duplicate :: in IPv6 address");
      }
    } else if (*c == ':' || *c == '\0') {
      if (groups[index] != -1) {
        index++;
      }
      if (*c == '\0') {
        if (duplicate_index != -1) {
          // Shift groups across to make space for zeros
          size_t zero_count = 8 - index;
          size_t suffix_count = 7 - duplicate_index;
          for (int i = 7; i > 8 - suffix_count; i--) {
            groups[i] = groups[i - zero_count];
          }
          for (int i = duplicate_index; i < duplicate_index + zero_count; i++) {
            groups[i] = 0;
          }
          index = 8;
        }
        if (index != 8) {
          return ut_error_new("Insufficient number of groups in IPv6 address");
        }
        uint8_t a[16];
        for (size_t i = 0; i < 8; i++) {
          a[i * 2] = groups[i] >> 8;
          a[i * 2 + 1] = groups[i] & 0xff;
        }
        return ut_ipv6_address_new(a);
      }
      size_t max_index = duplicate_index == -1 ? 7 : 6;
      if (index > max_index) {
        return ut_error_new("Too many groups in IPv6 address");
      }
    } else {
      return ut_error_new("Invalid character in IPv6 address");
    }
  }
}

const uint8_t *ut_ipv6_address_get_address(UtObject *object) {
  assert(ut_object_is_ipv6_address(object));
  UtIPv6Address *self = (UtIPv6Address *)object;
  return self->address;
}

bool ut_object_is_ipv6_address(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
