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

static bool is_valid_visible_string_char(uint8_t c) {
  return c >= 32 && c <= 126;
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

static UtObject *encode_utf8_string(UtObject *string,
                                    bool (*is_valid_char)(uint8_t c)) {
  // Check only valid characters provided.
  // Doesn't check codepoints, as all 8 bit values aren't valid.
  UtObjectRef utf8 = ut_string_get_utf8(string);
  size_t utf8_length = ut_list_get_length(utf8);
  for (size_t i = 0; i < utf8_length; i++) {
    uint8_t c = ut_uint8_list_get_element(utf8, i);
    if (!is_valid_char(c)) {
      return NULL;
    }
  }

  // Can use UTF8 output, as it's all 7 bit and characters map exactly to UTF8.
  return ut_object_ref(utf8);
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

UtObject *ut_asn1_encode_ia5_string(UtObject *string) {
  return encode_utf8_string(string, is_valid_ia5_string_char);
}

UtObject *ut_asn1_decode_ia5_string(UtObject *ia5_string) {
  return decode_utf8_string(ia5_string, is_valid_ia5_string_char);
}

UtObject *ut_asn1_encode_visible_string(const char *string) {
  return encode_utf8_cstring(string, is_valid_visible_string_char);
}

UtObject *ut_asn1_decode_visible_string(UtObject *visible_string) {
  return decode_utf8_string(visible_string, is_valid_visible_string_char);
}
