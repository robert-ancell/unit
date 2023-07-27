#include <assert.h>
#include <float.h>

#include "ut.h"

typedef enum {
  TAG_MODE_EXPLICIT,
  TAG_MODE_IMPLICIT,
  TAG_MODE_AUTOMATIC,
} TagMode;

typedef struct {
  UtObject object;

  // Tokens being parsed.
  UtObject *tokens;
  size_t index;

  // How to apply tags.
  TagMode tag_mode;

  // true if all sequence/set/choice are extensible.
  bool extensibility_implied;

  // Map of values and types defined in this module.
  UtObject *assignments;

  // Completed module definition.
  UtObject *module_definition;

  // Error that occurred during parsing.
  UtObject *error;
} UtAsn1ModuleDefinitionParser;

static void set_error(UtAsn1ModuleDefinitionParser *self,
                      const char *description) {
  if (self->error == NULL) {
    self->error = ut_asn1_error_new(description);
  }
}

static bool is_reserved(const char *word) {
  static const char *reserved_words[] = {"ABSENT",
                                         "ABSTRACT-SYNTAX",
                                         "ALL",
                                         "APPLICATION",
                                         "AUTOMATIC",
                                         "BEGIN",
                                         "BIT",
                                         "BMPString",
                                         "BOOLEAN",
                                         "BY",
                                         "CHARACTER",
                                         "CHOICE",
                                         "CLASS",
                                         "COMPONENT",
                                         "COMPONENTS",
                                         "CONSTRAINED",
                                         "CONTAINING",
                                         "DATE",
                                         "DATE-TIME",
                                         "DEFAULT",
                                         "DEFINITIONS",
                                         "DURATION",
                                         "EMBEDDED",
                                         "ENCODED",
                                         "ENCODING-CONTROL",
                                         "END",
                                         "ENUMERATED",
                                         "EXCEPT",
                                         "EXPLICIT",
                                         "EXPORTS",
                                         "EXTENSIBILITY",
                                         "EXTERNAL",
                                         "FALSE",
                                         "FROM",
                                         "GeneralizedTime",
                                         "GeneralString",
                                         "GraphicString",
                                         "IA5String",
                                         "IDENTIFIER",
                                         "IMPLICIT",
                                         "IMPLIED",
                                         "IMPORTS",
                                         "INCLUDES",
                                         "INSTANCE",
                                         "INSTRUCTIONS",
                                         "INTEGER",
                                         "INTERSECTION",
                                         "ISO646String",
                                         "MAX",
                                         "MIN",
                                         "MINUS-INFINITY",
                                         "NOT-A-NUMBER",
                                         "NULL",
                                         "NumericString",
                                         "OBJECT",
                                         "ObjectDescriptor",
                                         "OCTET",
                                         "OF",
                                         "OID-IRI",
                                         "OPTIONAL",
                                         "PATTERN",
                                         "PDV",
                                         "PLUS-INFINITY",
                                         "PRESENT",
                                         "PrintableString",
                                         "PRIVATE",
                                         "REAL",
                                         "RELATIVE-OID",
                                         "RELATIVE-OID-IRI",
                                         "SEQUENCE",
                                         "SET",
                                         "SETTINGS",
                                         "SIZE",
                                         "STRING",
                                         "SYNTAX",
                                         "T61String",
                                         "TAGS",
                                         "TeletexString",
                                         "TIME",
                                         "TIME-OF-DAY",
                                         "TRUE",
                                         "TYPE-IDENTIFIER",
                                         "UNION",
                                         "UNIQUE",
                                         "UNIVERSAL",
                                         "UniversalString",
                                         "UTCTime",
                                         "UTF8String",
                                         "VideotexString",
                                         "VisibleString",
                                         "WITH",
                                         NULL};
  for (size_t i = 0; reserved_words[i] != NULL; i++) {
    if (ut_cstring_equal(word, reserved_words[i])) {
      return true;
    }
  }

  return false;
}

static bool is_whitespace(char c) {
  return c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' ||
         c == ' ';
}

static bool is_lower_alpha(char c) { return (c >= 'a' && c <= 'z'); }

static bool is_upper_alpha(char c) { return (c >= 'A' && c <= 'Z'); }

static bool is_alpha(char c) { return is_lower_alpha(c) || is_upper_alpha(c); }

static bool is_digit(char c) { return c >= '0' && c <= '9'; }

static bool is_alnum(char c) { return is_alpha(c) || is_digit(c); }

static bool is_hexdigit(char c) {
  return is_digit(c) || (c >= 'A' && c <= 'F');
}

static UtObject *tokenize(UtAsn1ModuleDefinitionParser *self,
                          const char *definition) {
  UtObjectRef tokens = ut_string_list_new();

  const char *c = definition;
  while (true) {
    if (*c == '\0') {
      return ut_object_ref(tokens);
    } else if (is_whitespace(*c)) {
      // Skip
      c++;
    } else if (ut_cstring_starts_with(c, "::=")) {
      ut_string_list_append(tokens, "::=");
      c += 3;
    } else if (*c == '{' || *c == '}' || *c == '(' || *c == ')' || *c == '[' ||
               *c == ']' || *c == ',' || *c == ';' || *c == '<' || *c == '>' ||
               *c == '|' || *c == ':') {
      ut_string_list_append_printf(tokens, "%c", *c);
      c++;
    } else if (ut_cstring_starts_with(c, "..")) {
      const char *start = c;
      do {
        c++;
      } while (*c != '\0' && *c == '.');
      UtObjectRef token = ut_string_new_sized(start, c - start);
      ut_string_list_append(tokens, ut_string_get_text(token));
    } else if (ut_cstring_starts_with(c, "..")) {
      ut_string_list_append(tokens, "..");
      c += 2;
    } else if (ut_cstring_starts_with(c, "--")) {
      c += 2;
      while (*c != '\0' && *c != '\n') {
        c++;
      }
    } else if (ut_cstring_starts_with(c, "/*")) {
      c += 2;
      while (*c != '\0') {
        if (ut_cstring_starts_with(c, "*/")) {
          c += 2;
          break;
        } else {
          c++;
        }
      }
    } else if (is_alpha(*c)) {
      const char *start = c;
      do {
        c++;
      } while (is_alpha(*c) || is_digit(*c) || *c == '-');
      UtObjectRef token = ut_string_new_sized(start, c - start);
      ut_string_list_append(tokens, ut_string_get_text(token));
    } else if (is_digit(*c) || (*c == '-' && is_digit(c[1]))) {
      const char *start = c;
      do {
        c++;
      } while (is_digit(*c));
      if (ut_cstring_starts_with(c, "..")) {
        // Number finishes, is part of a range, e.g. "0..255"
      } else {
        if (*c == '.') {
          do {
            c++;
          } while (is_digit(*c));
        }
        if (*c == 'e' || *c == 'E') {
          c++;
          if (*c == '-') {
            c++;
          }
          while (is_digit(*c)) {
            c++;
          }
        }
      }
      UtObjectRef token = ut_string_new_sized(start, c - start);
      ut_string_list_append(tokens, ut_string_get_text(token));
    } else if (*c == '\"') {
      const char *start = c;
      c++;
      while (*c != '\0') {
        if (ut_cstring_starts_with(c, "\"\"")) {
          c += 2;
        } else if (*c == '\"') {
          c++;
          break;
        } else {
          c++;
        }
      }
      UtObjectRef token = ut_string_new_sized(start, c - start);
      ut_string_list_append(tokens, ut_string_get_text(token));
    } else if (*c == '\'') {
      const char *start = c;
      c++;
      while (*c != '\0') {
        if (c[0] == '\'' && c[1] != '\0') {
          c += 2;
          break;
        } else {
          c++;
        }
      }
      UtObjectRef token = ut_string_new_sized(start, c - start);
      ut_string_list_append(tokens, ut_string_get_text(token));
    } else {
      set_error(self, "Invalid character");
      return NULL;
    }
  }
}

static const char *current_token(UtAsn1ModuleDefinitionParser *self) {
  if (self->index >= ut_list_get_length(self->tokens)) {
    return "";
  }
  return ut_string_list_get_element(self->tokens, self->index);
}

static bool current_token_is(UtAsn1ModuleDefinitionParser *self,
                             const char *value) {
  return ut_cstring_equal(current_token(self), value);
}

