#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;

  // Tokens being parsed.
  UtObject *tokens;
  size_t index;

  // Syntax (2 or 3).
  int syntax;

  // Name of the package.
  char *package_name;

  UtObject *definitions;

  // Decoded definition.
  UtObject *definition;

  // First error that occurred during parsing.
  UtObject *error;
} UtProtobufDefinitionParser;

static void set_error(UtProtobufDefinitionParser *self,
                      const char *description) {
  if (self->error == NULL) {
    self->error = ut_protobuf_error_new(description);
  }
}

static void set_error_take(UtProtobufDefinitionParser *self,
                           char *description) {
  set_error(self, description);
  free(description);
}

static bool is_whitespace(char c) {
  return c == '\t' || c == '\n' || c == '\r' || c == ' ';
}

static bool is_lower_alpha(char c) { return (c >= 'a' && c <= 'z'); }

static bool is_upper_alpha(char c) { return (c >= 'A' && c <= 'Z'); }

static bool is_alpha(char c) { return is_lower_alpha(c) || is_upper_alpha(c); }

static bool is_digit(char c) { return c >= '0' && c <= '9'; }

static bool is_hex_digit(char c) {
  return is_digit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

static int hex_digit(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  } else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  } else {
    return 0;
  }
}

static bool tokenize(UtProtobufDefinitionParser *self, const char *text) {
  ut_object_unref(self->tokens);
  self->tokens = ut_string_list_new();

  const char *c = text;
  while (true) {
    if (*c == '\0') {
      return true;
    } else if (is_whitespace(*c)) {
      // Skip
      c++;
    } else if (*c == '=' || *c == ';' || *c == ',' || *c == '{' || *c == '}' ||
               *c == '(' || *c == ')') {
      ut_string_list_append_printf(self->tokens, "%c", *c);
      c++;
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
    } else if (ut_cstring_starts_with(c, "//")) {
      c += 2;
      while (*c != '\n' && *c != '\0') {
        c++;
      }
    } else if (is_alpha(*c) || *c == '_') {
      const char *start = c;
      do {
        c++;
      } while (is_alpha(*c) || is_digit(*c) || *c == '_' || *c == '.');
      UtObjectRef token = ut_string_new_sized(start, c - start);
      ut_string_list_append(self->tokens, ut_string_get_text(token));
    } else if (is_digit(*c)) {
      const char *start = c;
      do {
        c++;
      } while (is_digit(*c));
      UtObjectRef token = ut_string_new_sized(start, c - start);
      ut_string_list_append(self->tokens, ut_string_get_text(token));
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
      ut_string_list_append(self->tokens, ut_string_get_text(token));
    } else {
      set_error(self, "Invalid character");
      return false;
    }
  }
}

static const char *current_token(UtProtobufDefinitionParser *self) {
  if (self->index >= ut_list_get_length(self->tokens)) {
    return "";
  }
  return ut_string_list_get_element(self->tokens, self->index);
}

static bool current_token_is(UtProtobufDefinitionParser *self,
                             const char *value) {
  return ut_cstring_equal(current_token(self), value);
}

static void set_expected_error(UtProtobufDefinitionParser *self,
                               const char *type) {
  set_error_take(self, ut_cstring_new_printf("Expected %s, got %s", type,
                                             current_token(self)));
}

static bool maybe_parse_text(UtProtobufDefinitionParser *self,
                             const char *value) {
  const char *v = current_token(self);
  if (!ut_cstring_equal(v, value)) {
    return false;
  }

  self->index++;
  return true;
}

static bool parse_text(UtProtobufDefinitionParser *self, const char *value) {
  if (!maybe_parse_text(self, value)) {
    set_expected_error(self, value);
    return false;
  }

  return true;
}

