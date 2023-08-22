#include "ut.h"

static bool is_valid_numeric_string_char(uint8_t c) {
  return (c >= '0' && c <= '9') || c == ' ';
}

static bool is_valid_printable_string_char(uint8_t c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
         (c >= '0' && c <= '9') || c == ' ' || c == '\'' || c == '(' ||
         c == ')' || c == '+' || c == ',' || c == '-' || c == '.' || c == '/' ||
         c == ':' || c == '=' || c == '?';
}

static bool is_valid_ia5_string_char(uint8_t c) { return c >= 0 && c <= 127; }

static bool is_valid_graphic_string_char(uint8_t c) {
  return c >= 32 && c <= 126;
}

static bool is_valid_visible_string_char(uint8_t c) {
  return c >= 32 && c <= 126;
}

static bool is_valid_general_string_char(uint8_t c) {
  // FIXME: Check if 8 bit values are valid?
  return true;
}

static UtObject *encode_utf8_cstring(const char *string,
                                     bool (*is_valid_char)(uint8_t c)) {
  // Check only valid characters provided.
  // Doesn't check codepoints, as all 8 bit values aren't valid.
  for (const char *c = string; *c != '\0'; c++) {
    if (!is_valid_char(*c)) {
      return NULL;
    }
  }

  // Can use UTF8 output, as it's all 7 bit and characters map exactly to UTF8.
  UtObjectRef encoded_string = ut_string_new_constant(string);
  return ut_string_get_utf8(encoded_string);
}

static UtObject *decode_utf8_string(UtObject *encoded_string,
                                    bool (*is_valid_char)(uint8_t c)) {
  // Check only valid characters received.
  size_t encoded_string_length = ut_list_get_length(encoded_string);
  for (size_t i = 0; i < encoded_string_length; i++) {
    uint8_t c = ut_uint8_list_get_element(encoded_string, i);
    if (!is_valid_char(c)) {
      return NULL;
    }
  }

  // Can treat input as UTF8, as it's all 7 bit and characters map exactly to
  // UTF8.
  return ut_string_new_from_utf8(encoded_string);
}

UtObject *ut_asn1_encode_numeric_string(const char *string) {
  return encode_utf8_cstring(string, is_valid_numeric_string_char);
}

UtObject *ut_asn1_decode_numeric_string(UtObject *numeric_string) {
  return decode_utf8_string(numeric_string, is_valid_numeric_string_char);
}

UtObject *ut_asn1_encode_printable_string(const char *string) {
  return encode_utf8_cstring(string, is_valid_printable_string_char);
}

UtObject *ut_asn1_decode_printable_string(UtObject *printable_string) {
  return decode_utf8_string(printable_string, is_valid_printable_string_char);
}

UtObject *ut_asn1_encode_ia5_string(const char *string) {
  return encode_utf8_cstring(string, is_valid_ia5_string_char);
}

UtObject *ut_asn1_decode_ia5_string(UtObject *ia5_string) {
  return decode_utf8_string(ia5_string, is_valid_ia5_string_char);
}

UtObject *ut_asn1_encode_graphic_string(const char *string) {
  return encode_utf8_cstring(string, is_valid_graphic_string_char);
}

UtObject *ut_asn1_decode_graphic_string(UtObject *graphic_string) {
  return decode_utf8_string(graphic_string, is_valid_graphic_string_char);
}

UtObject *ut_asn1_encode_visible_string(const char *string) {
  return encode_utf8_cstring(string, is_valid_visible_string_char);
}

UtObject *ut_asn1_decode_visible_string(UtObject *visible_string) {
  return decode_utf8_string(visible_string, is_valid_visible_string_char);
}

UtObject *ut_asn1_encode_general_string(const char *string) {
  return encode_utf8_cstring(string, is_valid_general_string_char);
}

UtObject *ut_asn1_decode_general_string(UtObject *general_string) {
  return decode_utf8_string(general_string, is_valid_general_string_char);
}