static void set_expected_error(UtAsn1ModuleDefinitionParser *self,
                               const char *type) {
  ut_cstring_ref description =
      ut_cstring_new_printf("Expected %s, got %s", type, current_token(self));
  set_error(self, description);
}

static const char *text_to_uint64(const char *text, uint64_t *number) {
  uint64_t number_ = 0;
  const char *c;
  for (c = text; is_digit(*c); c++) {
    // FIXME: Handle overflow
    number_ = number_ * 10 + (*c - '0');
  }

  *number = number_;

  return c;
}

static bool maybe_parse_number(UtAsn1ModuleDefinitionParser *self,
                               uint64_t *value) {
  uint64_t value_ = 0;
  const char *c = current_token(self);
  if (*c == '\0') {
    return false;
  }
  c = text_to_uint64(c, &value_);
  if (*c != '\0') {
    return false;
  }

  *value = value_;
  self->index++;
  return true;
}

static bool parse_number(UtAsn1ModuleDefinitionParser *self, uint64_t *value) {
  if (!maybe_parse_number(self, value)) {
    set_expected_error(self, "number");
    return false;
  }

  return true;
}

static bool maybe_parse_signed_number(UtAsn1ModuleDefinitionParser *self,
                                      UtObject **value) {
  const char *text = current_token(self);

  int64_t sign = 1;
  const char *c = text;
  if (*c == '-') {
    sign = -1;
    c++;
  }

  uint64_t value_ = 0;
  if (*c == '\0') {
    return false;
  }
  c = text_to_uint64(c, &value_);
  if (*c != '\0') {
    return false;
  }
  // Would overflow signed value.
  if (value_ > INT64_MAX) {
    return false;
  }

  *value = ut_int64_new(sign * value_);
  self->index++;
  return true;
}

static bool maybe_parse_real_number(UtAsn1ModuleDefinitionParser *self,
                                    UtObject **value) {
  const char *text = current_token(self);

  double sign = 1.0;
  const char *c = text;
  if (*c == '-') {
    sign = -1;
    c++;
  }

  // FIXME: Disallow duplicate leading 0
  uint64_t integer = 0;
  if (!is_digit(*c)) {
    return false;
  }
  c = text_to_uint64(c, &integer);

  uint64_t fractional = 0, fractional_divisor = 1;
  if (*c == '.') { // FIXME: Allowed trailing .?
    c++;
    if (!is_digit(*c)) {
      return false;
    }
    const char *fractional_start = c;
    c = text_to_uint64(c, &fractional);
    size_t fractional_length = c - fractional_start;
    for (size_t i = 0; i < fractional_length; i++) {
      fractional_divisor *= 10.0;
    }
  }

  bool exponent_negative = false;
  uint64_t exponent = 0;
  if (*c == 'e' || *c == 'E') {
    c++;
    if (*c == '\0') {
      return false;
    }

    if (*c == '-') {
      exponent_negative = -1;
      c++;
    }

    c = text_to_uint64(c, &exponent);
  }
  double exponent_multiplier = 1;
  for (uint64_t i = 0; i < exponent; i++) {
    exponent_multiplier *= 10;
  }
  if (exponent_negative) {
    exponent_multiplier = 1.0 / exponent_multiplier;
  }

  if (*c != '\0') {
    return false;
  }

  *value = ut_float64_new(sign *
                          (integer + (double)fractional / fractional_divisor) *
                          exponent_multiplier);
  self->index++;
  return true;
}

static bool maybe_parse_cstring(UtAsn1ModuleDefinitionParser *self,
                                UtObject **value) {
  const char *token = current_token(self);

  if (token[0] != '\"') {
    return false;
  }

  UtObjectRef value_ = ut_string_new("");
  const char *start = token + 1;
  const char *c = start;
  while (*c != '\0') {
    if (ut_cstring_starts_with(c, "\"\"")) {
      ut_string_append_sized(value_, start, c - start);
      ut_string_append_code_point(value_, '\"');
      c += 2;
      start = c;
    } else if (*c == '\"') {
      // Unterminated string.
      if (c[1] != '\0') {
        return false;
      }
      ut_string_append_sized(value_, start, c - start);
      *value = ut_object_ref(value_);
      self->index++;
      return true;
    } else {
      c++;
    }
  }

  return false;
}

static uint8_t parse_hexdigit(char c) {
  return c <= '9' ? c - '0' : c - 'A' + 10;
}

static bool maybe_parse_hstring(UtAsn1ModuleDefinitionParser *self,
                                UtObject **value) {
  const char *token = current_token(self);

  if (token[0] != '\'') {
    return false;
  }

  UtObjectRef value_ = ut_uint8_list_new();
  uint8_t byte = 0x00;
  size_t nibble_count = 0;
  for (const char *c = token + 1; *c != '\0'; c++) {
    if (ut_cstring_starts_with(c, "'H")) {
      // Unterminated string.
      if (c[2] != '\0') {
        return false;
      }
      if (nibble_count == 1) {
        ut_uint8_list_append(value_, byte);
      }
      *value = ut_object_ref(value_);
      self->index++;
      return true;
    } else if (is_hexdigit(*c)) {
      uint8_t nibble = parse_hexdigit(*c);
      if (nibble_count == 0) {
        byte = nibble << 4;
        nibble_count = 1;
      } else {
        ut_uint8_list_append(value_, byte | nibble);
        nibble_count = 0;
      }
    } else if (*c == ' ') {
      // Skip whitespace.
    } else {
      return false;
    }
  }

  return false;
}

static bool maybe_parse_bstring(UtAsn1ModuleDefinitionParser *self,
                                UtObject **value) {
  const char *token = current_token(self);

  if (token[0] != '\'') {
    return false;
  }

  UtObjectRef data = ut_uint8_list_new();
  uint8_t byte = 0x00;
  uint8_t bit_value = 0x80;
  size_t bit_count = 0;
  for (const char *c = token + 1; *c != '\0'; c++) {
    if (ut_cstring_starts_with(c, "'B")) {
      // Unterminated string.
      if (c[2] != '\0') {
        return false;
      }
      if (bit_value != 0x80) {
        ut_uint8_list_append(data, byte);
      }
      *value = ut_bit_list_new_msb_from_data(data, bit_count);
      self->index++;
      return true;
    } else if (*c == '0' || *c == '1') {
      if (*c == '1') {
        byte |= bit_value;
      }
      bit_value >>= 1;
      if (bit_value == 0) {
        ut_uint8_list_append(data, byte);
        byte = 0x00;
        bit_value = 0x80;
      }
      bit_count++;
    } else if (*c == ' ') {
      // Skip whitespace.
    } else {
      return false;
    }
  }

  return false;
}

static bool is_valid_identifier(const char *identifier) {
  if (is_reserved(identifier) || !is_lower_alpha(identifier[0])) {
    return false;
  }

  bool is_hyphen = false;
  for (size_t i = 1; identifier[i] != '\0'; i++) {
    char c = identifier[i];
    if (c == '-') {
      // Can't be duplicate hyphen.
      if (is_hyphen) {
        return false;
      }
      is_hyphen = true;
    } else {
      is_hyphen = false;
      if (!is_alnum(c)) {
        return false;
      }
    }
  }
  // Can't end in a hyphen.
  if (is_hyphen) {
    return false;
  }

  return true;
}

static bool maybe_parse_identifier(UtAsn1ModuleDefinitionParser *self,
                                   char **identifier) {
  const char *identifier_ = current_token(self);
  if (!is_valid_identifier(identifier_)) {
    return false;
  }

  *identifier = ut_cstring_new(identifier_);
  self->index++;
  return true;
}

static bool parse_identifier(UtAsn1ModuleDefinitionParser *self,
                             char **identifier) {
  if (!maybe_parse_identifier(self, identifier)) {
    set_expected_error(self, "identifier");
    return false;
  }

  return true;
}

static bool is_valid_type_reference(const char *reference) {
  if (is_reserved(reference) || !is_upper_alpha(reference[0])) {
    return false;
  }

  bool is_hyphen = false;
  for (size_t i = 1; reference[i] != '\0'; i++) {
    char c = reference[i];
    if (c == '-') {
      // Can't be duplicate hyphen.
      if (is_hyphen) {
        return false;
      }
      is_hyphen = true;
    } else {
      is_hyphen = false;
      if (!is_alnum(c)) {
        return false;
      }
    }
  }
  // Can't end in a hyphen.
  if (is_hyphen) {
    return false;
  }

  return true;
}