static bool parse_string(UtProtobufDefinitionParser *self, char **value) {
  const char *token = current_token(self);

  if (token[0] != '\"') {
    set_expected_error(self, "string");
    return false;
  }

  UtObjectRef value_ = ut_string_new("");
  const char *start = token + 1;
  const char *c = start;
  while (*c != '\0') {
    if (ut_cstring_starts_with(c, "\\")) {
      c++;
      switch (*c) {
      case 'r':
        ut_string_append(value_, "\r");
        break;
      case 'n':
        ut_string_append(value_, "\n");
        break;
      case 't':
        ut_string_append(value_, "\t");
        break;
      case '\\':
        ut_string_append(value_, "\\");
        break;
      case '\"':
        ut_string_append(value_, "\"");
        break;
      case 'x':
        if (!is_hex_digit(c[1]) || !is_hex_digit(c[2])) {
          set_error(self, "Invalid octet escape sequence");
          return false;
        }
        ut_string_append_printf(value_, "%c",
                                hex_digit(c[1]) << 4 | hex_digit(c[2]));
        c += 2;
        break;
      case 'u':
        if (!is_hex_digit(c[1]) || !is_hex_digit(c[2]) || !is_hex_digit(c[3]) ||
            !is_hex_digit(c[4])) {
          set_error(self, "Invalid unicode escape sequence");
          return false;
        }
        ut_string_append_code_point(
            value_, hex_digit(c[1]) << 12 | hex_digit(c[2]) << 8 |
                        hex_digit(c[3]) << 4 | hex_digit(c[4]));
        c += 4;
        break;
      case 'U':
        if (!is_hex_digit(c[1]) || !is_hex_digit(c[2]) || !is_hex_digit(c[3]) ||
            !is_hex_digit(c[4]) || !is_hex_digit(c[4]) || !is_hex_digit(c[5]) ||
            !is_hex_digit(c[6]) || !is_hex_digit(c[7])) {
          set_error(self, "Invalid unicode escape sequence");
          return false;
        }
        ut_string_append_code_point(
            value_, hex_digit(c[1]) << 28 | hex_digit(c[2]) << 24 |
                        hex_digit(c[3]) << 20 | hex_digit(c[4]) << 16 |
                        hex_digit(c[5]) << 12 | hex_digit(c[6]) << 8 |
                        hex_digit(c[7]) << 4 | hex_digit(c[8]));
        c += 8;
        break;
      default:
        set_error(self, "Invalid escape sequence");
        return false;
      }
    } else if (*c == '\"') {
      // Unterminated string.
      if (c[1] != '\0') {
        set_error(self, "Data after string end");
        return false;
      }
      ut_string_append_sized(value_, start, c - start);
      self->index++;
      *value = ut_string_take_text(value_);
      return true;
    } else {
      c++;
    }
  }

  set_error(self, "Unterminated string");
  return false;
}

static bool parse_field_type(UtProtobufDefinitionParser *self,
                             UtProtobufMessageFieldType *type) {
  if (maybe_parse_text(self, "optional")) {
    *type = UT_PROTOBUF_MESSAGE_FIELD_TYPE_OPTIONAL;
  } else if (maybe_parse_text(self, "repeated")) {
    *type = UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED;
  } else if (maybe_parse_text(self, "required")) {
    if (self->syntax == 3) {
      set_error(self, "Required fields are not allowed in proto3");
      return false;
    }
    *type = UT_PROTOBUF_MESSAGE_FIELD_TYPE_REQUIRED;
  } else {
    if (self->syntax == 2) {
      set_expected_error(self, "required, optional or repeated");
      return false;
    }
    *type = UT_PROTOBUF_MESSAGE_FIELD_TYPE_IMPLICIT;
  }

  return true;
}

