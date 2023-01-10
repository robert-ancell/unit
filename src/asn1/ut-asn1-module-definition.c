#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *identifier;
  UtObject *type_assignments;
  UtObject *value_assignments;
} UtAsn1ModuleDefinition;

static bool is_whitespace(char c) {
  return c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' ||
         c == ' ';
}

static bool is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool is_digit(char c) { return c >= '0' && c <= '9'; }

static UtObject *tokenize(const char *definition) {
  UtObjectRef tokens = ut_string_list_new();

  const char *c = definition;
  while (true) {
    if (*c == '\0') {
      return ut_object_ref(tokens);
    } else if (is_whitespace(*c)) {
      // Skip
      c++;
    } else if (*c == '{' || *c == '}' || *c == '(' || *c == ')' || *c == '[' ||
               *c == ']' || *c == ',' || *c == ';') {
      ut_string_list_append_printf(tokens, "%c", *c);
      c++;
    } else if (ut_cstring_starts_with(c, "::=")) {
      ut_string_list_append(tokens, "::=");
      c += 3;
    } else if (ut_cstring_starts_with(c, "...")) {
      ut_string_list_append(tokens, "...");
      c += 3;
    } else if (is_alpha(*c)) {
      UtObjectRef token = ut_string_new("");
      do {
        ut_string_append_code_point(token, *c);
        c++;
      } while (is_alpha(*c) || is_digit(*c));
      ut_string_list_append(tokens, ut_string_get_text(token));
    } else if (is_digit(*c)) {
      UtObjectRef token = ut_string_new("");
      do {
        ut_string_append_code_point(token, *c);
        c++;
      } while (is_digit(*c));
      ut_string_list_append(tokens, ut_string_get_text(token));
    } else {
      assert(false);
    }
  }
}

static const char *current_token(UtObject *tokens, size_t *index) {
  if (*index >= ut_list_get_length(tokens)) {
    return "";
  }
  return ut_string_list_get_element(tokens, *index);
}

static bool parse_number(UtObject *tokens, size_t *index, unsigned int *value) {
  // const char *number = current_token(tokens, index);
  *value = 0; // FIXME
  (*index)++;
  return true;
}

static bool parse_module_identifier(UtAsn1ModuleDefinition *self,
                                    UtObject *tokens, size_t *index) {
  const char *identifier = current_token(tokens, index);
  (*index)++;
  // FIXME: Validate
  // FIXME: definitive identifier
  self->identifier = ut_cstring_new(identifier);
  return true;
}

static bool parse_text(UtObject *tokens, size_t *index, const char *value) {
  const char *v = current_token(tokens, index);
  if (ut_cstring_equal(v, value)) {
    (*index)++;
    return true;
  } else {
    return false;
  }
}

static bool parse_tag_default(UtAsn1ModuleDefinition *self, UtObject *tokens,
                              size_t *index) {
  if (parse_text(tokens, index, "EXPLICIT")) {
    return parse_text(tokens, index, "TAGS");
  } else if (parse_text(tokens, index, "IMPLICIT")) {
    return parse_text(tokens, index, "TAGS");
  } else if (parse_text(tokens, index, "AUTOMATIC")) {
    return parse_text(tokens, index, "TAGS");
  } else {
    return true;
  }
}

static bool parse_extension_default(UtAsn1ModuleDefinition *self,
                                    UtObject *tokens, size_t *index) {
  if (parse_text(tokens, index, "EXTENSIBILITY")) {
    return parse_text(tokens, index, "IMPLIED");
  } else {
    return true;
  }
}

static bool parse_exports(UtAsn1ModuleDefinition *self, UtObject *tokens,
                          size_t *index) {
  if (!parse_text(tokens, index, "EXPORTS")) {
    return true;
  }

  if (parse_text(tokens, index, "ALL")) {
    return parse_text(tokens, index, ";");
  }

  // FIXME: SymbolsExported
  return parse_text(tokens, index, ";");
}