static bool maybe_parse_type_reference(UtAsn1ModuleDefinitionParser *self,
                                       char **reference) {
  const char *reference_ = current_token(self);
  if (!is_valid_type_reference(reference_)) {
    return false;
  }
  *reference = ut_cstring_new(reference_);
  self->index++;
  return true;
}

static bool parse_type_reference(UtAsn1ModuleDefinitionParser *self,
                                 char **reference) {
  if (!maybe_parse_type_reference(self, reference)) {
    set_expected_error(self, "typereference");
    return false;
  }

  return true;
}

static bool maybe_parse_value_reference(UtAsn1ModuleDefinitionParser *self,
                                        char **reference) {
  return maybe_parse_identifier(self, reference);
}

static bool parse_module_reference(UtAsn1ModuleDefinitionParser *self,
                                   char **reference) {
  if (!maybe_parse_type_reference(self, reference)) {
    set_expected_error(self, "modulereference");
    return false;
  }

  return true;
}

static bool maybe_parse_text(UtAsn1ModuleDefinitionParser *self,
                             const char *value) {
  const char *v = current_token(self);
  if (!ut_cstring_equal(v, value)) {
    return false;
  }

  self->index++;
  return true;
}

static bool parse_text(UtAsn1ModuleDefinitionParser *self, const char *value) {
  if (!maybe_parse_text(self, value)) {
    set_expected_error(self, value);
    return false;
  }

  return true;
}

static bool find_name(const char *names[], const char *name, uint64_t *number) {
  for (size_t i = 0; names[i] != NULL; i++) {
    if (ut_cstring_equal(name, names[i])) {
      *number = i;
      return true;
    }
  }

  return false;
}

static bool parse_oid_root_name(const char *name, uint64_t *number) {
  const char *names[] = {"itu-t", "iso", "joint-iso-itu-t", NULL};
  return find_name(names, name, number);
}

static bool parse_oid_itu_t_name(const char *name, uint64_t *number) {
  const char *names[] = {"recommendation",
                         "question",
                         "administration",
                         "network-operator",
                         "identified-organization",
                         "r-recommendation",
                         "data",
                         NULL};
  return find_name(names, name, number);
}

static bool parse_oid_iso_name(const char *name, uint64_t *number) {
  const char *names[] = {"standard", "registration-authority", "member-body",
                         "identified-organization", NULL};
  return find_name(names, name, number);
}

static bool parse_oid_joint_iso_itu_t_name(const char *name, uint64_t *number) {
  const char *names[] = {"",
                         "module",
                         "document-types",
                         "asn-1",
                         "",
                         "international-md",
                         "international-organization",
                         NULL};
  return find_name(names, name, number);
}

static bool parse_oid_name(UtObject *oid, const char *name, uint64_t *number) {
  size_t oid_length = ut_list_get_length(oid);
  if (oid_length == 0) {
    return parse_oid_root_name(name, number);
  } else if (oid_length == 1) {
    uint32_t root_number = ut_uint32_list_get_element(oid, 0);
    switch (root_number) {
    case 0:
      return parse_oid_itu_t_name(name, number);
    case 1:
      return parse_oid_iso_name(name, number);
    case 2:
      return parse_oid_joint_iso_itu_t_name(name, number);
    default:
      return false;
    }
  } else {
    return false;
  }
}

static bool
parse_definitive_object_identifier_component(UtAsn1ModuleDefinitionParser *self,
                                             UtObject *value) {
  ut_cstring_ref name = NULL;
  uint64_t number = 0;
  if (!maybe_parse_identifier(self, &name)) {
    if (!maybe_parse_number(self, &number)) {
      set_expected_error(self, "identifier, number or }");
      return false;
    }
    ut_uint32_list_append(value, number);
    return true;
  }

  if (maybe_parse_text(self, "(")) {
    if (!parse_number(self, &number) || !parse_text(self, ")")) {
      return false;
    }
    uint64_t expected_number = 0;
    if (ut_list_get_length(value) < 2) {
      if (!parse_oid_name(value, name, &expected_number)) {
        set_error(self, "Unknown object identifier name");
        return false;
      }
      if (number != expected_number) {
        set_error(self, "Object identifier name / number mismatch");
        return false;
      }
    }
  } else if (!parse_oid_name(value, name, &number)) {
    set_error(self, "Unknown object identifier name");
    return false;
  }

  ut_uint32_list_append(value, number);
  return true;
}

static bool parse_module_identifier(UtAsn1ModuleDefinitionParser *self,
                                    char **reference,
                                    UtObject **object_identifier) {
  if (!parse_module_reference(self, reference)) {
    return false;
  }

  if (!maybe_parse_text(self, "{")) {
    *object_identifier = NULL;
    return true;
  }

  UtObjectRef oid = ut_uint32_list_new();
  while (true) {
    if (!parse_definitive_object_identifier_component(self, oid)) {
      return false;
    }

    if (maybe_parse_text(self, "}")) {
      *object_identifier = ut_object_ref(oid);
      return true;
    }
  }

  return true;
}

static bool parse_tag_default(UtAsn1ModuleDefinitionParser *self) {
  self->tag_mode = TAG_MODE_EXPLICIT;
  if (maybe_parse_text(self, "EXPLICIT")) {
    if (!parse_text(self, "TAGS")) {
      return false;
    }
    self->tag_mode = TAG_MODE_EXPLICIT;
  } else if (maybe_parse_text(self, "IMPLICIT")) {
    if (!parse_text(self, "TAGS")) {
      return false;
    }
    self->tag_mode = TAG_MODE_IMPLICIT;
  } else if (maybe_parse_text(self, "AUTOMATIC")) {
    if (!parse_text(self, "TAGS")) {
      return false;
    }
    self->tag_mode = TAG_MODE_AUTOMATIC;
  }

  return true;
}

static bool parse_extension_default(UtAsn1ModuleDefinitionParser *self) {
  self->extensibility_implied = false;
  if (maybe_parse_text(self, "EXTENSIBILITY")) {
    if (!parse_text(self, "IMPLIED")) {
      return false;
    }
    self->extensibility_implied = true;
  }

  return true;
}

static bool parse_symbol_list(UtAsn1ModuleDefinitionParser *self,
                              UtObject **symbols) {
  UtObjectRef symbols_ = ut_string_list_new();
  while (true) {
    if (ut_list_get_length(symbols_) > 0) {
      if (!maybe_parse_text(self, ",")) {
        *symbols = ut_object_ref(symbols_);
        return true;
      }
    }

    ut_cstring_ref reference = NULL;
    if (!maybe_parse_type_reference(self, &reference) &&
        !maybe_parse_value_reference(self, &reference)) {
      set_expected_error(self, "typereference or valuereference");
      return false;
    }
    ut_string_list_append(symbols_, reference);
  }
  return false;
}

static bool parse_exports(UtAsn1ModuleDefinitionParser *self, bool *all,
                          UtObject **symbols) {
  if (!maybe_parse_text(self, "EXPORTS")) {
    *all = false;
    *symbols = ut_string_list_new();
    return true;
  }

  if (maybe_parse_text(self, "ALL")) {
    *all = true;
    *symbols = ut_string_list_new();
    return parse_text(self, ";");
  }

  if (maybe_parse_text(self, ";")) {
    *all = false;
    *symbols = ut_string_list_new();
    return true;
  }

  UtObjectRef symbols_ = NULL;
  if (!parse_symbol_list(self, &symbols_)) {
    return false;
  }

  if (!parse_text(self, ";")) {
    return false;
  }

  *all = false;
  *symbols = ut_object_ref(symbols_);
  return true;
}

static bool parse_symbols_from_module(UtAsn1ModuleDefinitionParser *self,
                                      UtObject **module_imports) {
  UtObjectRef symbols = NULL;
  if (!parse_symbol_list(self, &symbols)) {
    return false;
  }

  if (!parse_text(self, "FROM")) {
    return false;
  }

  ut_cstring_ref reference = NULL;
  if (!parse_module_reference(self, &reference)) {
    return false;
  }

  *module_imports = ut_asn1_module_imports_new(reference, symbols);

  return true;
}