static bool parse_type(UtProtobufDefinitionParser *self, UtObject **type) {
  const char *v = current_token(self);

  if (ut_cstring_equal(v, "double")) {
    *type = ut_protobuf_primitive_type_new_double();
  } else if (ut_cstring_equal(v, "float")) {
    *type = ut_protobuf_primitive_type_new_float();
  } else if (ut_cstring_equal(v, "int32")) {
    *type = ut_protobuf_primitive_type_new_int32();
  } else if (ut_cstring_equal(v, "int64")) {
    *type = ut_protobuf_primitive_type_new_int64();
  } else if (ut_cstring_equal(v, "uint32")) {
    *type = ut_protobuf_primitive_type_new_uint32();
  } else if (ut_cstring_equal(v, "uint64")) {
    *type = ut_protobuf_primitive_type_new_uint64();
  } else if (ut_cstring_equal(v, "sint32")) {
    *type = ut_protobuf_primitive_type_new_sint32();
  } else if (ut_cstring_equal(v, "sint64")) {
    *type = ut_protobuf_primitive_type_new_sint64();
  } else if (ut_cstring_equal(v, "fixed32")) {
    *type = ut_protobuf_primitive_type_new_fixed32();
  } else if (ut_cstring_equal(v, "fixed64")) {
    *type = ut_protobuf_primitive_type_new_fixed64();
  } else if (ut_cstring_equal(v, "sfixed32")) {
    *type = ut_protobuf_primitive_type_new_sfixed32();
  } else if (ut_cstring_equal(v, "sfixed64")) {
    *type = ut_protobuf_primitive_type_new_sfixed64();
  } else if (ut_cstring_equal(v, "bool")) {
    *type = ut_protobuf_primitive_type_new_bool();
  } else if (ut_cstring_equal(v, "string")) {
    *type = ut_protobuf_primitive_type_new_string();
  } else if (ut_cstring_equal(v, "bytes")) {
    *type = ut_protobuf_primitive_type_new_bytes();
  } else {
    *type = ut_protobuf_referenced_type_new(v);
  }

  self->index++;
  return true;
}

static bool parse_import(UtProtobufDefinitionParser *self) {
  ut_cstring_ref path = NULL;
  if (!parse_text(self, "import") || !parse_string(self, &path) ||
      !parse_text(self, ";")) {
    return false;
  }

  set_error(self, "Imports not implemented");
  return false;
}

static bool is_identifier_starting_char(char c) {
  return is_alpha(c) || c == '_';
}

static bool is_identifier_char(char c) {
  return is_alpha(c) || is_digit(c) || c == '_';
}

static bool is_valid_identifier(const char *name) {
  if (!is_identifier_starting_char(name[0])) {
    return false;
  }

  for (size_t i = 1; name[i] != '\0'; i++) {
    if (!is_identifier_char(name[i])) {
      return false;
    }
  }

  return true;
}

static bool is_valid_namespaced_identifier(const char *name) {
  size_t offset = 0;
  while (true) {
    if (!is_identifier_starting_char(name[offset])) {
      return false;
    }
    offset++;

    while (is_identifier_char(name[offset])) {
      offset++;
    }

    if (name[offset] == '\0') {
      return true;
    }

    if (name[offset] != '.') {
      return false;
    }
    offset++;
  }
}

static bool parse_identifier(UtProtobufDefinitionParser *self,
                             const char *type_name, char **value) {
  const char *v = current_token(self);

  if (!is_valid_identifier(v)) {
    set_error_take(self,
                   ut_cstring_new_printf("Invalid %s \"%s\"", type_name, v));
    return false;
  }

  self->index++;
  *value = ut_cstring_new(v);
  return true;
}

static bool parse_namespaced_identifier(UtProtobufDefinitionParser *self,
                                        const char *type_name, char **value) {
  const char *v = current_token(self);

  if (!is_valid_namespaced_identifier(v)) {
    set_error_take(self,
                   ut_cstring_new_printf("Invalid %s \"%s\"", type_name, v));
    return false;
  }

  self->index++;
  *value = ut_cstring_new(v);
  return true;
}

static bool parse_package(UtProtobufDefinitionParser *self) {
  if (self->package_name != NULL) {
    set_error(self, "Multiple package definitions");
    return false;
  }

  if (!parse_text(self, "package") ||
      !parse_namespaced_identifier(self, "package name", &self->package_name) ||
      !parse_text(self, ";")) {
    return false;
  }

  return true;
}

static bool parse_message_name(UtProtobufDefinitionParser *self, char **name) {
  return parse_identifier(self, "message name", name);
}

static bool parse_field_name(UtProtobufDefinitionParser *self, char **name) {
  return parse_identifier(self, "field name", name);
}

