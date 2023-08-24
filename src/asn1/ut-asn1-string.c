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

static bool is_utc_digit(const char c) { return c >= '0' && c <= '9'; }

static int utc_digit(const char c) { return c - '0'; }

static bool get_utc_time_value(const char *c, size_t *offset,
                               unsigned int *value) {
  if (!is_utc_digit(c[*offset]) || !is_utc_digit(c[*offset + 1])) {
    return false;
  }
  *value = utc_digit(c[*offset]) * 10 + utc_digit(c[*offset + 1]);
  *offset += 2;
  return true;
}

static bool maybe_get_utc_time_value(const char *c, size_t *offset,
                                     unsigned int *value) {
  if (!is_utc_digit(c[*offset])) {
    *value = 0;
    return true;
  }
  return get_utc_time_value(c, offset, value);
}

static bool get_utc_time_offset(const char *c, size_t *offset, int *value) {
  if (c[*offset] == 'Z') {
    (*offset)++;
    *value = 0;
    return true;
  }

  int sign;
  if (c[*offset] == '+') {
    sign = 1;
  } else if (c[*offset] == '-') {
    sign = -1;
  } else {
    return false;
  }
  (*offset)++;

  unsigned int offset_hours, offset_minutes;
  if (!get_utc_time_value(c, offset, &offset_hours) ||
      !get_utc_time_value(c, offset, &offset_minutes) || offset_hours > 13 ||
      offset_minutes > 60) {
    return false;
  }

  *value = sign * (offset_hours * 60 + offset_minutes);
  return true;
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

UtObject *ut_asn1_encode_utc_time(UtObject *value) {
  UtObjectRef value_string = ut_string_new("");

  unsigned int year = ut_date_time_get_year(value);
  if (year < 1950 || year > 2049) {
    return NULL;
  }
  year = year % 100;

  ut_string_append_printf(
      value_string, "%02d%02d%02d%02d%02d%02d", year,
      ut_date_time_get_month(value), ut_date_time_get_day(value),
      ut_date_time_get_hour(value), ut_date_time_get_minutes(value),
      ut_date_time_get_seconds(value));
  int utc_offset = ut_date_time_get_utc_offset(value);
  if (utc_offset == 0) {
    ut_string_append(value_string, "Z");
  } else {
    char sign;
    if (utc_offset < 0) {
      utc_offset = -utc_offset;
      sign = '-';
    } else {
      sign = '+';
    }
    ut_string_append_printf(value_string, "%c%02d%02d", sign, utc_offset / 60,
                            utc_offset % 60);
  }

  return ut_object_ref(value_string);
}

UtObject *ut_asn1_decode_utc_time(UtObject *string) {
  const char *text = ut_string_get_text(string);
  size_t offset = 0;
  unsigned int year, month, day, hour, minutes, seconds;
  int utc_offset;
  if (!get_utc_time_value(text, &offset, &year) ||
      !get_utc_time_value(text, &offset, &month) ||
      !get_utc_time_value(text, &offset, &day) ||
      !get_utc_time_value(text, &offset, &hour) ||
      !get_utc_time_value(text, &offset, &minutes) ||
      !maybe_get_utc_time_value(text, &offset, &seconds) ||
      !get_utc_time_offset(text, &offset, &utc_offset) ||
      text[offset] != '\0' || month < 1 || month > 12 || day < 1 || day > 31) {
    return NULL;
  }

  if (year >= 50) {
    year += 1900;
  } else {
    year += 2000;
  }

  return ut_date_time_new(year, month, day, hour, minutes, seconds, utc_offset);
}

UtObject *ut_asn1_encode_generalized_time(UtObject *value) {
  UtObjectRef value_string = ut_string_new("");

  unsigned int year = ut_date_time_get_year(value);
  if (year > 9999) {
    return NULL;
  }

  ut_string_append_printf(
      value_string, "%04d%02d%02d%02d%02d%02d", year,
      ut_date_time_get_month(value), ut_date_time_get_day(value),
      ut_date_time_get_hour(value), ut_date_time_get_minutes(value),
      ut_date_time_get_seconds(value));
  // FIXME: Subseconds
  int utc_offset = ut_date_time_get_utc_offset(value);
  if (utc_offset == 0) {
    ut_string_append(value_string, "Z");
  } else {
    char sign;
    if (utc_offset < 0) {
      utc_offset = -utc_offset;
      sign = '-';
    } else {
      sign = '+';
    }
    ut_string_append_printf(value_string, "%c%02d%02d", sign, utc_offset / 60,
                            utc_offset % 60);
  }

  return ut_object_ref(value_string);
}

UtObject *ut_asn1_decode_generalized_time(UtObject *string) {
  const char *text = ut_string_get_text(string);
  size_t offset = 0;
  unsigned int century, year, month, day, hour, minutes,
      seconds; // FIXME: subseconds
  int utc_offset;
  if (!get_utc_time_value(text, &offset, &century) ||
      !get_utc_time_value(text, &offset, &year) ||
      !get_utc_time_value(text, &offset, &month) ||
      !get_utc_time_value(text, &offset, &day) ||
      !get_utc_time_value(text, &offset, &hour) ||
      !get_utc_time_value(text, &offset, &minutes) ||
      !maybe_get_utc_time_value(text, &offset, &seconds) ||
      !get_utc_time_offset(text, &offset, &utc_offset) ||
      text[offset] != '\0' || month < 1 || month > 12 || day < 1 || day > 31) {
    return NULL;
  }

  year = century * 100 + year;

  return ut_date_time_new(year, month, day, hour, minutes, seconds, utc_offset);
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

UtObject *ut_asn1_encode_bmp_string(const char *string) {
  UtObjectRef encoded_string = ut_uint8_list_new();
  UtObjectRef s = ut_string_new(string);
  UtObjectRef code_points = ut_string_get_code_points(s);
  size_t code_points_length = ut_list_get_length(code_points);
  for (size_t i = 0; i < code_points_length; i++) {
    uint32_t code_point = ut_uint32_list_get_element(code_points, i);
    if (code_point >= 0xffff) {
      return NULL;
    }
    ut_uint8_list_append(encoded_string, code_point >> 8);
    ut_uint8_list_append(encoded_string, code_point & 0xff);
  }
  return ut_object_ref(encoded_string);
}

UtObject *ut_asn1_decode_bmp_string(UtObject *bmp_string) {
  size_t bmp_string_length = ut_list_get_length(bmp_string);
  if (bmp_string_length % 2 != 0) {
    return NULL;
  }
  UtObjectRef decoded_string = ut_string_new("");
  for (size_t i = 0; i < bmp_string_length; i += 2) {
    ut_string_append_code_point(
        decoded_string, ut_uint8_list_get_element(bmp_string, i) << 8 |
                            ut_uint8_list_get_element(bmp_string, i + 1));
  }
  return ut_object_ref(decoded_string);
}