static bool parse_imports(UtAsn1ModuleDefinitionParser *self,
                          UtObject **imports) {
  if (!maybe_parse_text(self, "IMPORTS")) {
    return true;
  }

  UtObjectRef imports_ = ut_object_list_new();
  while (!current_token_is(self, ";")) {
    ut_cstring_ref module = NULL;
    UtObjectRef module_imports = NULL;
    if (!parse_symbols_from_module(self, &module_imports)) {
      return false;
    }
    ut_list_append(imports_, module_imports);
  }

  if (!parse_text(self, ";")) {
    return false;
  }

  *imports = ut_object_ref(imports_);
  return true;
}

static bool type_matches(UtObject *type1, UtObject *type2) {
  UtObject *base_type1 = ut_asn1_type_get_base_type(type1);
  UtObject *base_type2 = ut_asn1_type_get_base_type(type2);
  return ut_object_equal(base_type1, base_type2);
}

static bool maybe_parse_defined_value(UtAsn1ModuleDefinitionParser *self,
                                      UtObject *type, UtObject **value) {
  const char *reference = current_token(self);

  UtObject *referenced_type_value =
      ut_map_lookup_string(self->assignments, reference);
  if (referenced_type_value == NULL ||
      !ut_object_is_asn1_type_value(referenced_type_value)) {
    return false;
  }
  self->index++;

  if (!type_matches(type, ut_asn1_type_value_get_type(referenced_type_value))) {
    ut_cstring_ref description =
        ut_cstring_new_printf("Reference %s has incorrect type", reference);
    set_error(self, description);
    return false;
  }

  *value = ut_object_ref(ut_asn1_type_value_get_value(referenced_type_value));

  return true;
}

static bool parse_boolean_value(UtAsn1ModuleDefinitionParser *self,
                                UtObject **value) {
  if (maybe_parse_text(self, "TRUE")) {
    *value = ut_boolean_new(true);
    return true;
  } else if (maybe_parse_text(self, "FALSE")) {
    *value = ut_boolean_new(false);
    return true;
  }

  set_expected_error(self, "TRUE or FALSE");
  return false;
}

static bool parse_integer_value(UtAsn1ModuleDefinitionParser *self,
                                UtObject **value) {
  if (maybe_parse_signed_number(self, value)) {
    return true;
  }

  set_expected_error(self, "integer");
  return false;
}

static bool parse_bit_string_value(UtAsn1ModuleDefinitionParser *self,
                                   UtObject **value) {
  if (maybe_parse_bstring(self, value)) {
    return true;
  }

  set_expected_error(self, "bstring");
  return false;
}

static bool parse_octet_string_value(UtAsn1ModuleDefinitionParser *self,
                                     UtObject **value) {
  if (maybe_parse_hstring(self, value)) {
    return true;
  }
  UtObjectRef bstring_value = NULL;
  if (maybe_parse_bstring(self, &bstring_value)) {
    *value = ut_object_ref(ut_bit_list_get_data(bstring_value));
    return true;
  }

  set_expected_error(self, "hstring or bstring");
  return false;
}

static bool parse_null_value(UtAsn1ModuleDefinitionParser *self,
                             UtObject **value) {
  if (maybe_parse_text(self, "NULL")) {
    *value = ut_null_new();
    return true;
  }

  set_expected_error(self, "NULL");
  return false;
}

static bool
parse_object_identifier_component(UtAsn1ModuleDefinitionParser *self,
                                  UtObject *value) {
  ut_cstring_ref name = NULL;
  uint64_t number = 0;
  if (!maybe_parse_identifier(self, &name)) {
    if (!maybe_parse_number(self, &number)) {
      set_expected_error(self, "identifier, number or }");
      return false;
    }
    ut_uint32_list_append(value, number);
    return true;
  }

  if (maybe_parse_text(self, "(")) {
    if (!parse_number(self, &number) || !parse_text(self, ")")) {
      return false;
    }
    uint64_t expected_number = 0;
    if (ut_list_get_length(value) < 2) {
      if (!parse_oid_name(value, name, &expected_number)) {
        set_error(self, "Unknown object identifier name");
        return false;
      }
      if (number != expected_number) {
        set_error(self, "Object identifier name / number mismatch");
        return false;
      }
    }
  } else if (!parse_oid_name(value, name, &number)) {
    UtObject *referenced_type_value =
        ut_map_lookup_string(self->assignments, name);
    if (referenced_type_value == NULL ||
        !ut_object_is_asn1_type_value(referenced_type_value)) {
      set_error(self, "Unknown object identifier name");
      return false;
    }

    bool is_first = ut_list_get_length(value) == 0;
    UtObject *referenced_type =
        ut_asn1_type_value_get_type(referenced_type_value);
    if ((is_first &&
         !ut_object_is_asn1_object_identifier_type(referenced_type)) ||
        (!is_first && !ut_object_is_asn1_relative_oid_type(referenced_type))) {
      set_error(self, "Invalid object identifier type");
      return false;
    }

    ut_list_append_list(value,
                        ut_asn1_type_value_get_value(referenced_type_value));
    return true;
  }

  ut_uint32_list_append(value, number);
  return true;
}

static bool parse_object_identifier_value(UtAsn1ModuleDefinitionParser *self,
                                          UtObject **value) {
  if (!parse_text(self, "{")) {
    return false;
  }

  UtObjectRef value_ = ut_uint32_list_new();
  while (true) {
    if (maybe_parse_text(self, "}")) {
      *value = ut_object_ref(value_);
      return true;
    }

    if (!parse_object_identifier_component(self, value_)) {
      return false;
    }
  }
}

static bool parse_relative_oid_component(UtAsn1ModuleDefinitionParser *self,
                                         UtObject *value) {
  ut_cstring_ref name = NULL;
  uint64_t number = 0;
  if (maybe_parse_identifier(self, &name)) {
    if (!maybe_parse_text(self, "(")) {
      UtObject *referenced_type_value =
          ut_map_lookup_string(self->assignments, name);
      if (referenced_type_value != NULL &&
          ut_object_is_asn1_type_value(referenced_type_value) &&
          ut_object_is_asn1_relative_oid_type(
              ut_asn1_type_value_get_type(referenced_type_value))) {
        ut_list_append_list(
            value, ut_asn1_type_value_get_value(referenced_type_value));
        return true;
      }

      set_error(self, "Unknown object identifier name");
      return false;
    }

    if (!parse_number(self, &number) || !parse_text(self, ")")) {
      return false;
    }
  } else if (!maybe_parse_number(self, &number)) {
    set_expected_error(self, "identifier, number or }");
    return false;
  }

  ut_uint32_list_append(value, number);
  return true;
}

static bool parse_relative_oid_value(UtAsn1ModuleDefinitionParser *self,
                                     UtObject **value) {
  if (!parse_text(self, "{")) {
    return false;
  }

  UtObjectRef value_ = ut_uint32_list_new();
  while (true) {
    if (maybe_parse_text(self, "}")) {
      *value = ut_object_ref(value_);
      return true;
    }

    if (!parse_relative_oid_component(self, value_)) {
      return false;
    }
  }
}

static bool parse_real_value(UtAsn1ModuleDefinitionParser *self,
                             UtObject **value) {
  if (maybe_parse_real_number(self, value)) {
    return true;
  } else if (maybe_parse_text(self, "PLUS-INFINITY")) {
    *value = ut_float64_new_plus_infinity();
    return true;
  } else if (maybe_parse_text(self, "MINUS-INFINITY")) {
    *value = ut_float64_new_minus_infinity();
    return true;
  } else if (maybe_parse_text(self, "NOT-A-NUMBER")) {
    *value = ut_float64_new_not_a_number();
    return true;
  }

  set_expected_error(
      self, "real value, PLUS-INFINITY, MINUS-INFINITY or NOT-A-NUMBER");
  return false;
}

static bool parse_value(UtAsn1ModuleDefinitionParser *self, UtObject *type,
                        UtObject **value);