static bool parse_field_number(UtProtobufDefinitionParser *self, int *number) {
  const char *v = current_token(self);

  int number_ = 0;
  for (size_t i = 0; v[i] != '\0'; i++) {
    if (!is_digit(v[i])) {
      set_error(self, "Invalid character in number");
      return false;
    }
    number_ = number_ * 10 + (v[i] - '0');
  }

  if (number_ < 1 || number_ > 0x1fffffff) {
    set_error_take(self,
                   ut_cstring_new_printf("Invalid field number %d", number_));
    return false;
  }

  self->index++;
  *number = number_;
  return true;
}

static bool field_name_is_reserved(UtObject *reserved_names, const char *name) {
  size_t reserved_names_length = ut_list_get_length(reserved_names);
  for (size_t i = 0; i < reserved_names_length; i++) {
    if (ut_cstring_equal(ut_string_list_get_element(reserved_names, i), name)) {
      return true;
    }
  }

  return false;
}

static bool field_number_is_reserved(UtObject *reserved_numbers,
                                     uint32_t number) {
  size_t reserved_numbers_length = ut_list_get_length(reserved_numbers);
  for (size_t i = 0; i < reserved_numbers_length; i++) {
    if (ut_uint32_list_get_element(reserved_numbers, i) == number) {
      return true;
    }
  }

  return false;
}

static bool check_reserved_fields(UtProtobufDefinitionParser *self,
                                  UtObject *fields, UtObject *reserved_names,
                                  UtObject *reserved_numbers) {
  UtObjectRef field_items = ut_map_get_items(fields);
  size_t field_items_length = ut_list_get_length(field_items);
  for (size_t i = 0; i < field_items_length; i++) {
    UtObject *item = ut_object_list_get_element(field_items, i);
    const char *name = ut_string_get_text(ut_map_item_get_key(item));
    UtObject *field = ut_map_item_get_value(item);

    uint32_t number = ut_protobuf_message_field_get_number(field);
    if (field_name_is_reserved(reserved_names, name)) {
      set_error_take(
          self, ut_cstring_new_printf("Field name \"%s\" is reserved", name));
      return false;
    }
    if (field_number_is_reserved(reserved_numbers, number)) {
      set_error_take(
          self, ut_cstring_new_printf("Field \"%s\" uses reserved number %u",
                                      name, number));
      return false;
    }
  }

  return true;
}

static bool add_definition(UtProtobufDefinitionParser *self, const char *name,
                           UtObject *value) {
  if (ut_map_lookup_string(self->definitions, name) != NULL) {
    set_error_take(self,
                   ut_cstring_new_printf("\"%s\" is already defined", name));
    return false;
  }

  ut_map_insert_string(self->definitions, name, value);
  return true;
}

static bool parse_message(UtProtobufDefinitionParser *self,
                          const char *parent_name) {
  ut_cstring_ref message_name = NULL;
  if (!parse_text(self, "message") ||
      !parse_message_name(self, &message_name) || !parse_text(self, "{")) {
    return false;
  }

  ut_cstring_ref full_name = NULL;
  if (parent_name != NULL) {
    full_name = ut_cstring_new_printf("%s.%s", parent_name, message_name);
  } else {
    full_name = ut_cstring_new(message_name);
  }

  UtObjectRef fields = ut_map_new();
  UtObjectRef reserved_names = ut_string_list_new();
  UtObjectRef reserved_numbers = ut_uint32_list_new();
  while (true) {
    if (maybe_parse_text(self, "}")) {
      if (!check_reserved_fields(self, fields, reserved_names,
                                 reserved_numbers)) {
        return false;
      }

      UtObjectRef message_type = ut_protobuf_message_type_new(fields);
      if (!add_definition(self, full_name, message_type)) {
        return false;
      }

      return true;
    }

    if (maybe_parse_text(self, "reserved")) {
      ut_cstring_ref name = NULL;
      int number = 0;
      if (current_token(self)[0] == '\"' && parse_string(self, &name)) {
        ut_string_list_append(reserved_names, name);
        while (!maybe_parse_text(self, ";")) {
          ut_cstring_ref name = NULL;
          if (!parse_text(self, ",") || !parse_string(self, &name)) {
            return false;
          }
          ut_string_list_append(reserved_names, name);
        }
      } else if (parse_field_number(self, &number)) {
        ut_uint32_list_append(reserved_numbers, number);
        while (!maybe_parse_text(self, ";")) {
          int number;
          if (!parse_text(self, ",") || !parse_field_number(self, &number)) {
            return false;
          }
          ut_uint32_list_append(reserved_numbers, number);
        }
      } else {
        set_expected_error(self, "field name or number");
        return false;
      }

      continue;
    }

    if (current_token_is(self, "message")) {
      if (!parse_message(self, full_name)) {
        return false;
      }

      continue;
    }

    UtProtobufMessageFieldType field_type;
    UtObjectRef type = NULL;
    ut_cstring_ref name = NULL;
    int number;
    if (!parse_field_type(self, &field_type) || !parse_type(self, &type) ||
        !parse_field_name(self, &name) || !parse_text(self, "=") ||
        !parse_field_number(self, &number) || !parse_text(self, ";")) {
      return false;
    }

    ut_map_insert_string_take(
        fields, name, ut_protobuf_message_field_new(field_type, type, number));
  }

  return false;
}