static bool parse_imports(UtAsn1ModuleDefinition *self, UtObject *tokens,
                          size_t *index) {
  if (!parse_text(tokens, index, "IMPORTS")) {
    return true;
  }

  // FIXME: SymbolsImported
  return parse_text(tokens, index, ";");
}

static bool parse_type_reference(UtObject *tokens, size_t *index,
                                 char **reference) {
  *reference = ut_cstring_new(current_token(tokens, index));
  (*index)++;
  return true;
}

static bool parse_named_type(UtObject *tokens, size_t *index, char **identifier,
                             UtObject **type);

static bool parse_value(UtObject *tokens, size_t *index) { return false; }

static bool parse_type(UtObject *tokens, size_t *index, UtObject **type) {
  if (parse_text(tokens, index, "BOOLEAN")) {
    *type = ut_asn1_boolean_type_new();
    return true;
  } else if (parse_text(tokens, index, "BMPString")) {
    *type = ut_asn1_string_type_new();
    return true;
  } else if (parse_text(tokens, index, "GeneralString")) {
    *type = ut_asn1_string_type_new();
    return true;
  } else if (parse_text(tokens, index, "IA5String")) {
    *type = ut_asn1_string_type_new();
    return true;
  } else if (parse_text(tokens, index, "PrintableString")) {
    *type = ut_asn1_string_type_new();
    return true;
  } else if (parse_text(tokens, index, "UTF8String")) {
    *type = ut_asn1_string_type_new();
    return true;
  } else if (parse_text(tokens, index, "INTEGER")) {
    *type = ut_asn1_integer_type_new();
    return true;
  } else if (parse_text(tokens, index, "NULL")) {
    *type = ut_asn1_null_type_new();
    return true;
  } else if (parse_text(tokens, index, "OCTET")) {
    assert(parse_text(tokens, index, "STRING"));
    *type = ut_asn1_octet_string_type_new();
    return true;
  } else if (parse_text(tokens, index, "SEQUENCE")) {
    if (parse_text(tokens, index, "OF")) {
      UtObjectRef child_type = NULL;
      assert(parse_type(tokens, index, &child_type));
      *type = ut_asn1_sequence_of_type_new(child_type);
      return true;
    }

    assert(parse_text(tokens, index, "{"));
    UtObjectRef components = ut_object_list_new();
    while (true) {
      if (ut_cstring_equal(current_token(tokens, index), "}")) {
        assert(parse_text(tokens, index, "}"));
        *type = ut_asn1_sequence_type_new(components, false);
        return true;
      }

      if (ut_list_get_length(components) > 0) {
        assert(parse_text(tokens, index, ","));
      }

      if (parse_text(tokens, index, "...")) {
        assert(parse_text(tokens, index, "}"));
        *type = ut_asn1_sequence_type_new(components, true);
        return true;
      }

      ut_cstring_ref identifier = NULL;
      UtObjectRef component_type = NULL;
      assert(parse_named_type(tokens, index, &identifier, &component_type));
      if (parse_text(tokens, index, "OPTIONAL")) {
      } else if (parse_text(tokens, index, "DEFAULT")) {
        assert(parse_value(tokens, index));
      }
      ut_list_append(components, component_type);
    }
    return false;
  } else if (parse_text(tokens, index, "SET")) {
    if (parse_text(tokens, index, "OF")) {
      UtObjectRef child_type = NULL;
      assert(parse_type(tokens, index, &child_type));
      assert(false);
      return true;
    }

    assert(false);
    return true;
  } else if (parse_text(tokens, index, "[")) {
    // FIXME: Class
    unsigned int number;
    assert(parse_number(tokens, index, &number));
    assert(parse_text(tokens, index, "]"));
    if (parse_text(tokens, index, "IMPLICIT") == 0) {
      // FIXME
    } else if (parse_text(tokens, index, "EXPLICIT") == 0) {
      // FIXME
    }
    UtObjectRef tagged_type = NULL;
    assert(parse_type(tokens, index, &tagged_type));
    *type = ut_asn1_tagged_type_new(number, tagged_type);
    return true;
  } else {
    ut_cstring_ref reference = NULL;
    assert(parse_type_reference(tokens, index, &reference));
    *type = ut_string_new(reference); // FIXME
    return true;
  }
}

