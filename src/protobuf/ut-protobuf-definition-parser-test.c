#include "ut.h"

static void test_empty() {
  // Completely empty file.
  UtObjectRef parser1 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser1, "");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser1));
}

static void test_comments() {
  UtObjectRef parser1 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser1, "// Single line comment.\n"
                                               "/* Multi\n"
                                               "   Line\n"
                                               "   Comment */\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser1));
}

static void test_syntax() {
  UtObjectRef parser1 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser1, "syntax = \"proto2\";\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser1));
  UtObject *definition1 = ut_protobuf_definition_parser_get_definition(parser1);
  ut_assert_int_equal(ut_protobuf_definition_get_syntax(definition1), 2);

  UtObjectRef parser2 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser2, "syntax = \"proto3\";\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser2));
  UtObject *definition2 = ut_protobuf_definition_parser_get_definition(parser2);
  ut_assert_int_equal(ut_protobuf_definition_get_syntax(definition2), 3);

  // Defaults to proto2.
  UtObjectRef parser3 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser3, "\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser3));
  UtObject *definition3 = ut_protobuf_definition_parser_get_definition(parser3);
  ut_assert_int_equal(ut_protobuf_definition_get_syntax(definition3), 2);

  // Invalid value.
  UtObjectRef parser10 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser10, "syntax = \"invalid\";\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser10), "Unrecognised syntax");

  // Value not a string.
  UtObjectRef parser11 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser11, "syntax = 2;\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser11),
      "Expected string, got 2");

  // Missing value.
  UtObjectRef parser12 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser12, "syntax =;\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser12),
      "Expected string, got ;");

  // Missing assigment.
  UtObjectRef parser13 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser13, "syntax;\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser13), "Expected =, got ;");
}