static bool parse_sequence_value(UtAsn1ModuleDefinitionParser *self,
                                 UtObject *type, UtObject **value) {
  if (!parse_text(self, "{")) {
    return false;
  }

  UtObjectRef values = ut_map_new();
  while (!maybe_parse_text(self, "}")) {
    if (ut_map_get_length(values) > 0) {
      if (!parse_text(self, ",")) {
        return false;
      }
    }

    ut_cstring_ref identifier = NULL;
    if (!parse_identifier(self, &identifier)) {
      return false;
    }

    UtObject *component_type =
        ut_asn1_sequence_type_get_component(type, identifier);
    if (component_type == NULL) {
      set_error(self, "Invalid sequence component");
      return false;
    }

    UtObjectRef component_value = NULL;
    if (!parse_value(self, component_type, &component_value)) {
      return false;
    }

    ut_map_insert_string(values, identifier, component_value);
  }

  *value = ut_object_ref(values);
  return true;
}

static bool parse_value_list(UtAsn1ModuleDefinitionParser *self, UtObject *type,
                             UtObject **value) {
  if (!parse_text(self, "{")) {
    return false;
  }

  UtObjectRef values = ut_list_new();
  while (!maybe_parse_text(self, "}")) {
    if (ut_list_get_length(values) > 0) {
      if (!parse_text(self, ",")) {
        return false;
      }
    }

    UtObjectRef component_value = NULL;
    if (!parse_value(self, type, &component_value)) {
      return false;
    }

    ut_list_append(values, component_value);
  }

  *value = ut_object_ref(values);
  return true;
}

static bool parse_sequence_of_value(UtAsn1ModuleDefinitionParser *self,
                                    UtObject *type, UtObject **value) {
  return parse_value_list(self, ut_asn1_sequence_of_type_get_type(type), value);
}

static bool parse_set_value(UtAsn1ModuleDefinitionParser *self, UtObject *type,
                            UtObject **value) {
  if (!parse_text(self, "{")) {
    return false;
  }

  UtObjectRef values = ut_map_new();
  while (!maybe_parse_text(self, "}")) {
    if (ut_map_get_length(values) > 0) {
      if (!parse_text(self, ",")) {
        return false;
      }
    }

    ut_cstring_ref identifier = NULL;
    if (!parse_identifier(self, &identifier)) {
      return false;
    }

    UtObject *component_type = ut_asn1_set_type_get_component(type, identifier);
    if (component_type == NULL) {
      set_error(self, "Invalid set component");
      return false;
    }

    UtObjectRef component_value = NULL;
    if (!parse_value(self, component_type, &component_value)) {
      return false;
    }

    ut_map_insert_string(values, identifier, component_value);
  }

  *value = ut_object_ref(values);
  return true;
}

static bool parse_set_of_value(UtAsn1ModuleDefinitionParser *self,
                               UtObject *type, UtObject **value) {
  return parse_value_list(self, ut_asn1_set_of_type_get_type(type), value);
}

static bool maybe_parse_enumerated_value(UtAsn1ModuleDefinitionParser *self,
                                         UtObject *type, UtObject **value) {
  const char *identifier = current_token(self);

  if (ut_asn1_enumerated_type_get_value(type, identifier) < 0) {
    return false;
  }
  self->index++;

  *value = ut_string_new(identifier);
  return true;
}

static bool parse_enumerated_value(UtAsn1ModuleDefinitionParser *self,
                                   UtObject *type, UtObject **value) {
  if (!maybe_parse_enumerated_value(self, type, value) &&
      !maybe_parse_defined_value(self, type, value)) {
    set_expected_error(self, "enumerated value");
    return false;
  }

  return true;
}

static bool maybe_parse_choice_value(UtAsn1ModuleDefinitionParser *self,
                                     UtObject *type, UtObject **value) {
  const char *identifier = current_token(self);

  UtObject *component_type =
      ut_asn1_choice_type_get_component(type, identifier);
  if (component_type == NULL) {
    return false;
  }
  self->index++;

  return parse_text(self, ":") &&
         parse_value(self, component_type, value); // FIXME: AsnChoiceValue
}

static bool parse_choice_value(UtAsn1ModuleDefinitionParser *self,
                               UtObject *type, UtObject **value) {
  if (!maybe_parse_choice_value(self, type, value) &&
      !maybe_parse_defined_value(self, type, value)) {
    set_expected_error(self, "choice value");
    return false;
  }

  return true;
}

static bool parse_character_string_value(UtAsn1ModuleDefinitionParser *self,
                                         UtObject **value) {
  if (maybe_parse_cstring(self, value)) {
    return true;
  }

  set_expected_error(self, "cstring");
  return false;
}

static bool parse_value(UtAsn1ModuleDefinitionParser *self, UtObject *type,
                        UtObject **value) {
  // Dereference types.
  if (ut_object_is_asn1_referenced_type(type)) {
    return parse_value(self, ut_asn1_referenced_type_get_type(type), value);
  }

  // Tagging doesn't change values.
  if (ut_object_is_asn1_tagged_type(type)) {
    return parse_value(self, ut_asn1_tagged_type_get_type(type), value);
  }

  // Enumerated and choice are special cases as values can also look like
  // references.
  if (ut_object_is_asn1_enumerated_type(type)) {
    return parse_enumerated_value(self, type, value);
  } else if (ut_object_is_asn1_choice_type(type)) {
    return parse_choice_value(self, type, value);
  }

  UtObjectRef value_type = NULL;
  if (maybe_parse_defined_value(self, type, value)) {
    return true;
  }

  if (ut_object_is_asn1_boolean_type(type)) {
    return parse_boolean_value(self, value);
  } else if (ut_object_is_asn1_integer_type(type)) {
    return parse_integer_value(self, value);
  } else if (ut_object_is_asn1_bit_string_type(type)) {
    return parse_bit_string_value(self, value);
  } else if (ut_object_is_asn1_octet_string_type(type)) {
    return parse_octet_string_value(self, value);
  } else if (ut_object_is_asn1_null_type(type)) {
    return parse_null_value(self, value);
  } else if (ut_object_is_asn1_object_identifier_type(type)) {
    return parse_object_identifier_value(self, value);
  } else if (ut_object_is_asn1_object_descriptor_type(type)) {
    return parse_character_string_value(self, value);
  } else if (ut_object_is_asn1_relative_oid_type(type)) {
    return parse_relative_oid_value(self, value);
  } else if (ut_object_is_asn1_real_type(type)) {
    return parse_real_value(self, value);
  } else if (ut_object_is_asn1_utf8_string_type(type)) {
    return parse_character_string_value(self, value);
  } else if (ut_object_is_asn1_sequence_type(type)) {
    return parse_sequence_value(self, type, value);
  } else if (ut_object_is_asn1_sequence_of_type(type)) {
    return parse_sequence_of_value(self, type, value);
  } else if (ut_object_is_asn1_set_type(type)) {
    return parse_set_value(self, type, value);
  } else if (ut_object_is_asn1_set_of_type(type)) {
    return parse_set_of_value(self, type, value);
  } else if (ut_object_is_asn1_numeric_string_type(type)) {
    return parse_character_string_value(self, value);
  } else if (ut_object_is_asn1_printable_string_type(type)) {
    return parse_character_string_value(self, value);
  } else if (ut_object_is_asn1_ia5_string_type(type)) {
    return parse_character_string_value(self, value);
  } else if (ut_object_is_asn1_graphic_string_type(type)) {
    return parse_character_string_value(self, value);
  } else if (ut_object_is_asn1_visible_string_type(type)) {
    return parse_character_string_value(self, value);
  } else if (ut_object_is_asn1_general_string_type(type)) {
    return parse_character_string_value(self, value);
  }

  set_expected_error(self, "value");
  return false;
}

static bool parse_constraint(UtAsn1ModuleDefinitionParser *self, UtObject *type,
                             UtObject **constraint);

static bool parse_size_constraint(UtAsn1ModuleDefinitionParser *self,
                                  UtObject **constraint) {
  UtObjectRef size_constraint_type = ut_asn1_integer_type_new();
  UtObjectRef size_constraint = NULL;
  if (!parse_constraint(self, size_constraint_type, &size_constraint) ||
      size_constraint == NULL) {
    return false;
  }

  // FIXME: Check for negative values.

  *constraint = ut_asn1_size_constraint_new(size_constraint);
  return true;
}