static bool parse_identifier(UtObject *tokens, size_t *index,
                             char **identifier) {
  *identifier = ut_cstring_new(current_token(tokens, index));
  (*index)++;
  return true;
}

static bool parse_named_type(UtObject *tokens, size_t *index, char **identifier,
                             UtObject **type) {
  assert(parse_identifier(tokens, index, identifier));
  assert(parse_type(tokens, index, type));
  return true;
}

static bool parse_type_assignment(UtAsn1ModuleDefinition *self,
                                  UtObject *tokens, size_t *index) {
  ut_cstring_ref reference = NULL;
  assert(parse_type_reference(tokens, index, &reference));
  assert(parse_text(tokens, index, "::="));
  UtObjectRef type = NULL;
  assert(parse_type(tokens, index, &type));

  UtObjectRef assignment = ut_asn1_type_assignment_new(reference, type);
  ut_list_append(self->type_assignments, assignment);

  return true;
}

static bool parse_value_reference(UtObject *tokens, size_t *index,
                                  char **reference) {
  *reference = ut_cstring_new(current_token(tokens, index));
  (*index)++;
  return true;
}

static bool parse_value_assignment(UtAsn1ModuleDefinition *self,
                                   UtObject *tokens, size_t *index) {
  ut_cstring_ref reference = NULL;
  assert(parse_value_reference(tokens, index, &reference));
  UtObjectRef type = NULL;
  assert(parse_type(tokens, index, &type));
  assert(parse_text(tokens, index, "::="));
  assert(parse_value(tokens, index));

  UtObjectRef assignment = ut_asn1_value_assignment_new(reference, type, NULL);
  ut_list_append(self->value_assignments, assignment);

  return true;
}

static bool parse_assignment(UtAsn1ModuleDefinition *self, UtObject *tokens,
                             size_t *index) {
  return parse_type_assignment(self, tokens, index) ||
         parse_value_assignment(self, tokens, index);
}

static void ut_asn1_module_definition_init(UtObject *object) {
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  self->type_assignments = ut_object_list_new();
  self->value_assignments = ut_object_list_new();
}

static void ut_asn1_module_definition_cleanup(UtObject *object) {
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  free(self->identifier);
  ut_object_unref(self->type_assignments);
  ut_object_unref(self->value_assignments);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1ModuleDefinition",
    .init = ut_asn1_module_definition_init,
    .cleanup = ut_asn1_module_definition_cleanup,
    .interfaces = {{NULL, NULL}}};

UtObject *ut_asn1_module_definition_new(const char *definition) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1ModuleDefinition), &object_interface);
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;

  UtObjectRef tokens = tokenize(definition);

  size_t index = 0;
  assert(parse_module_identifier(self, tokens, &index));
  assert(parse_text(tokens, &index, "DEFINITIONS"));
  assert(parse_tag_default(self, tokens, &index));
  assert(parse_extension_default(self, tokens, &index));
  assert(parse_text(tokens, &index, "::="));
  assert(parse_text(tokens, &index, "BEGIN"));
  assert(parse_exports(self, tokens, &index));
  assert(parse_imports(self, tokens, &index));
  while (!ut_cstring_equal(current_token(tokens, &index), "END")) {
    assert(parse_assignment(self, tokens, &index));
  }
  assert(parse_text(tokens, &index, "END"));

  return object;
}

const char *ut_asn1_module_definition_get_identifier(UtObject *object) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  return self->identifier;
}

UtObject *ut_asn1_module_definition_get_type_assignments(UtObject *object) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  return self->type_assignments;
}

UtObject *ut_asn1_module_definition_get_value_assignments(UtObject *object) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  return self->value_assignments;
}

bool ut_object_is_asn1_module_definition(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