static bool parse_enum_name(UtProtobufDefinitionParser *self, char **name) {
  return parse_identifier(self, "enum name", name);
}

static bool parse_enum_value_name(UtProtobufDefinitionParser *self,
                                  char **name) {
  return parse_identifier(self, "enum value name", name);
}

static bool parse_enum_value(UtProtobufDefinitionParser *self, int *number) {
  const char *v = current_token(self);

  int sign = 1;
  size_t start = 0;
  if (v[0] == '-') {
    sign = -1;
    start = 1;
  }

  int number_ = 0;
  for (size_t i = start; v[i] != '\0'; i++) {
    if (!is_digit(v[i])) {
      set_error(self, "Invalid character in number");
      return false;
    }
    number_ = number_ * 10 + (v[i] - '0');
  }

  self->index++;
  *number = sign * number_;
  return true;
}

static bool parse_enum(UtProtobufDefinitionParser *self) {
  ut_cstring_ref enum_name = NULL;
  if (!parse_text(self, "enum") || !parse_enum_name(self, &enum_name) ||
      !parse_text(self, "{")) {
    return false;
  }

  UtObjectRef values_by_name = ut_map_new();
  while (true) {
    if (maybe_parse_text(self, "}")) {
      if (ut_map_get_length(values_by_name) == 0) {
        set_error(self, "Enums must contain at least one value");
        return false;
      }

      UtObjectRef enum_type = ut_protobuf_enum_type_new(values_by_name);
      if (!add_definition(self, enum_name, enum_type)) {
        return false;
      }

      return true;
    }

    UtObjectRef type = NULL;
    ut_cstring_ref name = NULL;
    int number;
    if (!parse_enum_value_name(self, &name) || !parse_text(self, "=") ||
        !parse_enum_value(self, &number) || !parse_text(self, ";")) {
      return false;
    }

    if (self->syntax == 3 && ut_map_get_length(values_by_name) == 0 &&
        number != 0) {
      set_error(self, "The first enum value must be zero in proto3");
      return false;
    }

    if (ut_map_lookup_string(values_by_name, name) != NULL) {
      set_error_take(self,
                     ut_cstring_new_printf("\"%s\" is already defined", name));
      return false;
    }

    ut_map_insert_string_take(values_by_name, name, ut_int32_new(number));
  }

  return false;
}

static bool parse_service_name(UtProtobufDefinitionParser *self, char **name) {
  return parse_identifier(self, "service name", name);
}

static bool parse_method_name(UtProtobufDefinitionParser *self, char **name) {
  return parse_identifier(self, "method name", name);
}

