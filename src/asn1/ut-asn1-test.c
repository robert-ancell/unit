#include <assert.h>
#include <stdio.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef module_definition = ut_asn1_module_definition_new(
      "FooProtocol DEFINITIONS ::= BEGIN\n"
      "\n"
      "    FooQuestion ::= SEQUENCE {\n"
      "        trackingNumber INTEGER,\n"
      "        question       IA5String\n"
      "    }\n"
      "\n"
      "    FooAnswer ::= SEQUENCE {\n"
      "        questionNumber INTEGER,\n"
      "        answer         BOOLEAN\n"
      "    }\n"
      "\n"
      "    FooHistory ::= SEQUENCE {\n"
      "        questions SEQUENCE OF FooQuestion,\n"
      "        answers SEQUENCE OF FooAnswer,\n"
      "        anArray SEQUENCE OF INTEGER,\n"
      "        ...\n"
      "    }\n"
      "\n"
      "END\n");
  printf("%s\n", ut_asn1_module_definition_get_identifier(module_definition));
  UtObject *type_assignments =
      ut_asn1_module_definition_get_type_assignments(module_definition);
  size_t type_assignments_length = ut_list_get_length(type_assignments);
  for (size_t i = 0; i < type_assignments_length; i++) {
    UtObjectRef type_assignment = ut_list_get_element(type_assignments, i);
    ut_cstring_ref type_string =
        ut_object_to_string(ut_asn1_type_assignment_get_type(type_assignment));
    printf("%s ::= %s\n",
           ut_asn1_type_assignment_get_reference(type_assignment), type_string);
  }

  return 0;
}