static bool parse_constraint_element(UtAsn1ModuleDefinitionParser *self,
                                     UtObject *type, UtObject **constraint) {
  bool is_integer = ut_object_is_asn1_integer_type(type);
  bool is_real = ut_object_is_asn1_real_type(type);

  UtObjectRef lower_value = NULL;
  bool lower_is_min = false;
  if (maybe_parse_text(self, "MIN")) {
    lower_is_min = true;
    if (is_integer) {
      lower_value = ut_int64_new(INT64_MIN);
    } else if (is_real) {
      lower_value = ut_float64_new(DBL_MIN);
    }
  } else if (maybe_parse_text(self, "SIZE")) {
    return parse_size_constraint(self, constraint);
  } else if (!parse_value(self, type, &lower_value)) {
    return false;
  }

  if (!maybe_parse_text(self, "..")) {
    if (lower_is_min) {
      set_expected_error(self, "value range");
      return false;
    }

    *constraint = ut_asn1_value_constraint_new(lower_value);
    return true;
  }

  if (!is_integer && !is_real) {
    set_error(self, "Range constraint only supported for INTEGER and REAL");
    return false;
  }

  UtObjectRef upper_value = NULL;
  if (maybe_parse_text(self, "MAX")) {
    if (is_integer) {
      upper_value = ut_int64_new(INT64_MAX);
    } else if (is_real) {
      upper_value = ut_float64_new(DBL_MAX);
    }
  } else if (!parse_value(self, type, &upper_value)) {
    return false;
  }

  // FIXME: Check upper value >= lower value

  if (is_integer) {
    *constraint = ut_asn1_integer_range_constraint_new(
        ut_int64_get_value(lower_value), ut_int64_get_value(upper_value));
  } else if (is_real) {
    *constraint = ut_asn1_real_range_constraint_new(
        ut_float64_get_value(lower_value), ut_float64_get_value(upper_value));
  }

  return true;
}

static bool maybe_parse_union_mark(UtAsn1ModuleDefinitionParser *self) {
  return maybe_parse_text(self, "|") || maybe_parse_text(self, "UNION");
}

static bool maybe_parse_intersection_mark(UtAsn1ModuleDefinitionParser *self) {
  return maybe_parse_text(self, "^") || maybe_parse_text(self, "INTERSECTION");
}

static bool parse_intersections(UtAsn1ModuleDefinitionParser *self,
                                UtObject *type, UtObject *constraint0,
                                UtObject **constraint);

static bool parse_unions(UtAsn1ModuleDefinitionParser *self, UtObject *type,
                         UtObject *constraint0, UtObject **constraint) {
  UtObjectRef constraints = ut_list_new();
  ut_list_append(constraints, constraint0);
  while (true) {
    UtObjectRef c = NULL;
    if (!parse_constraint_element(self, type, &c)) {
      return false;
    }

    if (maybe_parse_intersection_mark(self)) {
      UtObjectRef c2 = NULL;
      if (!parse_intersections(self, type, c, &c2)) {
        return false;
      }
      ut_list_append(constraints, c2);
    } else {
      ut_list_append(constraints, c);
    }

    if (!maybe_parse_union_mark(self)) {
      *constraint = ut_asn1_union_constraint_new(constraints);
      return true;
    }
  }
}

static bool parse_intersections(UtAsn1ModuleDefinitionParser *self,
                                UtObject *type, UtObject *constraint0,
                                UtObject **constraint) {
  UtObjectRef constraints = ut_list_new();
  ut_list_append(constraints, constraint0);
  while (true) {
    UtObjectRef c = NULL;
    if (!parse_constraint_element(self, type, &c)) {
      return false;
    }
    ut_list_append(constraints, c);

    if (maybe_parse_union_mark(self)) {
      UtObjectRef c2 = ut_asn1_intersection_constraint_new(constraints);
      return parse_unions(self, type, c2, constraint);
    }

    if (!maybe_parse_intersection_mark(self)) {
      *constraint = ut_asn1_intersection_constraint_new(constraints);
      return true;
    }
  }
}

static bool parse_constraint(UtAsn1ModuleDefinitionParser *self, UtObject *type,
                             UtObject **constraint) {
  if (!maybe_parse_text(self, "(")) {
    *constraint = NULL;
    return true;
  }

  UtObjectRef constraint0 = NULL;
  if (!parse_constraint_element(self, type, &constraint0)) {
    return false;
  }

  UtObjectRef constraint_ = NULL;
  if (maybe_parse_union_mark(self)) {
    parse_unions(self, type, constraint0, &constraint_);
  } else if (maybe_parse_intersection_mark(self)) {
    parse_intersections(self, type, constraint0, &constraint_);
  } else {
    constraint_ = ut_object_ref(constraint0);
  }

  if (!parse_text(self, ")")) {
    return false;
  }

  *constraint = ut_object_ref(constraint_);
  return true;
}

static bool parse_enumeration_item(UtAsn1ModuleDefinitionParser *self,
                                   char **name, uint64_t *number) {
  ut_cstring_ref name_ = NULL;
  if (!parse_identifier(self, &name_)) {
    return false;
  }

  uint64_t number_ = *number;
  if (maybe_parse_text(self, "(")) {
    if (!parse_number(self, &number_) || !parse_text(self, ")")) {
      return false;
    }
  }

  *name = ut_cstring_take(&name_);
  *number = number_;
  return true;
}

static bool parse_enumerated_type(UtAsn1ModuleDefinitionParser *self,
                                  UtObject **type) {
  if (!parse_text(self, "{")) {
    return false;
  }
  UtObjectRef components = ut_map_new();
  uint64_t next_number = 0;
  while (true) {
    if (ut_map_get_length(components) > 0) {
      if (!parse_text(self, ",")) {
        return false;
      }
    }

    ut_cstring_ref name = NULL;
    uint64_t number = next_number;
    if (!parse_enumeration_item(self, &name, &number)) {
      return false;
    }

    if (number < next_number) {
      set_error(self, "Invalid enumeration number");
      return false;
    }
    next_number = number + 1;

    ut_map_insert_string(components, name, ut_uint64_new(number));

    if (maybe_parse_text(self, "}")) {
      *type = ut_asn1_enumerated_type_new(components, false);
      return true;
    }
  }

  return false;
}

static bool parse_named_type(UtAsn1ModuleDefinitionParser *self,
                             char **identifier, UtObject **type);

static bool parse_component_type_list(UtAsn1ModuleDefinitionParser *self,
                                      UtObject **components, bool *extensible) {
  UtObjectRef components_ = ut_map_new();
  bool extensible_ = false;
  bool first_component = true;
  while (true) {
    if (maybe_parse_text(self, "}")) {
      *components = ut_object_ref(components_);
      *extensible = extensible_ || self->extensibility_implied;
      return true;
    }

    if (!first_component) {
      if (!parse_text(self, ",")) {
        return false;
      }
    }
    first_component = false;

    if (maybe_parse_text(self, "...")) {
      if (extensible_) {
        set_error(self, "Duplicate extensible marker");
        return false;
      }
      extensible_ = true;
      continue;
    }

    ut_cstring_ref identifier = NULL;
    UtObjectRef type = NULL;
    UtObjectRef component_type = NULL;
    if (!parse_named_type(self, &identifier, &type)) {
      return false;
    }
    if (maybe_parse_text(self, "OPTIONAL")) {
      component_type = ut_asn1_optional_type_new(type);
    } else if (maybe_parse_text(self, "DEFAULT")) {
      UtObjectRef default_value = NULL;
      if (!parse_value(self, type, &default_value)) {
        return false;
      }
      component_type = ut_asn1_default_type_new(type, default_value);
    } else {
      component_type = ut_object_ref(type);
    }

    UtObject *existing_type = ut_map_lookup_string(components_, identifier);
    if (existing_type != NULL) {
      ut_cstring_ref description = ut_cstring_new_printf(
          "Duplicate component identifier %s", identifier);
      set_error(self, description);
      return false;
    }

    ut_map_insert_string(components_, identifier, component_type);
  }
}