static bool parse_service(UtProtobufDefinitionParser *self) {
  ut_cstring_ref service_name = NULL;
  if (!parse_text(self, "service") ||
      !parse_service_name(self, &service_name) || !parse_text(self, "{")) {
    return false;
  }

  UtObjectRef method_calls = ut_map_new();
  while (true) {
    if (maybe_parse_text(self, "}")) {
      UtObjectRef service = ut_protobuf_service_new(method_calls);
      if (!add_definition(self, service_name, service)) {
        return false;
      }

      return true;
    }

    ut_cstring_ref method_name = NULL;
    ut_cstring_ref request_name = NULL;
    ut_cstring_ref response_name = NULL;
    if (!parse_text(self, "rpc") || !parse_method_name(self, &method_name) ||
        !parse_text(self, "(") || !parse_message_name(self, &request_name) ||
        !parse_text(self, ")") || !parse_text(self, "returns") ||
        !parse_text(self, "(") || !parse_message_name(self, &response_name) ||
        !parse_text(self, ")") || !parse_text(self, ";")) {
      return false;
    }

    UtObjectRef request_type = ut_protobuf_referenced_type_new(request_name);
    UtObjectRef response_type = ut_protobuf_referenced_type_new(response_name);
    ut_map_insert_string_take(
        method_calls, method_name,
        ut_protobuf_method_call_new(request_type, response_type));
  }

  return false;
}

static void add_package_to_definitions(UtProtobufDefinitionParser *self) {
  if (self->package_name == NULL) {
    return;
  }

  UtObjectRef unprefixed_definitions = self->definitions;
  self->definitions = ut_map_new();

  UtObjectRef items = ut_map_get_items(unprefixed_definitions);
  size_t items_length = ut_list_get_length(items);
  for (size_t i = 0; i < items_length; i++) {
    UtObject *item = ut_object_list_get_element(items, i);
    const char *name = ut_string_get_text(ut_map_item_get_key(item));
    UtObject *type = ut_map_item_get_value(item);

    ut_cstring_ref prefixed_name =
        ut_cstring_new_printf("%s.%s", self->package_name, name);
    ut_map_insert_string(self->definitions, prefixed_name, type);
  }
}

static void strip_prefix(char *prefix) {
  size_t end = 0;
  for (size_t i = 0; prefix[i] != '\0'; i++) {
    if (prefix[i] == '.') {
      end = i;
    }
  }
  prefix[end] = '\0';
}

static bool resolve_type(UtProtobufDefinitionParser *self,
                         const char *message_name, UtObject *type) {
  if (!ut_object_is_protobuf_referenced_type(type)) {
    return true;
  }

  const char *type_name = ut_protobuf_referenced_type_get_name(type);
  UtObject *resolved_type = NULL;
  if (message_name != NULL) {
    ut_cstring_ref prefix = ut_cstring_new(message_name);
    do {
      ut_cstring_ref name = ut_cstring_new_printf("%s.%s", prefix, type_name);
      resolved_type = ut_map_lookup_string(self->definitions, name);
      strip_prefix(prefix);
    } while (resolved_type == NULL && prefix[0] != '\0');
  }
  if (resolved_type == NULL) {
    resolved_type = ut_map_lookup_string(self->definitions, type_name);
  }

  if (resolved_type == NULL) {
    set_error_take(self,
                   ut_cstring_new_printf("\"%s\" is not defined", type_name));
    return false;
  }

  ut_protobuf_referenced_type_set_type(type, resolved_type);
  return true;
}

static bool resolve_message(UtProtobufDefinitionParser *self, const char *name,
                            UtObject *message) {
  UtObjectRef fields =
      ut_map_get_values(ut_protobuf_message_type_get_fields(message));
  size_t fields_length = ut_list_get_length(fields);
  for (size_t i = 0; i < fields_length; i++) {
    UtObject *field = ut_object_list_get_element(fields, i);
    if (!resolve_type(self, name,
                      ut_protobuf_message_field_get_value_type(field))) {
      return false;
    }
  }

  return true;
}

static bool resolve_service(UtProtobufDefinitionParser *self,
                            UtObject *service) {
  UtObject *method_calls = ut_protobuf_service_get_method_calls(service);
  UtObjectRef items = ut_map_get_items(method_calls);
  size_t items_length = ut_list_get_length(items);
  for (size_t i = 0; i < items_length; i++) {
    UtObject *item = ut_object_list_get_element(items, i);
    UtObject *method_call = ut_map_item_get_value(item);

    if (!resolve_type(self, NULL,
                      ut_protobuf_method_call_get_request_type(method_call))) {
      return false;
    }
    if (!resolve_type(self, NULL,
                      ut_protobuf_method_call_get_response_type(method_call))) {
      return false;
    }
  }

  return true;
}

