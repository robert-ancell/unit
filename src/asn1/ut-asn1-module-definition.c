#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *identifier;
  UtObject *object_identifier;
  UtObject *exports;
  bool export_all;
  UtObject *imports;
  UtObject *assignments;
} UtAsn1ModuleDefinition;

static void ut_asn1_module_definition_init(UtObject *object) {
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  self->exports = ut_string_list_new();
  self->imports = ut_object_list_new();
}

static void ut_asn1_module_definition_cleanup(UtObject *object) {
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  free(self->identifier);
  ut_object_unref(self->object_identifier);
  ut_object_unref(self->exports);
  ut_object_unref(self->imports);
  ut_object_unref(self->assignments);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1ModuleDefinition",
    .init = ut_asn1_module_definition_init,
    .cleanup = ut_asn1_module_definition_cleanup};

UtObject *ut_asn1_module_definition_new(const char *identifier,
                                        UtObject *object_identifier,
                                        UtObject *assignments) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1ModuleDefinition), &object_interface);
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;

  self->identifier = ut_cstring_new(identifier);
  self->object_identifier = ut_object_ref(object_identifier);
  self->assignments =
      assignments != NULL ? ut_object_ref(assignments) : ut_map_new();

  return object;
}

UtObject *ut_asn1_module_definition_new_from_text(const char *text) {
  UtObjectRef parser = ut_asn1_module_definition_parser_new();
  if (!ut_asn1_module_definition_parser_parse(parser, text)) {
    return ut_object_ref(ut_asn1_module_definition_parser_get_error(parser));
  }

  return ut_object_ref(
      ut_asn1_module_definition_parser_get_module_definition(parser));
}

void ut_asn1_module_definition_set_exports(UtObject *object,
                                           UtObject *exports) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  self->export_all = false;
  ut_object_unref(self->exports);
  self->exports = ut_object_ref(exports);
}

void ut_asn1_module_definition_set_export_all(UtObject *object) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  self->export_all = true;
  ut_object_unref(self->exports);
  self->exports = ut_string_list_new();
}

void ut_asn1_module_definition_set_imports(UtObject *object,
                                           UtObject *imports) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  ut_object_unref(self->imports);
  self->imports = ut_object_ref(imports);
}

const char *ut_asn1_module_definition_get_identifier(UtObject *object) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  return self->identifier;
}

UtObject *ut_asn1_module_definition_get_object_identifier(UtObject *object) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  return self->object_identifier;
}

UtObject *ut_asn1_module_definition_get_exports(UtObject *object) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  return self->exports;
}

bool ut_asn1_module_definition_get_export_all(UtObject *object) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  return self->export_all;
}

UtObject *ut_asn1_module_definition_get_imports(UtObject *object) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  return self->imports;
}

UtObject *ut_asn1_module_definition_get_assignments(UtObject *object) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  return self->assignments;
}

UtObject *ut_asn1_module_definition_get_assignment(UtObject *object,
                                                   const char *reference) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  return ut_map_lookup_string(self->assignments, reference);
}

bool ut_object_is_asn1_module_definition(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