static UtObject *apply_automatic_tagging(UtAsn1ModuleDefinitionParser *self,
                                         UtObject *components) {
  if (self->tag_mode != TAG_MODE_AUTOMATIC) {
    return ut_object_ref(components);
  }

  // Automatic tagging only applied if no tags provided.
  UtObjectRef identifiers = ut_map_get_keys(components);
  size_t identifiers_length = ut_list_get_length(identifiers);
  for (size_t i = 0; i < identifiers_length; i++) {
    UtObjectRef identifier = ut_list_get_element(identifiers, i);
    UtObject *type =
        ut_map_lookup_string(components, ut_string_get_text(identifier));

    if (ut_object_is_asn1_optional_type(type)) {
      type = ut_asn1_optional_type_get_type(type);
    } else if (ut_object_is_asn1_default_type(type)) {
      type = ut_asn1_default_type_get_type(type);
    }

    if (ut_object_is_asn1_tagged_type(type)) {
      return ut_object_ref(components);
    }
  }

  UtObjectRef tagged_components = ut_map_new();
  for (size_t i = 0; i < identifiers_length; i++) {
    UtObjectRef identifier = ut_list_get_element(identifiers, i);
    UtObject *type =
        ut_map_lookup_string(components, ut_string_get_text(identifier));
    UtObjectRef tagged_type = ut_asn1_tagged_type_new(
        UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, i, false, type);
    ut_map_insert_string(tagged_components, ut_string_get_text(identifier),
                         tagged_type);
  }

  return ut_object_ref(tagged_components);
}

static bool parse_type(UtAsn1ModuleDefinitionParser *self, UtObject **type);

static bool parse_sequence_type(UtAsn1ModuleDefinitionParser *self,
                                UtObject **type) {
  UtObjectRef size_constraint = NULL;
  bool is_list = false;
  if (maybe_parse_text(self, "SIZE")) {
    if (!parse_size_constraint(self, &size_constraint) ||
        !parse_text(self, "OF")) {
      return false;
    }
    is_list = true;
  } else {
    is_list = maybe_parse_text(self, "OF");
  }

  if (is_list) {
    UtObjectRef child_type = NULL;
    if (!parse_type(self, &child_type)) {
      set_error(self, "Missing type for SEQUENCE OF");
      return false;
    }
    UtObjectRef list_type = ut_asn1_sequence_of_type_new(child_type);
    if (size_constraint != NULL) {
      *type = ut_asn1_constrained_type_new(list_type, size_constraint);
    } else {
      *type = ut_object_ref(list_type);
    }
    return true;
  }

  if (!parse_text(self, "{")) {
    return false;
  }

  UtObjectRef components = NULL;
  bool extensible = false;
  if (!parse_component_type_list(self, &components, &extensible)) {
    return false;
  }

  UtObjectRef tagged_components = apply_automatic_tagging(self, components);

  UtObjectRef type_ = ut_asn1_sequence_type_new(tagged_components, extensible);
  if (ut_object_implements_error(type_)) {
    if (self->error == NULL) {
      self->error = ut_object_ref(type_);
    }
    return false;
  }

  *type = ut_object_ref(type_);
  return true;
}

static bool parse_set_type(UtAsn1ModuleDefinitionParser *self,
                           UtObject **type) {
  UtObjectRef size_constraint = NULL;
  bool is_list = false;
  if (maybe_parse_text(self, "SIZE")) {
    if (!parse_size_constraint(self, &size_constraint) ||
        !parse_text(self, "OF")) {
      return false;
    }
    is_list = true;
  } else {
    is_list = maybe_parse_text(self, "OF");
  }

  if (is_list) {
    UtObjectRef child_type = NULL;
    if (!parse_type(self, &child_type)) {
      set_error(self, "Missing type for SET OF");
      return false;
    }
    UtObjectRef list_type = ut_asn1_set_of_type_new(child_type);
    if (size_constraint != NULL) {
      *type = ut_asn1_constrained_type_new(list_type, size_constraint);
    } else {
      *type = ut_object_ref(list_type);
    }
    return true;
  }

  if (!parse_text(self, "{")) {
    return false;
  }

  UtObjectRef components = NULL;
  bool extensible = false;
  if (!parse_component_type_list(self, &components, &extensible)) {
    return false;
  }

  UtObjectRef tagged_components = apply_automatic_tagging(self, components);

  UtObjectRef type_ = ut_asn1_set_type_new(tagged_components, extensible);
  if (ut_object_implements_error(type_)) {
    if (self->error == NULL) {
      self->error = ut_object_ref(type_);
    }
    return false;
  }

  *type = ut_object_ref(type_);
  return true;
}

static bool parse_choice_type(UtAsn1ModuleDefinitionParser *self,
                              UtObject **type) {
  if (!parse_text(self, "{")) {
    return false;
  }

  if (maybe_parse_text(self, "}")) {
    set_error(self, "Empty choice");
    return false;
  }

  UtObjectRef components = ut_map_new();
  bool extensible = false;
  bool first_component = true;
  do {
    if (!first_component) {
      if (!parse_text(self, ",")) {
        return false;
      }
    }
    first_component = false;

    if (maybe_parse_text(self, "...")) {
      if (extensible) {
        set_error(self, "Duplicate extensible marker");
        return false;
      }
      if (ut_map_get_length(components) == 0) {
        set_error(self, "Empty choice");
        return false;
      }
      extensible = true;
      continue;
    }

    ut_cstring_ref identifier = NULL;
    UtObjectRef component_type = NULL;
    if (!parse_named_type(self, &identifier, &component_type)) {
      return false;
    }

    UtObject *existing_type = ut_map_lookup_string(components, identifier);
    if (existing_type != NULL) {
      ut_cstring_ref description =
          ut_cstring_new_printf("Duplicate choice option %s", identifier);
      set_error(self, description);
      return false;
    }

    ut_map_insert_string(components, identifier, component_type);
  } while (!maybe_parse_text(self, "}"));

  UtObjectRef tagged_components = apply_automatic_tagging(self, components);

  UtObjectRef type_ = ut_asn1_choice_type_new(
      tagged_components, extensible || self->extensibility_implied);
  if (ut_object_implements_error(type_)) {
    if (self->error == NULL) {
      self->error = ut_object_ref(type_);
    }
    return false;
  }

  *type = ut_object_ref(type_);
  return true;
}