static bool resolve_references(UtProtobufDefinitionParser *self) {
  UtObjectRef items = ut_map_get_items(self->definitions);
  size_t items_length = ut_list_get_length(items);
  for (size_t i = 0; i < items_length; i++) {
    UtObject *item = ut_object_list_get_element(items, i);
    const char *name = ut_string_get_text(ut_map_item_get_key(item));
    UtObject *type = ut_map_item_get_value(item);
    if (ut_object_is_protobuf_message_type(type)) {
      if (!resolve_message(self, name, type)) {
        return false;
      }
    } else if (ut_object_is_protobuf_service(type)) {
      if (!resolve_service(self, type)) {
        return false;
      }
    }
  }

  return true;
}

static void ut_protobuf_definition_parser_cleanup(UtObject *object) {
  UtProtobufDefinitionParser *self = (UtProtobufDefinitionParser *)object;
  ut_object_unref(self->tokens);
  free(self->package_name);
  ut_object_unref(self->definitions);
  ut_object_unref(self->definition);
  ut_object_unref(self->error);
}

static UtObjectInterface object_interface = {
    .type_name = "UtProtobufDefinitionParser",
    .cleanup = ut_protobuf_definition_parser_cleanup};

UtObject *ut_protobuf_definition_parser_new() {
  return ut_object_new(sizeof(UtProtobufDefinitionParser), &object_interface);
}

bool ut_protobuf_definition_parser_parse(UtObject *object, const char *text) {
  assert(ut_object_is_protobuf_definition_parser(object));
  UtProtobufDefinitionParser *self = (UtProtobufDefinitionParser *)object;

  if (!tokenize(self, text)) {
    return false;
  }

  ut_cstring_set(&self->package_name, NULL);
  ut_object_clear(&self->definition);
  ut_object_unref(self->definitions);
  self->definitions = ut_map_new();

  if (maybe_parse_text(self, "syntax")) {
    ut_cstring_ref value = NULL;
    if (!parse_text(self, "=") || !parse_string(self, &value) ||
        !parse_text(self, ";")) {
      return false;
    }

    if (ut_cstring_equal(value, "proto2")) {
      self->syntax = 2;
    } else if (ut_cstring_equal(value, "proto3")) {
      self->syntax = 3;
    } else {
      set_error(self, "Unrecognised syntax");
      return false;
    }
  } else {
    self->syntax = 2;
  }

  while (true) {
    if (current_token_is(self, "")) {
      add_package_to_definitions(self);

      if (!resolve_references(self)) {
        return false;
      }

      self->definition =
          ut_protobuf_definition_new(self->syntax, self->definitions);
      return true;
    } else if (current_token_is(self, "import")) {
      if (!parse_import(self)) {
        return false;
      }
    } else if (current_token_is(self, "package")) {
      if (!parse_package(self)) {
        return false;
      }
    } else if (current_token_is(self, "message")) {
      if (!parse_message(self, NULL)) {
        return false;
      }
    } else if (current_token_is(self, "enum")) {
      if (!parse_enum(self)) {
        return false;
      }
    } else if (current_token_is(self, "service")) {
      if (!parse_service(self)) {
        return false;
      }
    } else {
      set_expected_error(self, "message, enum or service");
      return false;
    }
  }
}

UtObject *ut_protobuf_definition_parser_get_definition(UtObject *object) {
  assert(ut_object_is_protobuf_definition_parser(object));
  UtProtobufDefinitionParser *self = (UtProtobufDefinitionParser *)object;
  return self->definition;
}

UtObject *ut_protobuf_definition_parser_get_error(UtObject *object) {
  assert(ut_object_is_protobuf_definition_parser(object));
  UtProtobufDefinitionParser *self = (UtProtobufDefinitionParser *)object;
  return self->error;
}

bool ut_object_is_protobuf_definition_parser(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
