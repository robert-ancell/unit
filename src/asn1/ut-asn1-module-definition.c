#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *identifier;
  UtObject *object_identifier;
  UtObject *assignments;
} UtAsn1ModuleDefinition;

static void ut_asn1_module_definition_cleanup(UtObject *object) {
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  free(self->identifier);
  ut_object_unref(self->object_identifier);
  ut_object_unref(self->assignments);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1ModuleDefinition",
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

UtObject *ut_asn1_module_definition_get_assignments(UtObject *object) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  return self->assignments;
}

UtObject *ut_asn1_module_definition_lookup_assignment(UtObject *object,
                                                      const char *reference) {
  assert(ut_object_is_asn1_module_definition(object));
  UtAsn1ModuleDefinition *self = (UtAsn1ModuleDefinition *)object;
  return ut_map_lookup_string(self->assignments, reference);
}

bool ut_object_is_asn1_module_definition(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