static void test_message() {
  // Empty message.
  UtObjectRef parser1 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser1, "message Empty {}\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser1));
  UtObject *definition1 = ut_protobuf_definition_parser_get_definition(parser1);
  UtObject *message1 = ut_protobuf_definition_lookup(definition1, "Empty");
  ut_assert_non_null_object(message1);
  ut_assert_true(ut_object_is_protobuf_message_type(message1));
  ut_assert_int_equal(
      ut_map_get_length(ut_protobuf_message_type_get_fields(message1)), 0);

  // Simple message.
  UtObjectRef parser2 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser2, "syntax = \"proto3\";\n"
                                               "message Person {\n"
                                               "  string name = 1;\n"
                                               "  uint32 age = 2;\n"
                                               "}\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser2));
  UtObject *definition2 = ut_protobuf_definition_parser_get_definition(parser2);
  UtObject *message2 = ut_protobuf_definition_lookup(definition2, "Person");
  ut_assert_non_null_object(message2);
  ut_assert_true(ut_object_is_protobuf_message_type(message2));
  ut_assert_int_equal(
      ut_map_get_length(ut_protobuf_message_type_get_fields(message2)), 2);
  UtObject *field2a = ut_protobuf_message_type_get_field(message2, "name");
  ut_assert_non_null_object(field2a);
  ut_assert_int_equal(ut_protobuf_message_field_get_type(field2a),
                      UT_PROTOBUF_MESSAGE_FIELD_TYPE_IMPLICIT);
  ut_assert_int_equal(ut_protobuf_message_field_get_number(field2a), 1);
  UtObject *type2a = ut_protobuf_message_field_get_value_type(field2a);
  ut_assert_true(ut_object_is_protobuf_primitive_type(type2a));
  ut_assert_int_equal(ut_protobuf_primitive_type_get_type(type2a),
                      UT_PROTOBUF_PRIMITIVE_STRING);
  UtObject *field2b = ut_protobuf_message_type_get_field(message2, "age");
  ut_assert_non_null_object(field2b);
  ut_assert_int_equal(ut_protobuf_message_field_get_type(field2b),
                      UT_PROTOBUF_MESSAGE_FIELD_TYPE_IMPLICIT);
  ut_assert_int_equal(ut_protobuf_message_field_get_number(field2b), 2);
  UtObject *type2b = ut_protobuf_message_field_get_value_type(field2b);
  ut_assert_true(ut_object_is_protobuf_primitive_type(type2b));
  ut_assert_int_equal(ut_protobuf_primitive_type_get_type(type2b),
                      UT_PROTOBUF_PRIMITIVE_UINT32);

  // All field types (proto2).
  UtObjectRef parser3 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser3, "syntax = \"proto2\";\n"
                                               "message Message {\n"
                                               "  required string value1 = 1;\n"
                                               "  optional string value2 = 2;\n"
                                               "  repeated string value3 = 3;\n"
                                               "}\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser3));
  UtObject *definition3 = ut_protobuf_definition_parser_get_definition(parser3);
  UtObject *message3 = ut_protobuf_definition_lookup(definition3, "Message");
  ut_assert_non_null_object(message3);
  UtObject *field3a = ut_protobuf_message_type_get_field(message3, "value1");
  ut_assert_non_null_object(field3a);
  ut_assert_int_equal(ut_protobuf_message_field_get_type(field3a),
                      UT_PROTOBUF_MESSAGE_FIELD_TYPE_REQUIRED);
  UtObject *field3b = ut_protobuf_message_type_get_field(message3, "value2");
  ut_assert_non_null_object(field3b);
  ut_assert_int_equal(ut_protobuf_message_field_get_type(field3b),
                      UT_PROTOBUF_MESSAGE_FIELD_TYPE_OPTIONAL);
  UtObject *field3c = ut_protobuf_message_type_get_field(message3, "value3");
  ut_assert_non_null_object(field3c);
  ut_assert_int_equal(ut_protobuf_message_field_get_type(field3c),
                      UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED);

  // All field types (proto3).
  UtObjectRef parser4 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser4, "syntax = \"proto3\";\n"
                                               "message Message {\n"
                                               "  string value1 = 1;\n"
                                               "  optional string value2 = 2;\n"
                                               "  repeated string value3 = 3;\n"
                                               "}\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser4));
  UtObject *definition4 = ut_protobuf_definition_parser_get_definition(parser4);
  UtObject *message4 = ut_protobuf_definition_lookup(definition4, "Message");
  ut_assert_non_null_object(message4);
  UtObject *field4a = ut_protobuf_message_type_get_field(message4, "value1");
  ut_assert_non_null_object(field4a);
  ut_assert_int_equal(ut_protobuf_message_field_get_type(field4a),
                      UT_PROTOBUF_MESSAGE_FIELD_TYPE_IMPLICIT);
  UtObject *field4b = ut_protobuf_message_type_get_field(message4, "value2");
  ut_assert_non_null_object(field4b);
  ut_assert_int_equal(ut_protobuf_message_field_get_type(field4b),
                      UT_PROTOBUF_MESSAGE_FIELD_TYPE_OPTIONAL);
  UtObject *field4c = ut_protobuf_message_type_get_field(message4, "value3");
  ut_assert_non_null_object(field4c);
  ut_assert_int_equal(ut_protobuf_message_field_get_type(field4c),
                      UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED);

  // Using a message type in a field.
  UtObjectRef parser5 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser5,
                                      "syntax = \"proto3\";\n"
                                      "message Class {\n"
                                      "  Person teacher = 1;\n"
                                      "  repeated Person students = 2;\n"
                                      "}\n"
                                      "\n"
                                      "message Person {\n"
                                      "  string name = 1;\n"
                                      "  uint32 age = 2;\n"
                                      "}\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser5));
  UtObject *definition5 = ut_protobuf_definition_parser_get_definition(parser5);
  UtObject *message5 = ut_protobuf_definition_lookup(definition5, "Class");
  ut_assert_non_null_object(message5);
  UtObject *message5i = ut_protobuf_definition_lookup(definition5, "Person");
  ut_assert_non_null_object(message5i);
  UtObject *field5a = ut_protobuf_message_type_get_field(message5, "teacher");
  ut_assert_non_null_object(field5a);
  ut_assert_int_equal(ut_protobuf_message_field_get_type(field5a),
                      UT_PROTOBUF_MESSAGE_FIELD_TYPE_IMPLICIT);
  UtObject *type5a = ut_protobuf_message_field_get_value_type(field5a);
  ut_assert_true(ut_object_is_protobuf_referenced_type(type5a));
  UtObject *type5ai = ut_protobuf_referenced_type_get_type(type5a);
  ut_assert_true(type5ai == message5i);
  UtObject *field5b = ut_protobuf_message_type_get_field(message5, "students");
  ut_assert_non_null_object(field5b);
  ut_assert_int_equal(ut_protobuf_message_field_get_type(field5b),
                      UT_PROTOBUF_MESSAGE_FIELD_TYPE_REPEATED);
  UtObject *type5b = ut_protobuf_message_field_get_value_type(field5b);
  ut_assert_true(ut_object_is_protobuf_referenced_type(type5b));
  UtObject *type5bi = ut_protobuf_referenced_type_get_type(type5b);
  ut_assert_true(type5bi == message5i);

  // Nested message types.
  UtObjectRef parser6 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser6,
                                      "syntax = \"proto3\";\n"
                                      "message Class {\n"
                                      "  message Person {\n"
                                      "    string name = 1;\n"
                                      "    uint32 age = 2;\n"
                                      "  }\n"
                                      "\n"
                                      "  Person teacher = 1;\n"
                                      "  repeated Person students = 2;\n"
                                      "}\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser6));
  UtObject *definition6 = ut_protobuf_definition_parser_get_definition(parser6);
  UtObject *message6a =
      ut_protobuf_definition_lookup(definition6, "Class.Person");
  ut_assert_non_null_object(message6a);
  UtObject *message6b = ut_protobuf_definition_lookup(definition6, "Class");
  ut_assert_non_null_object(message6b);
  UtObject *field6b = ut_protobuf_message_type_get_field(message6b, "teacher");
  ut_assert_non_null_object(field6b);
  UtObject *type6b = ut_protobuf_message_field_get_value_type(field6b);
  ut_assert_true(ut_object_is_protobuf_referenced_type(type6b));
  UtObject *type6bi = ut_protobuf_referenced_type_get_type(type6b);
  ut_assert_true(type6bi == message6a);

  // Accessing nested types e.g. "Class.Person"
  UtObjectRef parser7 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser7,
                                      "syntax = \"proto3\";\n"
                                      "message Class {\n"
                                      "  message Person {\n"
                                      "    string name = 1;\n"
                                      "    uint32 age = 2;\n"
                                      "  }\n"
                                      "}\n"
                                      "\n"
                                      "message School {\n"
                                      "  repeated Class.Person students = 1;\n"
                                      "}\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser7));
  UtObject *definition7 = ut_protobuf_definition_parser_get_definition(parser7);
  UtObject *message7a =
      ut_protobuf_definition_lookup(definition7, "Class.Person");
  ut_assert_non_null_object(message7a);
  UtObject *message7b = ut_protobuf_definition_lookup(definition7, "School");
  ut_assert_non_null_object(message7b);
  UtObject *field7b = ut_protobuf_message_type_get_field(message7b, "students");
  ut_assert_non_null_object(field7b);
  UtObject *type7b = ut_protobuf_message_field_get_value_type(field7b);
  ut_assert_true(ut_object_is_protobuf_referenced_type(type7b));
  UtObject *type7bi = ut_protobuf_referenced_type_get_type(type7b);
  ut_assert_true(type7bi == message7a);

  // Dependency loop.
  // FIXME: Makes reference loop
  // https://github.com/robert-ancell/unit/issues/122
#if 0
  UtObjectRef parser8 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser8, "syntax = \"proto3\";\n"
                                               "message A {\n"
                                               "  B b = 1;\n"
                                               "}\n"
                                               "message B {\n"
                                               "  A a = 1;\n"
                                               "}\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser8));
  UtObject *definition8 = ut_protobuf_definition_parser_get_definition(parser8);
  UtObject *message8a = ut_protobuf_definition_lookup(definition8, "A");
  ut_assert_non_null_object(message8a);
  UtObject *message8b = ut_protobuf_definition_lookup(definition8, "B");
  ut_assert_non_null_object(message8b);
  UtObject *field8a = ut_protobuf_message_type_get_field(message8a, "b");
  ut_assert_non_null_object(field8a);
  UtObject *type8a = ut_protobuf_message_field_get_value_type(field8a);
  ut_assert_true(ut_object_is_protobuf_referenced_type(type8a));
  ut_assert_true(ut_protobuf_referenced_type_get_type(type8a) == message8b);
  UtObject *field8b = ut_protobuf_message_type_get_field(message8b, "a");
  ut_assert_non_null_object(field8b);
  UtObject *type8b = ut_protobuf_message_field_get_value_type(field8b);
  ut_assert_true(ut_object_is_protobuf_referenced_type(type8b));
  ut_assert_true(ut_protobuf_referenced_type_get_type(type8b) == message8a);
#endif

  // Invalid field number.
  UtObjectRef parser10 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser10, "syntax = \"proto3\";\n"
                                                "message Message {\n"
                                                "  string name = 0;\n"
                                                "}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser10),
      "Invalid field number 0");

  // Missing field number.
  UtObjectRef parser11 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser11, "syntax = \"proto3\";\n"
                                                "message Person {\n"
                                                "  string name;\n"
                                                "}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser11), "Expected =, got ;");

  // Duplicate message.
  UtObjectRef parser12 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser12, "syntax = \"proto3\";\n"
                                                "message Person {\n"
                                                "  string name = 1;\n"
                                                "}\n"
                                                "message Person {\n"
                                                "  uint32 age = 1;\n"
                                                "}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser12),
      "\"Person\" is already defined");

  // Need field type in proto2.
  UtObjectRef parser13 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser13, "syntax = \"proto2\";\n"
                                                "message Person {\n"
                                                "  string name;\n"
                                                "}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser13),
      "Expected required, optional or repeated, got string");

  // Required field type in proto3.
  UtObjectRef parser14 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser14, "syntax = \"proto3\";\n"
                                                "message Person {\n"
                                                "  required string name;\n"
                                                "}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser14),
      "Required fields are not allowed in proto3");

  // Missing message type.
  UtObjectRef parser15 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser15,
                                      "syntax = \"proto3\";\n"
                                      "message Class {\n"
                                      "  Person teacher = 1;\n"
                                      "  repeated Person students = 2;\n"
                                      "}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser15),
      "\"Person\" is not defined");
}