static bool parse_type(UtAsn1ModuleDefinitionParser *self, UtObject **type) {
  UtObjectRef type_ = NULL;
  if (maybe_parse_text(self, "BOOLEAN")) {
    type_ = ut_asn1_boolean_type_new();
  } else if (maybe_parse_text(self, "INTEGER")) {
    type_ = ut_asn1_integer_type_new();
  } else if (maybe_parse_text(self, "BIT")) {
    if (!parse_text(self, "STRING")) {
      return false;
    }
    type_ = ut_asn1_bit_string_type_new();
  } else if (maybe_parse_text(self, "OCTET")) {
    if (!parse_text(self, "STRING")) {
      return false;
    }
    type_ = ut_asn1_octet_string_type_new();
  } else if (maybe_parse_text(self, "NULL")) {
    type_ = ut_asn1_null_type_new();
  } else if (maybe_parse_text(self, "OBJECT")) {
    if (!parse_text(self, "IDENTIFIER")) {
      return false;
    }
    type_ = ut_asn1_object_identifier_type_new();
  } else if (maybe_parse_text(self, "ObjectDescriptor")) {
    type_ = ut_asn1_object_descriptor_type_new();
  } else if (maybe_parse_text(self, "EXTERNAL")) {
    set_error(self, "EXTERNAL type not supported");
    return false;
  } else if (maybe_parse_text(self, "REAL")) {
    type_ = ut_asn1_real_type_new();
  } else if (maybe_parse_text(self, "ENUMERATED")) {
    if (!parse_enumerated_type(self, &type_)) {
      return false;
    }
  } else if (maybe_parse_text(self, "EMBEDDED")) {
    if (!parse_text(self, "PDV")) {
      return false;
    }
    set_error(self, "EMBEDDED PDV type not supported");
    return false;
  } else if (maybe_parse_text(self, "UTF8String")) {
    type_ = ut_asn1_utf8_string_type_new();
  } else if (maybe_parse_text(self, "RELATIVE-OID")) {
    type_ = ut_asn1_relative_oid_type_new();
  } else if (maybe_parse_text(self, "SEQUENCE")) {
    if (!parse_sequence_type(self, &type_)) {
      return false;
    }
  } else if (maybe_parse_text(self, "SET")) {
    if (!parse_set_type(self, &type_)) {
      return false;
    }
  } else if (maybe_parse_text(self, "CHOICE")) {
    if (!parse_choice_type(self, &type_)) {
      return false;
    }
  } else if (maybe_parse_text(self, "[")) {
    UtAsn1TagClass class;
    if (maybe_parse_text(self, "UNIVERSAL")) {
      class = UT_ASN1_TAG_CLASS_UNIVERSAL;
    } else if (maybe_parse_text(self, "APPLICATION")) {
      class = UT_ASN1_TAG_CLASS_APPLICATION;
    } else if (maybe_parse_text(self, "PRIVATE")) {
      class = UT_ASN1_TAG_CLASS_PRIVATE;
    } else {
      class = UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC;
    }

    uint64_t number;
    if (!parse_number(self, &number)) {
      return false;
    }
    if (!parse_text(self, "]")) {
      return false;
    }
    bool is_explicit = self->tag_mode != TAG_MODE_IMPLICIT;
    if (maybe_parse_text(self, "IMPLICIT")) {
      is_explicit = false;
    } else if (maybe_parse_text(self, "EXPLICIT")) {
      is_explicit = true;
    }
    UtObjectRef tagged_type = NULL;
    if (!parse_type(self, &tagged_type)) {
      return false;
    }
    type_ = ut_asn1_tagged_type_new(class, number, is_explicit, tagged_type);
  } else if (maybe_parse_text(self, "NumericString")) {
    type_ = ut_asn1_numeric_string_type_new();
  } else if (maybe_parse_text(self, "PrintableString")) {
    type_ = ut_asn1_printable_string_type_new();
  } else if (maybe_parse_text(self, "TeletexString")) {
    type_ = ut_asn1_teletex_string_type_new();
  } else if (maybe_parse_text(self, "VideotexString")) {
    type_ = ut_asn1_videotex_string_type_new();
  } else if (maybe_parse_text(self, "IA5String")) {
    type_ = ut_asn1_ia5_string_type_new();
  } else if (maybe_parse_text(self, "UTCTime")) {
    type_ = ut_asn1_utc_time_type_new();
  } else if (maybe_parse_text(self, "GeneralizedTime")) {
    type_ = ut_asn1_generalized_time_type_new();
  } else if (maybe_parse_text(self, "GraphicString")) {
    type_ = ut_asn1_graphic_string_type_new();
  } else if (maybe_parse_text(self, "VisibleString")) {
    type_ = ut_asn1_visible_string_type_new();
  } else if (maybe_parse_text(self, "GeneralString")) {
    type_ = ut_asn1_general_string_type_new();
  } else if (maybe_parse_text(self, "UniversalString")) {
    type_ = ut_asn1_universal_string_type_new();
  } else if (maybe_parse_text(self, "CHARACTER")) {
    if (!parse_text(self, "STRING")) {
      return false;
    }
    type_ = ut_asn1_character_string_type_new();
  } else if (maybe_parse_text(self, "BMPString")) {
    type_ = ut_asn1_bmp_string_type_new();
  } else {
    ut_cstring_ref reference = NULL;
    if (!parse_type_reference(self, &reference)) {
      return false;
    }

    UtObject *referenced_type =
        ut_map_lookup_string(self->assignments, reference);
    if (referenced_type == NULL) {
      ut_cstring_ref description =
          ut_cstring_new_printf("Unknown type %s", reference);
      set_error(self, description);
      return false;
    }

    type_ = ut_asn1_referenced_type_new(referenced_type);
  }

  UtObjectRef constraint = NULL;
  if (!parse_constraint(self, type_, &constraint)) {
    return false;
  }

  if (constraint != NULL) {
    *type = ut_asn1_constrained_type_new(type_, constraint);
  } else {
    *type = ut_object_ref(type_);
  }
  return true;
}

static bool parse_named_type(UtAsn1ModuleDefinitionParser *self,
                             char **identifier, UtObject **type) {
  return parse_identifier(self, identifier) && parse_type(self, type);
}

static bool parse_type_assignment(UtAsn1ModuleDefinitionParser *self) {
  ut_cstring_ref reference = NULL;
  UtObjectRef type_ = NULL;
  if (!maybe_parse_type_reference(self, &reference) ||
      !parse_text(self, "::=") || !parse_type(self, &type_)) {
    return false;
  }

  ut_map_insert_string(self->assignments, reference, type_);
  return true;
}

static bool parse_value_assignment(UtAsn1ModuleDefinitionParser *self) {
  ut_cstring_ref reference = NULL;
  UtObjectRef type = NULL;
  UtObjectRef value = NULL;
  if (!maybe_parse_value_reference(self, &reference) ||
      !parse_type(self, &type) || !parse_text(self, "::=")) {
    return false;
  }

  if (!parse_value(self, type, &value)) {
    return false;
  }

  ut_map_insert_string_take(self->assignments, reference,
                            ut_asn1_type_value_new(type, value));
  return true;
}

static bool parse_assignment(UtAsn1ModuleDefinitionParser *self) {
  return parse_type_assignment(self) || parse_value_assignment(self);
}

static bool parse_assignments(UtAsn1ModuleDefinitionParser *self) {
  UtObjectRef assignments_ = ut_map_new();

  while (!current_token_is(self, "END")) {
    if (!parse_assignment(self)) {
      set_expected_error(self, "valueassignment or typeassignment");
      return false;
    }
  }

  return true;
}

static bool parse_module_definition(UtAsn1ModuleDefinitionParser *self,
                                    const char *definition) {
  ut_object_clear(&self->module_definition);

  ut_object_unref(self->tokens);
  self->tokens = tokenize(self, definition);
  if (self->tokens == NULL) {
    return false;
  }

  ut_cstring_ref identifier = NULL;
  UtObjectRef object_identifier = NULL;
  bool export_all;
  UtObjectRef exported_symbols = NULL;
  UtObjectRef imports = NULL;
  if (!parse_module_identifier(self, &identifier, &object_identifier) ||
      !parse_text(self, "DEFINITIONS") || !parse_tag_default(self) ||
      !parse_extension_default(self) || !parse_text(self, "::=") ||
      !parse_text(self, "BEGIN") ||
      !parse_exports(self, &export_all, &exported_symbols) ||
      !parse_imports(self, &imports) || !parse_assignments(self) ||
      !parse_text(self, "END")) {
    return false;
  }

  ut_object_clear(&self->module_definition);
  self->module_definition = ut_asn1_module_definition_new(
      identifier, object_identifier, self->assignments);
  if (export_all) {
    ut_asn1_module_definition_set_export_all(self->module_definition);
  } else {
    ut_asn1_module_definition_set_exports(self->module_definition,
                                          exported_symbols);
  }
  ut_asn1_module_definition_set_imports(self->module_definition, imports);

  return true;
}

static void ut_asn1_module_definition_parser_init(UtObject *object) {
  UtAsn1ModuleDefinitionParser *self = (UtAsn1ModuleDefinitionParser *)object;
  self->assignments = ut_map_new();
}

static void ut_asn1_module_definition_parser_cleanup(UtObject *object) {
  UtAsn1ModuleDefinitionParser *self = (UtAsn1ModuleDefinitionParser *)object;
  ut_object_unref(self->tokens);
  ut_object_unref(self->assignments);
  ut_object_unref(self->module_definition);
  ut_object_unref(self->error);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1ModuleDefinitionParser",
    .init = ut_asn1_module_definition_parser_init,
    .cleanup = ut_asn1_module_definition_parser_cleanup};

UtObject *ut_asn1_module_definition_parser_new() {
  return ut_object_new(sizeof(UtAsn1ModuleDefinitionParser), &object_interface);
}

bool ut_asn1_module_definition_parser_parse(UtObject *object,
                                            const char *text) {
  assert(ut_object_is_asn1_module_definition_parser(object));
  UtAsn1ModuleDefinitionParser *self = (UtAsn1ModuleDefinitionParser *)object;
  return parse_module_definition(self, text);
}

UtObject *
ut_asn1_module_definition_parser_get_module_definition(UtObject *object) {
  assert(ut_object_is_asn1_module_definition_parser(object));
  UtAsn1ModuleDefinitionParser *self = (UtAsn1ModuleDefinitionParser *)object;
  return self->module_definition;
}

UtObject *ut_asn1_module_definition_parser_get_error(UtObject *object) {
  assert(ut_object_is_asn1_module_definition_parser(object));
  UtAsn1ModuleDefinitionParser *self = (UtAsn1ModuleDefinitionParser *)object;
  return self->error;
}

bool ut_object_is_asn1_module_definition_parser(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
