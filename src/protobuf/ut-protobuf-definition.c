#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;

  int syntax;
  UtObject *definitions;
} UtProtobufDefinition;

static void ut_protobuf_definition_cleanup(UtObject *object) {
  UtProtobufDefinition *self = (UtProtobufDefinition *)object;

  ut_object_unref(self->definitions);
}

static UtObjectInterface object_interface = {
    .type_name = "UtProtobufDefinition",
    .cleanup = ut_protobuf_definition_cleanup};

UtObject *ut_protobuf_definition_new(int syntax, UtObject *definitions) {
  UtObject *object =
      ut_object_new(sizeof(UtProtobufDefinition), &object_interface);
  UtProtobufDefinition *self = (UtProtobufDefinition *)object;

  self->syntax = syntax;
  self->definitions = ut_object_ref(definitions);

  return object;
}

UtObject *ut_protobuf_definition_new_from_text(const char *text) {
  UtObjectRef parser = ut_protobuf_definition_parser_new();
  if (!ut_protobuf_definition_parser_parse(parser, text)) {
    return ut_object_ref(ut_protobuf_definition_parser_get_error(parser));
  }

  return ut_object_ref(ut_protobuf_definition_parser_get_definition(parser));
}

int ut_protobuf_definition_get_syntax(UtObject *object) {
  assert(ut_object_is_protobuf_definition(object));
  UtProtobufDefinition *self = (UtProtobufDefinition *)object;
  return self->syntax;
}

UtObject *ut_protobuf_definition_get_definitions(UtObject *object) {
  assert(ut_object_is_protobuf_definition(object));
  UtProtobufDefinition *self = (UtProtobufDefinition *)object;
  return self->definitions;
}

UtObject *ut_protobuf_definition_lookup(UtObject *object, const char *name) {
  assert(ut_object_is_protobuf_definition(object));
  UtProtobufDefinition *self = (UtProtobufDefinition *)object;
  return ut_map_lookup_string(self->definitions, name);
}

bool ut_object_is_protobuf_definition(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