static void test_reserved() {
  // Reserved numbers.
  UtObjectRef parser1 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser1, "syntax = \"proto3\";\n"
                                               "message Person {\n"
                                               "  reserved 1, 2;\n"
                                               "  string name = 3;\n"
                                               "  uint32 age = 4;\n"
                                               "}\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser1));

  // Reserved names.
  UtObjectRef parser2 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser2,
                                      "syntax = \"proto3\";\n"
                                      "message Person {\n"
                                      "  reserved \"eye_color\", \"height\";\n"
                                      "  string name = 1;\n"
                                      "  uint32 age = 2;\n"
                                      "}\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser2));

  // Use of a reseved number.
  UtObjectRef parser10 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser10, "syntax = \"proto3\";\n"
                                                "message Person {\n"
                                                "  reserved 1, 2;\n"
                                                "  string name = 1;\n"
                                                "  uint32 age = 4;\n"
                                                "}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser10),
      "Field \"name\" uses reserved number 1");

  // Use of a reseved name.
  UtObjectRef parser11 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser11,
                                      "syntax = \"proto3\";\n"
                                      "message Person {\n"
                                      "  reserved \"eye_color\", \"height\";\n"
                                      "  string name = 1;\n"
                                      "  uint32 age = 2;\n"
                                      "  uint32 height = 3;\n"
                                      "}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser11),
      "Field name \"height\" is reserved");
}

static void test_enum() {
  // Simple enum.
  UtObjectRef parser1 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser1, "enum Fruit {\n"
                                               "  APPLE = 0;\n"
                                               "  ORANGE = 1;\n"
                                               "}\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser1));
  UtObject *definition1 = ut_protobuf_definition_parser_get_definition(parser1);
  UtObject *enum1 = ut_protobuf_definition_lookup(definition1, "Fruit");
  ut_assert_non_null_object(enum1);
  ut_assert_true(ut_object_is_protobuf_enum_type(enum1));
  ut_assert_int_equal(
      ut_map_get_length(ut_protobuf_enum_type_lookup_values_by_name(enum1)), 2);
  int value1a;
  ut_assert_true(ut_protobuf_enum_type_lookup_value(enum1, "APPLE", &value1a));
  ut_assert_int_equal(value1a, 0);
  int value1b;
  ut_assert_true(ut_protobuf_enum_type_lookup_value(enum1, "ORANGE", &value1b));
  ut_assert_int_equal(value1b, 1);

  // First element can be non zero in proto2.
  UtObjectRef parser2 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser2, "syntax = \"proto2\";\n"
                                               "enum Fruit {\n"
                                               "  APPLE = 1;\n"
                                               "  ORANGE = 2;\n"
                                               "}\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser2));

  // Empty enum.
  UtObjectRef parser10 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser10, "enum Fruit {}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser10),
      "Enums must contain at least one value");

  // Duplicate name.
  UtObjectRef parser11 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser11, "enum Fruit {\n"
                                                "  APPLE = 0;\n"
                                                "  APPLE = 1;\n"
                                                "}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser11),
      "\"APPLE\" is already defined");

  // First element must be zero in proto3.
  UtObjectRef parser12 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser12, "syntax = \"proto3\";\n"
                                                "enum Fruit {\n"
                                                "  APPLE = 1;\n"
                                                "  ORANGE = 2;\n"
                                                "}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser12),
      "The first enum value must be zero in proto3");

  // Duplicate enum.
  UtObjectRef parser13 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser13, "enum Fruit {\n"
                                                "  APPLE = 1;\n"
                                                "}\n"
                                                "enum Fruit {\n"
                                                "  ORANGE = 1;\n"
                                                "}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser13),
      "\"Fruit\" is already defined");
}

static void test_service() {
  UtObjectRef parser1 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(
      parser1, "syntax = \"proto3\";\n"
               "service Fruit {\n"
               "  rpc Query(Request) returns (Response);\n"
               "}\n"
               "message Request {\n"
               "  string key = 1;\n"
               "}\n"
               "\n"
               "message Response {\n"
               "  string value = 1;\n"
               "}\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser1));
  UtObject *definition1 = ut_protobuf_definition_parser_get_definition(parser1);
  UtObject *service1 = ut_protobuf_definition_lookup(definition1, "Fruit");
  ut_assert_non_null_object(service1);
  ut_assert_true(ut_object_is_protobuf_service(service1));
}

static void test_package() {
  UtObjectRef parser1 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser1, "syntax = \"proto3\";\n"
                                               "package foo.bar;\n"
                                               "message Person {\n"
                                               "  string name = 1;\n"
                                               "  uint32 age = 2;\n"
                                               "}\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser1));
  UtObject *definition1 = ut_protobuf_definition_parser_get_definition(parser1);
  UtObject *message1 =
      ut_protobuf_definition_lookup(definition1, "foo.bar.Person");
  ut_assert_non_null_object(message1);
  ut_assert_true(ut_object_is_protobuf_message_type(message1));

  // Package defined at end of file.
  UtObjectRef parser2 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser2, "syntax = \"proto3\";\n"
                                               "message Person {\n"
                                               "  string name = 1;\n"
                                               "  uint32 age = 2;\n"
                                               "}\n"
                                               "package foo.bar;\n");
  ut_assert_null_object(ut_protobuf_definition_parser_get_error(parser2));
  UtObject *definition2 = ut_protobuf_definition_parser_get_definition(parser2);
  UtObject *message2 =
      ut_protobuf_definition_lookup(definition2, "foo.bar.Person");
  ut_assert_non_null_object(message2);
  ut_assert_true(ut_object_is_protobuf_message_type(message2));

  // Invalid package name.
  UtObjectRef parser10 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser10, "syntax = \"proto3\";\n"
                                                "package .foo.bar;\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser10), "Invalid character");

  // Invalid package name.
  UtObjectRef parser11 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser11, "syntax = \"proto3\";\n"
                                                "package foo..bar;\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser11),
      "Invalid package name \"foo..bar\"");

  // Invalid package name.
  UtObjectRef parser12 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser12, "syntax = \"proto3\";\n"
                                                "package foo.bar.;\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser12),
      "Invalid package name \"foo.bar.\"");

  // Duplicate package definition.
  UtObjectRef parser13 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser13, "syntax = \"proto3\";\n"
                                                "package foo.bar;\n"
                                                "package foo.bar;\n"
                                                "message Person {}\n"
                                                "  string name = 1;\n"
                                                "  uint32 age = 2;\n"
                                                "}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser13),
      "Multiple package definitions");

  // Multiple package definitions.
  UtObjectRef parser14 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser14, "syntax = \"proto3\";\n"
                                                "package foo.bar;\n"
                                                "package foo.baz;\n"
                                                "message Person {}\n"
                                                "  string name = 1;\n"
                                                "  uint32 age = 2;\n"
                                                "}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser14),
      "Multiple package definitions");
}

static void test_import() {
  UtObjectRef parser1 = ut_protobuf_definition_parser_new();
  ut_protobuf_definition_parser_parse(parser1, "syntax = \"proto3\";\n"
                                               "import \"foo.proto\";\n"
                                               "message Person {\n"
                                               "  string name = 1;\n"
                                               "  uint32 age = 2;\n"
                                               "}\n");
  ut_assert_is_error_with_description(
      ut_protobuf_definition_parser_get_error(parser1),
      "Imports not implemented");
}

int main(int argc, char **argv) {
  test_empty();
  test_comments();
  test_syntax();
  test_message();
  test_reserved();
  test_enum();
  test_service();
  test_package();
  test_import();

  return 0;
}
