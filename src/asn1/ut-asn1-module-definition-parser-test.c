#include <math.h>

#include "ut.h"

static void test_module_identifier() {
  // Simple module identifier.
  UtObjectRef module1 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN END");
  ut_assert_is_not_error(module1);
  ut_assert_cstring_equal(ut_asn1_module_definition_get_identifier(module1),
                          "Test");
  ut_assert_null_object(
      ut_asn1_module_definition_get_object_identifier(module1));
  ut_assert_int_equal(
      ut_map_get_length(ut_asn1_module_definition_get_assignments(module1)), 0);

  // Module identifier with object identifier (name and number form).
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test { iso(1) identified-organization(3) dod(6) internet(1) security(5) "
      "mechanisms(5) pkix(7) id-mod(0) id-pkix1-explicit (18) } DEFINITIONS "
      "::= BEGIN END");
  ut_assert_is_not_error(module2);
  ut_assert_cstring_equal(ut_asn1_module_definition_get_identifier(module2),
                          "Test");
  ut_assert_non_null_object(
      ut_asn1_module_definition_get_object_identifier(module2));
  ut_assert_int_equal(
      ut_map_get_length(ut_asn1_module_definition_get_assignments(module2)), 0);

  // Module identifier with object identifier (number form).
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test { 1 3 6 1 5 5 7 0 18 } DEFINITIONS ::= BEGIN END");
  ut_assert_is_not_error(module3);
  ut_assert_cstring_equal(ut_asn1_module_definition_get_identifier(module3),
                          "Test");
  ut_assert_non_null_object(
      ut_asn1_module_definition_get_object_identifier(module3));
  ut_assert_int_equal(
      ut_map_get_length(ut_asn1_module_definition_get_assignments(module3)), 0);

  // Invalid identifier starting character.
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "test1 DEFINITIONS ::= BEGIN END");
  ut_assert_is_error_with_description(module4,
                                      "Expected modulereference, got test1");

  // Invalid identifier character.
  UtObjectRef module5 = ut_asn1_module_definition_new_from_text(
      "Test_1 DEFINITIONS ::= BEGIN END");
  ut_assert_is_error_with_description(module5, "Invalid character");

  // Duplicate identifier.
  UtObjectRef module6 = ut_asn1_module_definition_new_from_text(
      "Test1 Test2 DEFINITIONS ::= BEGIN END");
  ut_assert_is_error_with_description(module6,
                                      "Expected DEFINITIONS, got Test2");

  // Invalid object identifier.
  UtObjectRef module7 = ut_asn1_module_definition_new_from_text(
      "Test1 {} DEFINITIONS ::= BEGIN END");
  ut_assert_is_error_with_description(
      module7, "Expected identifier, number or }, got }");
}

static void test_whitespace() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "\nTest\nDEFINITIONS\n::=\nBEGIN\nEND\n");
  ut_assert_is_not_error(module1);

  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "\tTest\tDEFINITIONS\t::=\tBEGIN\tEND\t");
  ut_assert_is_not_error(module2);

  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "   Test   DEFINITIONS   ::=   BEGIN   END   ");
  ut_assert_is_not_error(module3);
}

static void test_module_definitions() {
  // Test missing required parts of module definition.
  UtObjectRef truncated_module0 = ut_asn1_module_definition_new_from_text("");
  ut_assert_is_error_with_description(truncated_module0,
                                      "Expected modulereference, got ");
  UtObjectRef truncated_module1 =
      ut_asn1_module_definition_new_from_text("Test");
  ut_assert_is_error_with_description(truncated_module1,
                                      "Expected DEFINITIONS, got ");
  UtObjectRef truncated_module2 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS");
  ut_assert_is_error_with_description(truncated_module2, "Expected ::=, got ");
  UtObjectRef truncated_module3 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::=");
  ut_assert_is_error_with_description(truncated_module3,
                                      "Expected BEGIN, got ");
  UtObjectRef truncated_module4 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN");
  ut_assert_is_error_with_description(
      truncated_module4, "Expected valueassignment or typeassignment, got ");
}

static void test_comments() {
  UtObjectRef module1 =
      ut_asn1_module_definition_new_from_text("-- Leading comment\n"
                                              "Test DEFINITIONS ::= BEGIN END");
  ut_assert_is_not_error(module1);

  UtObjectRef module2 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN END\n"
                                              "-- Trailing comment\n");
  ut_assert_is_not_error(module2);

  UtObjectRef module3 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::=\n"
                                              "BEGIN -- The beginning\n"
                                              "END -- The end");
  ut_assert_is_not_error(module3);

  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test /* Comment */ DEFINITIONS /* Comment */ ::= /* Comment */ BEGIN /* "
      "Comment */ END");
  ut_assert_is_not_error(module4);
}

static void test_boolean_type_assignment() {
  UtObjectRef module1 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    BooleanType ::= BOOLEAN\n"
                                              "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "BooleanType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_boolean_type(type1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    BooleanType1 ::= BOOLEAN\n"
      "    BooleanType2 ::= BooleanType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "BooleanType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(
      ut_object_is_asn1_boolean_type(ut_asn1_referenced_type_get_type(type2)));

  // Constrained to single value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    BooleanType ::= BOOLEAN (TRUE)\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *type3 =
      ut_asn1_module_definition_get_assignment(module3, "BooleanType");
  ut_assert_non_null_object(type3);
  ut_assert_true(ut_object_is_asn1_constrained_type(type3));
  ut_assert_true(
      ut_object_is_asn1_boolean_type(ut_asn1_constrained_type_get_type(type3)));
  UtObject *constraint3 = ut_asn1_constrained_type_get_constraint(type3);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint3));
  UtObject *constraint_value3 = ut_asn1_value_constraint_get_value(constraint3);
  ut_assert_true(ut_object_is_boolean(constraint_value3));
  ut_assert_true(ut_boolean_get_value(constraint_value3));

  // Invalid type in constraint.
  UtObjectRef module10 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    BooleanType ::= BOOLEAN (NULL)\n"
      "END");
  ut_assert_is_error_with_description(module10,
                                      "Expected TRUE or FALSE, got NULL");
}

static void test_boolean_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    booleanValue BOOLEAN ::= TRUE\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "booleanValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(
      ut_object_is_asn1_boolean_type(ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_is_boolean(value1));
  ut_assert_true(ut_boolean_get_value(value1));

  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    booleanValue BOOLEAN ::= FALSE\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "booleanValue");
  ut_assert_non_null_object(assignment2);
  ut_assert_true(
      ut_object_is_asn1_boolean_type(ut_asn1_type_value_get_type(assignment2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_is_boolean(value2));
  ut_assert_false(ut_boolean_get_value(value2));

  // Referenced type.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    BooleanType ::= BOOLEAN\n"
      "    booleanValue BooleanType ::= TRUE\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "booleanValue");
  ut_assert_non_null_object(assignment3);
  UtObject *type3 = ut_asn1_type_value_get_type(assignment3);
  ut_assert_true(ut_object_is_asn1_referenced_type(type3));
  ut_assert_true(
      ut_object_is_asn1_boolean_type(ut_asn1_referenced_type_get_type(type3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_is_boolean(value3));
  ut_assert_true(ut_boolean_get_value(value3));

  // Referenced value.
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    booleanValue1 BOOLEAN ::= TRUE\n"
      "    booleanValue2 BOOLEAN ::= booleanValue1\n"
      "END");
  ut_assert_is_not_error(module4);
  UtObject *assignment4 =
      ut_asn1_module_definition_get_assignment(module4, "booleanValue2");
  ut_assert_non_null_object(assignment4);
  ut_assert_true(
      ut_object_is_asn1_boolean_type(ut_asn1_type_value_get_type(assignment4)));
  UtObject *value4 = ut_asn1_type_value_get_value(assignment4);
  ut_assert_true(ut_object_is_boolean(value4));
  ut_assert_true(ut_boolean_get_value(value4));

  // Invalid value type.
  UtObjectRef module5 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    booleanValue1 BOOLEAN ::= \"Hello World\"\n"
      "END");
  ut_assert_is_error_with_description(
      module5, "Expected TRUE or FALSE, got \"Hello World\"");

  UtObjectRef module6 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    booleanValue1 BOOLEAN ::= 0\n"
      "END");
  ut_assert_is_error_with_description(module6, "Expected TRUE or FALSE, got 0");

  UtObjectRef module7 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    booleanValue1 BOOLEAN ::= 'DEADBEEF'H\n"
      "END");
  ut_assert_is_error_with_description(
      module7, "Expected TRUE or FALSE, got 'DEADBEEF'H");
}

static void test_integer_type_assignment() {
  UtObjectRef module1 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    IntegerType ::= INTEGER\n"
                                              "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "IntegerType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_integer_type(type1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    IntegerType1 ::= INTEGER\n"
      "    IntegerType2 ::= IntegerType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "IntegerType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_referenced_type_get_type(type2)));

  // Constrained to single value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    IntegerType ::= INTEGER (42)\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *type3 =
      ut_asn1_module_definition_get_assignment(module3, "IntegerType");
  ut_assert_non_null_object(type3);
  ut_assert_true(ut_object_is_asn1_constrained_type(type3));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_constrained_type_get_type(type3)));
  UtObject *constraint3 = ut_asn1_constrained_type_get_constraint(type3);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint3));
  UtObject *constraint_value3 = ut_asn1_value_constraint_get_value(constraint3);
  ut_assert_true(ut_object_is_int64(constraint_value3));
  ut_assert_int_equal(ut_int64_get_value(constraint_value3), 42);

  // Constrained to value range.
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    IntegerType ::= INTEGER (0..255)\n"
      "END");
  ut_assert_is_not_error(module4);
  UtObject *type4 =
      ut_asn1_module_definition_get_assignment(module4, "IntegerType");
  ut_assert_non_null_object(type4);
  ut_assert_true(ut_object_is_asn1_constrained_type(type4));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_constrained_type_get_type(type4)));
  UtObject *constraint4 = ut_asn1_constrained_type_get_constraint(type4);
  ut_assert_true(ut_object_is_asn1_integer_range_constraint(constraint4));
  ut_assert_int_equal(ut_asn1_integer_range_constraint_get_lower(constraint4),
                      0);
  ut_assert_int_equal(ut_asn1_integer_range_constraint_get_upper(constraint4),
                      255);

  // Constrained to negative values.
  UtObjectRef module5 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    IntegerType ::= INTEGER (MIN..0)\n"
      "END");
  ut_assert_is_not_error(module5);
  UtObject *type5 =
      ut_asn1_module_definition_get_assignment(module5, "IntegerType");
  ut_assert_non_null_object(type5);
  ut_assert_true(ut_object_is_asn1_constrained_type(type5));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_constrained_type_get_type(type5)));
  UtObject *constraint5 = ut_asn1_constrained_type_get_constraint(type5);
  ut_assert_true(ut_object_is_asn1_integer_range_constraint(constraint5));
  ut_assert_int_equal(ut_asn1_integer_range_constraint_get_lower(constraint5),
                      INT64_MIN);
  ut_assert_int_equal(ut_asn1_integer_range_constraint_get_upper(constraint5),
                      0);

  // Constrained to positive values.
  UtObjectRef module6 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    IntegerType ::= INTEGER (0..MAX)\n"
      "END");
  ut_assert_is_not_error(module6);
  UtObject *type6 =
      ut_asn1_module_definition_get_assignment(module6, "IntegerType");
  ut_assert_non_null_object(type6);
  ut_assert_true(ut_object_is_asn1_constrained_type(type6));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_constrained_type_get_type(type6)));
  UtObject *constraint6 = ut_asn1_constrained_type_get_constraint(type6);
  ut_assert_true(ut_object_is_asn1_integer_range_constraint(constraint6));
  ut_assert_int_equal(ut_asn1_integer_range_constraint_get_lower(constraint6),
                      0);
  ut_assert_int_equal(ut_asn1_integer_range_constraint_get_upper(constraint6),
                      INT64_MAX);

  // Constraint made of value references.
  UtObjectRef module7 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    minValue INTEGER ::= 0\n"
      "    maxValue INTEGER ::= 255\n"
      "    IntegerType ::= INTEGER (minValue..maxValue)\n"
      "END");
  ut_assert_is_not_error(module7);
  UtObject *type7 =
      ut_asn1_module_definition_get_assignment(module7, "IntegerType");
  ut_assert_non_null_object(type7);
  ut_assert_true(ut_object_is_asn1_constrained_type(type7));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_constrained_type_get_type(type7)));
  UtObject *constraint7 = ut_asn1_constrained_type_get_constraint(type7);
  ut_assert_true(ut_object_is_asn1_integer_range_constraint(constraint7));
  ut_assert_int_equal(ut_asn1_integer_range_constraint_get_lower(constraint7),
                      0);
  ut_assert_int_equal(ut_asn1_integer_range_constraint_get_upper(constraint7),
                      255);

  // Union of constraints.
  UtObjectRef module8 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    IntegerType ::= INTEGER (42|99)\n"
      "END");
  ut_assert_is_not_error(module8);
  UtObject *type8 =
      ut_asn1_module_definition_get_assignment(module8, "IntegerType");
  ut_assert_non_null_object(type8);
  ut_assert_true(ut_object_is_asn1_constrained_type(type8));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_constrained_type_get_type(type8)));
  UtObject *constraint8 = ut_asn1_constrained_type_get_constraint(type8);
  ut_assert_true(ut_object_is_asn1_union_constraint(constraint8));
  UtObject *constraints8 =
      ut_asn1_union_constraint_get_constraints(constraint8);
  ut_assert_int_equal(ut_list_get_length(constraints8), 2);
  UtObject *constraint8a = ut_object_list_get_element(constraints8, 0);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint8a));
  UtObject *constraint_value8a =
      ut_asn1_value_constraint_get_value(constraint8a);
  ut_assert_true(ut_object_is_int64(constraint_value8a));
  ut_assert_int_equal(ut_int64_get_value(constraint_value8a), 42);
  UtObject *constraint8b = ut_object_list_get_element(constraints8, 1);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint8b));
  UtObject *constraint_value8b =
      ut_asn1_value_constraint_get_value(constraint8b);
  ut_assert_true(ut_object_is_int64(constraint_value8b));
  ut_assert_int_equal(ut_int64_get_value(constraint_value8b), 99);

  // Union of constraints (alternate form).
  UtObjectRef module9 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    IntegerType ::= INTEGER (42 UNION 99)\n"
      "END");
  ut_assert_is_not_error(module9);
  UtObject *type9 =
      ut_asn1_module_definition_get_assignment(module9, "IntegerType");
  ut_assert_non_null_object(type9);
  ut_assert_true(ut_object_is_asn1_constrained_type(type9));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_constrained_type_get_type(type9)));
  UtObject *constraint9 = ut_asn1_constrained_type_get_constraint(type9);
  ut_assert_true(ut_object_is_asn1_union_constraint(constraint9));
  UtObject *constraints9 =
      ut_asn1_union_constraint_get_constraints(constraint9);
  ut_assert_int_equal(ut_list_get_length(constraints9), 2);
  UtObject *constraint9a = ut_object_list_get_element(constraints9, 0);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint9a));
  UtObject *constraint_value9a =
      ut_asn1_value_constraint_get_value(constraint9a);
  ut_assert_true(ut_object_is_int64(constraint_value9a));
  ut_assert_int_equal(ut_int64_get_value(constraint_value9a), 42);
  UtObject *constraint9b = ut_object_list_get_element(constraints9, 1);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint9b));
  UtObject *constraint_value9b =
      ut_asn1_value_constraint_get_value(constraint9b);
  ut_assert_true(ut_object_is_int64(constraint_value9b));
  ut_assert_int_equal(ut_int64_get_value(constraint_value9b), 99);

  // Invalid type in constraint.
  UtObjectRef module20 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    IntegerType ::= INTEGER (FALSE)\n"
      "END");
  ut_assert_is_error_with_description(module20, "Expected integer, got FALSE");

  // Invalid type in range constraint.
  UtObjectRef module21 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    IntegerType ::= INTEGER (0..FALSE)\n"
      "END");
  ut_assert_is_error_with_description(module21, "Expected integer, got FALSE");
  UtObjectRef module22 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    IntegerType ::= INTEGER (FALSE..0)\n"
      "END");
  ut_assert_is_error_with_description(module22, "Expected integer, got FALSE");
}

static void test_integer_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    integerValue INTEGER ::= 42\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "integerValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_is_int64(value1));
  ut_assert_int_equal(ut_int64_get_value(value1), 42);

  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    integerValue INTEGER ::= -42\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "integerValue");
  ut_assert_non_null_object(assignment2);
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_type_value_get_type(assignment2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_is_int64(value2));
  ut_assert_int_equal(ut_int64_get_value(value2), -42);

  // Referenced type.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    IntegerType ::= INTEGER\n"
      "    integerValue IntegerType ::= -42\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "integerValue");
  ut_assert_non_null_object(assignment3);
  UtObject *type3 = ut_asn1_type_value_get_type(assignment3);
  ut_assert_true(ut_object_is_asn1_referenced_type(type3));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_referenced_type_get_type(type3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_is_int64(value3));
  ut_assert_int_equal(ut_int64_get_value(value3), -42);

  // Referenced value.
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    integerValue1 INTEGER ::= -42\n"
      "    integerValue2 INTEGER ::= integerValue1\n"
      "END");
  ut_assert_is_not_error(module4);
  UtObject *assignment4 =
      ut_asn1_module_definition_get_assignment(module4, "integerValue2");
  ut_assert_non_null_object(assignment4);
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_type_value_get_type(assignment4)));
  UtObject *value4 = ut_asn1_type_value_get_value(assignment4);
  ut_assert_true(ut_object_is_int64(value4));
  ut_assert_int_equal(ut_int64_get_value(value4), -42);
}

static void test_bit_string_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    BitStringType ::= BIT STRING\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "BitStringType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_bit_string_type(type1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    BitStringType1 ::= BIT STRING\n"
      "    BitStringType2 ::= BitStringType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "BitStringType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_bit_string_type(
      ut_asn1_referenced_type_get_type(type2)));

  // Constrained to single value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    BitStringType ::= BIT STRING ('10101100110001'B)\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *type3 =
      ut_asn1_module_definition_get_assignment(module3, "BitStringType");
  ut_assert_non_null_object(type3);
  ut_assert_true(ut_object_is_asn1_constrained_type(type3));
  ut_assert_true(ut_object_is_asn1_bit_string_type(
      ut_asn1_constrained_type_get_type(type3)));
  UtObject *constraint3 = ut_asn1_constrained_type_get_constraint(type3);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint3));
  UtObject *constraint_value3 = ut_asn1_value_constraint_get_value(constraint3);
  ut_assert_true(ut_object_is_bit_list(constraint_value3));
  ut_assert_bit_list_equal_bin(constraint_value3, "10101100110001");

  // Invalid type in constraint.
  UtObjectRef module10 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    BitStringType ::= BIT STRING (NULL)\n"
      "END");
  ut_assert_is_error_with_description(module10, "Expected bstring, got NULL");
}

static void test_bit_string_value_assignment() {
  // Bit string.
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    bitStringValue BIT STRING ::= '10101100110001'B\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "bitStringValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(ut_object_is_asn1_bit_string_type(
      ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_is_bit_list(value1));
  ut_assert_bit_list_equal_bin(value1, "10101100110001");

  // Empty string.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    bitStringValue BIT STRING ::= ''B\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "bitStringValue");
  ut_assert_non_null_object(assignment2);
  ut_assert_true(ut_object_is_asn1_bit_string_type(
      ut_asn1_type_value_get_type(assignment2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_is_bit_list(value2));
  ut_assert_bit_list_equal_bin(value2, "");

  // Whitespace allowed.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    bitStringValue BIT STRING ::= ' 1 0 1 0 1 1 0 0 1 1 0 0 0 1 "
      "'B\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "bitStringValue");
  ut_assert_non_null_object(assignment3);
  ut_assert_true(ut_object_is_asn1_bit_string_type(
      ut_asn1_type_value_get_type(assignment3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_is_bit_list(value3));
  ut_assert_bit_list_equal_bin(value3, "10101100110001");

  // Referenced type.
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    BitStringType ::= BIT STRING\n"
      "    bitStringValue BitStringType ::= '10101100110001'B\n"
      "END");
  ut_assert_is_not_error(module4);
  UtObject *assignment4 =
      ut_asn1_module_definition_get_assignment(module4, "bitStringValue");
  ut_assert_non_null_object(assignment4);
  UtObject *type4 = ut_asn1_type_value_get_type(assignment4);
  ut_assert_true(ut_object_is_asn1_referenced_type(type4));
  ut_assert_true(ut_object_is_asn1_bit_string_type(
      ut_asn1_referenced_type_get_type(type4)));
  UtObject *value4 = ut_asn1_type_value_get_value(assignment4);
  ut_assert_true(ut_object_is_bit_list(value4));
  ut_assert_bit_list_equal_bin(value4, "10101100110001");

  // Referenced value.
  UtObjectRef module5 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    bitStringValue1 BIT STRING ::= '10101100110001'B\n"
      "    bitStringValue2 BIT STRING ::= bitStringValue1\n"
      "END");
  ut_assert_is_not_error(module5);
  UtObject *assignment5 =
      ut_asn1_module_definition_get_assignment(module5, "bitStringValue2");
  ut_assert_non_null_object(assignment5);
  ut_assert_true(ut_object_is_asn1_bit_string_type(
      ut_asn1_type_value_get_type(assignment5)));
  UtObject *value5 = ut_asn1_type_value_get_value(assignment5);
  ut_assert_true(ut_object_is_bit_list(value5));
  ut_assert_bit_list_equal_bin(value5, "10101100110001");
}

static void test_octet_string_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    OctetStringType ::= OCTET STRING\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "OctetStringType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_octet_string_type(type1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    OctetStringType1 ::= OCTET STRING\n"
      "    OctetStringType2 ::= OctetStringType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "OctetStringType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_octet_string_type(
      ut_asn1_referenced_type_get_type(type2)));

  // Constrained to single value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    OctetStringType ::= OCTET STRING ('DEADBEEF'H)\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *type3 =
      ut_asn1_module_definition_get_assignment(module3, "OctetStringType");
  ut_assert_non_null_object(type3);
  ut_assert_true(ut_object_is_asn1_constrained_type(type3));
  ut_assert_true(ut_object_is_asn1_octet_string_type(
      ut_asn1_constrained_type_get_type(type3)));
  UtObject *constraint3 = ut_asn1_constrained_type_get_constraint(type3);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint3));
  UtObject *constraint_value3 = ut_asn1_value_constraint_get_value(constraint3);
  ut_assert_true(ut_object_implements_uint8_list(constraint_value3));
  ut_assert_uint8_list_equal_hex(constraint_value3, "DEADBEEF");

  // Invalid type in constraint.
  UtObjectRef module10 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    OctetStringType ::= OCTET STRING (NULL)\n"
      "END");
  ut_assert_is_error_with_description(module10,
                                      "Expected hstring or bstring, got NULL");
}

static void test_octet_string_value_assignment() {
  // Hex string.
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    octetStringValue OCTET STRING ::= 'DEADBEEF'H\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "octetStringValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(ut_object_is_asn1_octet_string_type(
      ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_uint8_list(value1));
  ut_assert_uint8_list_equal_hex(value1, "deadbeef");

  // Empty string.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    octetStringValue OCTET STRING ::= ''H\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "octetStringValue");
  ut_assert_non_null_object(assignment2);
  ut_assert_true(ut_object_is_asn1_octet_string_type(
      ut_asn1_type_value_get_type(assignment2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_implements_uint8_list(value2));
  ut_assert_uint8_list_equal_hex(value2, "");

  // Whitespace allowed.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    octetStringValue OCTET STRING ::= ' DE AD BE EF 'H\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "octetStringValue");
  ut_assert_non_null_object(assignment3);
  ut_assert_true(ut_object_is_asn1_octet_string_type(
      ut_asn1_type_value_get_type(assignment3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_implements_uint8_list(value3));
  ut_assert_uint8_list_equal_hex(value3, "deadbeef");
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    octetStringValue OCTET STRING ::= ' D E A D B E E F 'H\n"
      "END");
  ut_assert_is_not_error(module4);
  UtObject *assignment4 =
      ut_asn1_module_definition_get_assignment(module4, "octetStringValue");
  ut_assert_non_null_object(assignment4);
  ut_assert_true(ut_object_is_asn1_octet_string_type(
      ut_asn1_type_value_get_type(assignment4)));
  UtObject *value4 = ut_asn1_type_value_get_value(assignment4);
  ut_assert_true(ut_object_implements_uint8_list(value4));
  ut_assert_uint8_list_equal_hex(value4, "deadbeef");

  // Half byte allowed.
  UtObjectRef module5 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    octetStringValue OCTET STRING ::= 'DEADBEE'H\n"
      "END");
  ut_assert_is_not_error(module5);
  UtObject *assignment5 =
      ut_asn1_module_definition_get_assignment(module5, "octetStringValue");
  ut_assert_non_null_object(assignment5);
  ut_assert_true(ut_object_is_asn1_octet_string_type(
      ut_asn1_type_value_get_type(assignment5)));
  UtObject *value5 = ut_asn1_type_value_get_value(assignment5);
  ut_assert_true(ut_object_implements_uint8_list(value5));
  ut_assert_uint8_list_equal_hex(value5, "deadbee0");

  // Bit string.
  UtObjectRef module6 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    octetStringValue OCTET STRING ::= '1010110011110001'B\n"
      "END");
  ut_assert_is_not_error(module6);
  UtObject *assignment6 =
      ut_asn1_module_definition_get_assignment(module6, "octetStringValue");
  ut_assert_non_null_object(assignment6);
  ut_assert_true(ut_object_is_asn1_octet_string_type(
      ut_asn1_type_value_get_type(assignment6)));
  UtObject *value6 = ut_asn1_type_value_get_value(assignment6);
  ut_assert_true(ut_object_implements_uint8_list(value6));
  ut_assert_uint8_list_equal_hex(value6, "acf1");

  // Whitespace allowed.
  UtObjectRef module7 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    octetStringValue OCTET STRING ::= ' 1 0 1 0 1 1 0 0 1 1 1 1 0 0 0 1 "
      "'B\n"
      "END");
  ut_assert_is_not_error(module7);
  UtObject *assignment7 =
      ut_asn1_module_definition_get_assignment(module7, "octetStringValue");
  ut_assert_non_null_object(assignment7);
  ut_assert_true(ut_object_is_asn1_octet_string_type(
      ut_asn1_type_value_get_type(assignment7)));
  UtObject *value7 = ut_asn1_type_value_get_value(assignment7);
  ut_assert_true(ut_object_implements_uint8_list(value7));
  ut_assert_uint8_list_equal_hex(value7, "acf1");

  // Partial byte allowed.
  UtObjectRef module8 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    octetStringValue OCTET STRING ::= '11111111111111'B\n"
      "END");
  ut_assert_is_not_error(module8);
  UtObject *assignment8 =
      ut_asn1_module_definition_get_assignment(module8, "octetStringValue");
  ut_assert_non_null_object(assignment8);
  ut_assert_true(ut_object_is_asn1_octet_string_type(
      ut_asn1_type_value_get_type(assignment8)));
  UtObject *value8 = ut_asn1_type_value_get_value(assignment8);
  ut_assert_true(ut_object_implements_uint8_list(value8));
  ut_assert_uint8_list_equal_hex(value8, "fffc");

  // Referenced type.
  UtObjectRef module9 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    OctetStringType ::= OCTET STRING\n"
      "    octetStringValue OctetStringType ::= 'DEADBEEF'H\n"
      "END");
  ut_assert_is_not_error(module9);
  UtObject *assignment9 =
      ut_asn1_module_definition_get_assignment(module9, "octetStringValue");
  ut_assert_non_null_object(assignment9);
  UtObject *type9 = ut_asn1_type_value_get_type(assignment9);
  ut_assert_true(ut_object_is_asn1_referenced_type(type9));
  ut_assert_true(ut_object_is_asn1_octet_string_type(
      ut_asn1_referenced_type_get_type(type9)));
  UtObject *value9 = ut_asn1_type_value_get_value(assignment9);
  ut_assert_true(ut_object_implements_uint8_list(value9));
  ut_assert_uint8_list_equal_hex(value9, "deadbeef");

  // Referenced value.
  UtObjectRef module10 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    octetStringValue1 OCTET STRING ::= 'DEADBEEF'H\n"
      "    octetStringValue2 OCTET STRING ::= octetStringValue1\n"
      "END");
  ut_assert_is_not_error(module10);
  UtObject *assignment10 =
      ut_asn1_module_definition_get_assignment(module10, "octetStringValue2");
  ut_assert_non_null_object(assignment10);
  ut_assert_true(ut_object_is_asn1_octet_string_type(
      ut_asn1_type_value_get_type(assignment10)));
  UtObject *value10 = ut_asn1_type_value_get_value(assignment10);
  ut_assert_true(ut_object_implements_uint8_list(value10));
  ut_assert_uint8_list_equal_hex(value10, "deadbeef");
}

static void test_null_type_assignment() {
  UtObjectRef module1 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    NullType ::= NULL\n"
                                              "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "NullType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_null_type(type1));

  // Referenced type.
  UtObjectRef module2 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    NullType1 ::= NULL\n"
                                              "    NullType2 ::= NullType1\n"
                                              "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "NullType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(
      ut_object_is_asn1_null_type(ut_asn1_referenced_type_get_type(type2)));

  // Constrained to single value.
  UtObjectRef module3 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    NullType ::= NULL (NULL)\n"
                                              "END");
  ut_assert_is_not_error(module3);
  UtObject *type3 =
      ut_asn1_module_definition_get_assignment(module3, "NullType");
  ut_assert_non_null_object(type3);
  ut_assert_true(ut_object_is_asn1_constrained_type(type3));
  ut_assert_true(
      ut_object_is_asn1_null_type(ut_asn1_constrained_type_get_type(type3)));
  UtObject *constraint3 = ut_asn1_constrained_type_get_constraint(type3);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint3));
  UtObject *constraint_value3 = ut_asn1_value_constraint_get_value(constraint3);
  ut_assert_true(ut_object_is_null(constraint_value3));

  // Invalid type in constraint.
  UtObjectRef module10 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    NullType ::= NULL (FALSE)\n"
                                              "END");
  ut_assert_is_error_with_description(module10, "Expected NULL, got FALSE");
}

static void test_null_value_assignment() {
  UtObjectRef module1 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    nullValue NULL ::= NULL\n"
                                              "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "nullValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(
      ut_object_is_asn1_null_type(ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_is_null(value1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    NullType ::= NULL\n"
      "    nullValue NullType ::= NULL\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "nullValue");
  ut_assert_non_null_object(assignment2);
  UtObject *type2 = ut_asn1_type_value_get_type(assignment2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(
      ut_object_is_asn1_null_type(ut_asn1_referenced_type_get_type(type2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_is_null(value2));

  // Referenced value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    nullValue1 NULL ::= NULL\n"
      "    nullValue2 NULL ::= nullValue1\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "nullValue2");
  ut_assert_non_null_object(assignment3);
  ut_assert_true(
      ut_object_is_asn1_null_type(ut_asn1_type_value_get_type(assignment3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_is_null(value3));
}

static void test_object_identifier_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ObjectIdentifierType ::= OBJECT IDENTIFIER\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "ObjectIdentifierType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_object_identifier_type(type1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ObjectIdentifierType1 ::= OBJECT IDENTIFIER\n"
      "    ObjectIdentifierType2 ::= ObjectIdentifierType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 = ut_asn1_module_definition_get_assignment(
      module2, "ObjectIdentifierType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_object_identifier_type(
      ut_asn1_referenced_type_get_type(type2)));

  // Constrained to single value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ObjectIdentifierType ::= OBJECT IDENTIFIER ({ 1 0 8571 1})\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *type3 =
      ut_asn1_module_definition_get_assignment(module3, "ObjectIdentifierType");
  ut_assert_non_null_object(type3);
  ut_assert_true(ut_object_is_asn1_constrained_type(type3));
  ut_assert_true(ut_object_is_asn1_object_identifier_type(
      ut_asn1_constrained_type_get_type(type3)));
  UtObject *constraint3 = ut_asn1_constrained_type_get_constraint(type3);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint3));
  UtObject *constraint_value3 = ut_asn1_value_constraint_get_value(constraint3);
  ut_assert_true(ut_object_implements_uint32_list(constraint_value3));
  uint32_t expected_values3[4] = {1, 0, 8571, 1};
  ut_assert_uint32_list_equal(constraint_value3, expected_values3, 4);

  // Invalid type in constraint.
  UtObjectRef module10 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ObjectIdentifierType ::= OBJECT IDENTIFIER (NULL)\n"
      "END");
  ut_assert_is_error_with_description(module10, "Expected {, got NULL");
}

static void test_object_identifier_value_assignment() {
  // Names.
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    objectIdentifierValue OBJECT IDENTIFIER ::= { iso standard 8571 "
      "application-context (1) } \n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 = ut_asn1_module_definition_get_assignment(
      module1, "objectIdentifierValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(ut_object_is_asn1_object_identifier_type(
      ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_uint32_list(value1));
  uint32_t expected_values1[4] = {1, 0, 8571, 1};
  ut_assert_uint32_list_equal(value1, expected_values1, 4);

  // Numbers.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    objectIdentifierValue OBJECT IDENTIFIER ::= { 1 0 8571 1 } \n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 = ut_asn1_module_definition_get_assignment(
      module2, "objectIdentifierValue");
  ut_assert_non_null_object(assignment2);
  ut_assert_true(ut_object_is_asn1_object_identifier_type(
      ut_asn1_type_value_get_type(assignment2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_implements_uint32_list(value2));
  uint32_t expected_values2[4] = {1, 0, 8571, 1};
  ut_assert_uint32_list_equal(value2, expected_values2, 4);

  // Names and numbers.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    objectIdentifierValue OBJECT IDENTIFIER ::= { iso (1) standard (0) "
      "8571 application-context (1) } \n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 = ut_asn1_module_definition_get_assignment(
      module3, "objectIdentifierValue");
  ut_assert_non_null_object(assignment3);
  ut_assert_true(ut_object_is_asn1_object_identifier_type(
      ut_asn1_type_value_get_type(assignment3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_implements_uint32_list(value3));
  uint32_t expected_values3[4] = {1, 0, 8571, 1};
  ut_assert_uint32_list_equal(value3, expected_values3, 4);

  // Extend existing identifier.
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    baseValue OBJECT IDENTIFIER ::= { iso standard }\n"
      "    objectIdentifierValue OBJECT IDENTIFIER ::= { baseValue 8571 1 }\n"
      "END");
  ut_assert_is_not_error(module4);
  UtObject *assignment4 = ut_asn1_module_definition_get_assignment(
      module4, "objectIdentifierValue");
  ut_assert_non_null_object(assignment4);
  ut_assert_true(ut_object_is_asn1_object_identifier_type(
      ut_asn1_type_value_get_type(assignment4)));
  UtObject *value4 = ut_asn1_type_value_get_value(assignment4);
  ut_assert_true(ut_object_implements_uint32_list(value4));
  uint32_t expected_values4[4] = {1, 0, 8571, 1};
  ut_assert_uint32_list_equal(value4, expected_values4, 4);

  // Append relative OID.
  UtObjectRef module5 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    relativeOIDValue RELATIVE-OID ::= { 8571 1 } \n"
      "    objectIdentifierValue OBJECT IDENTIFIER ::= { iso standard "
      "relativeOIDValue }\n"
      "END");
  ut_assert_is_not_error(module5);
  UtObject *assignment5 = ut_asn1_module_definition_get_assignment(
      module5, "objectIdentifierValue");
  ut_assert_non_null_object(assignment5);
  ut_assert_true(ut_object_is_asn1_object_identifier_type(
      ut_asn1_type_value_get_type(assignment5)));
  UtObject *value5 = ut_asn1_type_value_get_value(assignment5);
  ut_assert_true(ut_object_implements_uint32_list(value5));
  uint32_t expected_values5[4] = {1, 0, 8571, 1};
  ut_assert_uint32_list_equal(value5, expected_values5, 4);

  // Invalid root name.
  UtObjectRef module6 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    objectIdentifierValue OBJECT IDENTIFIER ::= { invalid standard "
      "8571 application-context (1) } \n"
      "END");
  ut_assert_is_error_with_description(module6,
                                      "Unknown object identifier name");

  // Invalid ITU-T name.
  UtObjectRef module7 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    objectIdentifierValue OBJECT IDENTIFIER ::= { itu-t invalid "
      "8571 application-context (1) } \n"
      "END");
  ut_assert_is_error_with_description(module7,
                                      "Unknown object identifier name");

  // Invalid ISO name.
  UtObjectRef module8 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    objectIdentifierValue OBJECT IDENTIFIER ::= { iso invalid "
      "8571 application-context (1) } \n"
      "END");
  ut_assert_is_error_with_description(module8,
                                      "Unknown object identifier name");

  // Invalid joint ISO / ITU-T name.
  UtObjectRef module9 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    objectIdentifierValue OBJECT IDENTIFIER ::= { joint-iso-itu-t "
      "invalid "
      "8571 application-context (1) } \n"
      "END");
  ut_assert_is_error_with_description(module9,
                                      "Unknown object identifier name");
}

static void test_object_descriptor_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ObjectDescriptorType ::= ObjectDescriptor\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "ObjectDescriptorType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_object_descriptor_type(type1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ObjectDescriptorType1 ::= ObjectDescriptor\n"
      "    ObjectDescriptorType2 ::= ObjectDescriptorType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 = ut_asn1_module_definition_get_assignment(
      module2, "ObjectDescriptorType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_object_descriptor_type(
      ut_asn1_referenced_type_get_type(type2)));

  // Constrained to single value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ObjectDescriptorType ::= ObjectDescriptor (\"Foo\")\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *type3 =
      ut_asn1_module_definition_get_assignment(module3, "ObjectDescriptorType");
  ut_assert_non_null_object(type3);
  ut_assert_true(ut_object_is_asn1_constrained_type(type3));
  ut_assert_true(ut_object_is_asn1_object_descriptor_type(
      ut_asn1_constrained_type_get_type(type3)));
  UtObject *constraint3 = ut_asn1_constrained_type_get_constraint(type3);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint3));
  UtObject *constraint_value3 = ut_asn1_value_constraint_get_value(constraint3);
  ut_assert_true(ut_object_implements_string(constraint_value3));
  ut_assert_cstring_equal(ut_string_get_text(constraint_value3), "Foo");

  // Invalid type in constraint.
  UtObjectRef module10 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ObjectIdentifierType ::= ObjectDescriptor (NULL)\n"
      "END");
  ut_assert_is_error_with_description(module10, "Expected cstring, got NULL");
}

static void test_object_descriptor_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    objectDescriptorValue ObjectDescriptor ::= \"Foo\"\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 = ut_asn1_module_definition_get_assignment(
      module1, "objectDescriptorValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(ut_object_is_asn1_object_descriptor_type(
      ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_string(value1));
  ut_assert_cstring_equal(ut_string_get_text(value1), "Foo");

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ObjectDescriptorType ::= ObjectDescriptor\n"
      "    objectDescriptorValue ObjectDescriptorType ::= \"Foo\"\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 = ut_asn1_module_definition_get_assignment(
      module2, "objectDescriptorValue");
  ut_assert_non_null_object(assignment2);
  UtObject *type2 = ut_asn1_type_value_get_type(assignment2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_object_descriptor_type(
      ut_asn1_referenced_type_get_type(type2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_implements_string(value2));
  ut_assert_cstring_equal(ut_string_get_text(value2), "Foo");

  // Referenced value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    objectDescriptorValue1 ObjectDescriptor ::= \"Foo\"\n"
      "    objectDescriptorValue2 ObjectDescriptor ::= objectDescriptorValue1\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 = ut_asn1_module_definition_get_assignment(
      module3, "objectDescriptorValue2");
  ut_assert_non_null_object(assignment3);
  ut_assert_true(ut_object_is_asn1_object_descriptor_type(
      ut_asn1_type_value_get_type(assignment3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_implements_string(value3));
  ut_assert_cstring_equal(ut_string_get_text(value3), "Foo");
}

static void test_real_type_assignment() {
  UtObjectRef module1 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    RealType ::= REAL\n"
                                              "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "RealType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_real_type(type1));

  // Referenced type.
  UtObjectRef module2 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    RealType1 ::= REAL\n"
                                              "    RealType2 ::= RealType1\n"
                                              "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "RealType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_referenced_type_get_type(type2)));

  // Constrained to single value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    RealType ::= REAL (3.14159)\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *type3 =
      ut_asn1_module_definition_get_assignment(module3, "RealType");
  ut_assert_non_null_object(type3);
  ut_assert_true(ut_object_is_asn1_constrained_type(type3));
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_constrained_type_get_type(type3)));
  UtObject *constraint3 = ut_asn1_constrained_type_get_constraint(type3);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint3));
  UtObject *constraint_value3 = ut_asn1_value_constraint_get_value(constraint3);
  ut_assert_true(ut_object_is_float64(constraint_value3));
  ut_assert_float_equal(ut_float64_get_value(constraint_value3), 3.14159);

  // Constrained to value range.
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    RealType ::= REAL (-3.14159..3.14159)\n"
      "END");
  ut_assert_is_not_error(module4);
  UtObject *type4 =
      ut_asn1_module_definition_get_assignment(module4, "RealType");
  ut_assert_non_null_object(type4);
  ut_assert_true(ut_object_is_asn1_constrained_type(type4));
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_constrained_type_get_type(type4)));
  UtObject *constraint4 = ut_asn1_constrained_type_get_constraint(type4);
  ut_assert_true(ut_object_is_asn1_real_range_constraint(constraint4));
  ut_assert_int_equal(ut_asn1_real_range_constraint_get_lower(constraint4),
                      -3.14159);
  ut_assert_int_equal(ut_asn1_real_range_constraint_get_upper(constraint4),
                      3.14159);

  // Invalid type in constraint.
  UtObjectRef module10 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    RealType ::= REAL (NULL)\n"
                                              "END");
  ut_assert_is_error_with_description(
      module10, "Expected real value, PLUS-INFINITY, MINUS-INFINITY or "
                "NOT-A-NUMBER, got NULL");
}

static void test_real_value_assignment() {
  // Integer number
  UtObjectRef module1 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    realValue REAL ::= 42\n"
                                              "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "realValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_is_float64(value1));
  ut_assert_int_equal(ut_float64_get_value(value1), 42.0);

  // Fractional number.
  UtObjectRef module2 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    realValue REAL ::= 3.14159\n"
                                              "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "realValue");
  ut_assert_non_null_object(assignment2);
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_type_value_get_type(assignment2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_is_float64(value2));
  ut_assert_float_equal(ut_float64_get_value(value2), 3.14159);

  // Exponents
  UtObjectRef module3 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    realValue REAL ::= 1e3\n"
                                              "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "realValue");
  ut_assert_non_null_object(assignment3);
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_type_value_get_type(assignment3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_is_float64(value3));
  ut_assert_float_equal(ut_float64_get_value(value3), 1000);

  UtObjectRef module4 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    realValue REAL ::= 1E3\n"
                                              "END");
  ut_assert_is_not_error(module4);
  UtObject *assignment4 =
      ut_asn1_module_definition_get_assignment(module4, "realValue");
  ut_assert_non_null_object(assignment4);
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_type_value_get_type(assignment4)));
  UtObject *value4 = ut_asn1_type_value_get_value(assignment4);
  ut_assert_true(ut_object_is_float64(value4));
  ut_assert_float_equal(ut_float64_get_value(value4), 1000);

  UtObjectRef module5 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    realValue REAL ::= 1e-3\n"
                                              "END");
  ut_assert_is_not_error(module5);
  UtObject *assignment5 =
      ut_asn1_module_definition_get_assignment(module5, "realValue");
  ut_assert_non_null_object(assignment5);
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_type_value_get_type(assignment5)));
  UtObject *value5 = ut_asn1_type_value_get_value(assignment5);
  ut_assert_true(ut_object_is_float64(value5));
  ut_assert_float_equal(ut_float64_get_value(value5), 0.001);

  UtObjectRef module6 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    realValue REAL ::= -12345.6e-2\n"
      "END");
  ut_assert_is_not_error(module6);
  UtObject *assignment6 =
      ut_asn1_module_definition_get_assignment(module6, "realValue");
  ut_assert_non_null_object(assignment6);
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_type_value_get_type(assignment6)));
  UtObject *value6 = ut_asn1_type_value_get_value(assignment6);
  ut_assert_true(ut_object_is_float64(value6));
  ut_assert_float_equal(ut_float64_get_value(value6), -123.456);

  // Special values.
  UtObjectRef module7 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    realValue REAL ::= PLUS-INFINITY\n"
      "END");
  ut_assert_is_not_error(module7);
  UtObject *assignment7 =
      ut_asn1_module_definition_get_assignment(module7, "realValue");
  ut_assert_non_null_object(assignment7);
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_type_value_get_type(assignment7)));
  UtObject *value7 = ut_asn1_type_value_get_value(assignment7);
  ut_assert_true(ut_object_is_float64(value7));
  ut_assert_float_equal(ut_float64_get_value(value7), INFINITY);

  UtObjectRef module8 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    realValue REAL ::= MINUS-INFINITY\n"
      "END");
  ut_assert_is_not_error(module8);
  UtObject *assignment8 =
      ut_asn1_module_definition_get_assignment(module8, "realValue");
  ut_assert_non_null_object(assignment8);
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_type_value_get_type(assignment8)));
  UtObject *value8 = ut_asn1_type_value_get_value(assignment8);
  ut_assert_true(ut_object_is_float64(value8));
  ut_assert_float_equal(ut_float64_get_value(value8), -INFINITY);

  UtObjectRef module9 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    realValue REAL ::= NOT-A-NUMBER\n"
      "END");
  ut_assert_is_not_error(module9);
  UtObject *assignment9 =
      ut_asn1_module_definition_get_assignment(module9, "realValue");
  ut_assert_non_null_object(assignment9);
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_type_value_get_type(assignment9)));
  UtObject *value9 = ut_asn1_type_value_get_value(assignment9);
  ut_assert_true(ut_object_is_float64(value9));
  ut_assert_true(isnan(ut_float64_get_value(value9)));

  // Referenced type.
  UtObjectRef module10 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    RealType ::= REAL\n"
      "    realValue RealType ::= 3.14159\n"
      "END");
  ut_assert_is_not_error(module10);
  UtObject *assignment10 =
      ut_asn1_module_definition_get_assignment(module10, "realValue");
  ut_assert_non_null_object(assignment10);
  UtObject *type10 = ut_asn1_type_value_get_type(assignment10);
  ut_assert_true(ut_object_is_asn1_referenced_type(type10));
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_referenced_type_get_type(type10)));
  UtObject *value10 = ut_asn1_type_value_get_value(assignment10);
  ut_assert_true(ut_object_is_float64(value10));
  ut_assert_float_equal(ut_float64_get_value(value10), 3.14159);

  // Referenced value.
  UtObjectRef module11 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    realValue1 REAL ::= 3.14159\n"
      "    realValue2 REAL ::= realValue1\n"
      "END");
  ut_assert_is_not_error(module11);
  UtObject *assignment11 =
      ut_asn1_module_definition_get_assignment(module11, "realValue2");
  ut_assert_non_null_object(assignment11);
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_type_value_get_type(assignment11)));
  UtObject *value11 = ut_asn1_type_value_get_value(assignment11);
  ut_assert_true(ut_object_is_float64(value11));
  ut_assert_float_equal(ut_float64_get_value(value11), 3.14159);
}

static void test_enumerated_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    EnumeratedType ::= ENUMERATED { red, green, blue }\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "EnumeratedType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_enumerated_type(type1));
  ut_assert_int_equal(ut_asn1_enumerated_type_get_value(type1, "red"), 0);
  ut_assert_int_equal(ut_asn1_enumerated_type_get_value(type1, "green"), 1);
  ut_assert_int_equal(ut_asn1_enumerated_type_get_value(type1, "blue"), 2);

  // Numbers explicitly set.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    EnumeratedType ::= ENUMERATED { red (1), green (2), blue (3) }\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "EnumeratedType");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_enumerated_type(type2));
  ut_assert_int_equal(ut_asn1_enumerated_type_get_value(type1, "red"), 0);
  ut_assert_int_equal(ut_asn1_enumerated_type_get_value(type1, "green"), 1);
  ut_assert_int_equal(ut_asn1_enumerated_type_get_value(type1, "blue"), 2);

  // Referenced type.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    EnumeratedType1 ::= ENUMERATED { red, green, blue }\n"
      "    EnumeratedType2 ::= EnumeratedType1\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *type3 =
      ut_asn1_module_definition_get_assignment(module3, "EnumeratedType2");
  ut_assert_non_null_object(type3);
  ut_assert_true(ut_object_is_asn1_referenced_type(type3));
  UtObject *type3i = ut_asn1_referenced_type_get_type(type3);
  ut_assert_true(ut_object_is_asn1_enumerated_type(type3i));
  ut_assert_int_equal(ut_asn1_enumerated_type_get_value(type1, "red"), 0);
  ut_assert_int_equal(ut_asn1_enumerated_type_get_value(type1, "green"), 1);
  ut_assert_int_equal(ut_asn1_enumerated_type_get_value(type1, "blue"), 2);

  // Constrained to single value.
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    EnumeratedType ::= ENUMERATED { red, green, blue } (green)\n"
      "END");
  ut_assert_is_not_error(module4);
  UtObject *type4 =
      ut_asn1_module_definition_get_assignment(module4, "EnumeratedType");
  ut_assert_non_null_object(type4);
  ut_assert_true(ut_object_is_asn1_constrained_type(type4));
  ut_assert_true(ut_object_is_asn1_enumerated_type(
      ut_asn1_constrained_type_get_type(type4)));
  UtObject *constraint4 = ut_asn1_constrained_type_get_constraint(type4);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint4));
  UtObject *constraint_value4 = ut_asn1_value_constraint_get_value(constraint4);
  ut_assert_true(ut_object_implements_string(constraint_value4));
  ut_assert_cstring_equal(ut_string_get_text(constraint_value4), "green");

  // Empty enumeration.
  UtObjectRef module10 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    EnumeratedType ::= ENUMERATED { }\n"
      "END");
  ut_assert_is_error_with_description(module10, "Expected identifier, got }");

  // Invalid type in constraint.
  UtObjectRef module11 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    EnumeratedType ::= ENUMERATED { red, green, blue } (NULL)\n"
      "END");
  ut_assert_is_error_with_description(module11,
                                      "Expected enumerated value, got NULL");
}

static void test_enumerated_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    enumeratedValue ENUMERATED { red, green, blue } ::= green\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "enumeratedValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(ut_object_is_asn1_enumerated_type(
      ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_string(value1));
  ut_assert_cstring_equal(ut_string_get_text(value1), "green");

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    EnumeratedType ::= ENUMERATED { red, green, blue }\n"
      "    enumeratedValue EnumeratedType ::= green\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "enumeratedValue");
  ut_assert_non_null_object(assignment2);
  UtObject *type2 = ut_asn1_type_value_get_type(assignment2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_enumerated_type(
      ut_asn1_referenced_type_get_type(type2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_implements_string(value2));
  ut_assert_cstring_equal(ut_string_get_text(value2), "green");

  // Referenced value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    EnumeratedType ::= ENUMERATED { red, green, blue }\n"
      "    enumeratedValue1 EnumeratedType ::= green\n"
      "    enumeratedValue2 EnumeratedType ::= enumeratedValue1\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "enumeratedValue2");
  ut_assert_non_null_object(assignment3);
  UtObject *type3 = ut_asn1_type_value_get_type(assignment3);
  ut_assert_true(ut_object_is_asn1_referenced_type(type3));
  ut_assert_true(ut_object_is_asn1_enumerated_type(
      ut_asn1_referenced_type_get_type(type3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_implements_string(value3));
  ut_assert_cstring_equal(ut_string_get_text(value3), "green");

  // Invalid enumeration item.
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    enumeratedValue ENUMERATED { red, green, blue } ::= yellow\n"
      "END");
  ut_assert_is_error_with_description(module4,
                                      "Expected enumerated value, got yellow");
}

static void test_utf8_string_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    UTF8StringType ::= UTF8String\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "UTF8StringType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_utf8_string_type(type1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    UTF8StringType1 ::= UTF8String\n"
      "    UTF8StringType2 ::= UTF8StringType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "UTF8StringType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_utf8_string_type(
      ut_asn1_referenced_type_get_type(type2)));

  // Constrained to single value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    UTF8StringType ::= UTF8String (\"Hello World\")\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *type3 =
      ut_asn1_module_definition_get_assignment(module3, "UTF8StringType");
  ut_assert_non_null_object(type3);
  ut_assert_true(ut_object_is_asn1_constrained_type(type3));
  ut_assert_true(ut_object_is_asn1_utf8_string_type(
      ut_asn1_constrained_type_get_type(type3)));
  UtObject *constraint3 = ut_asn1_constrained_type_get_constraint(type3);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint3));
  UtObject *constraint_value3 = ut_asn1_value_constraint_get_value(constraint3);
  ut_assert_true(ut_object_implements_string(constraint_value3));
  ut_assert_cstring_equal(ut_string_get_text(constraint_value3), "Hello World");

  // Invalid type in constraint.
  UtObjectRef module10 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    UTF8StringType ::= UTF8String (NULL)\n"
      "END");
  ut_assert_is_error_with_description(module10, "Expected cstring, got NULL");
}

static void test_utf8_string_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    utf8StringValue UTF8String ::= \"Hello World!\"\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "utf8StringValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(ut_object_is_asn1_utf8_string_type(
      ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_string(value1));
  ut_assert_cstring_equal(ut_string_get_text(value1), "Hello World!");

  // Empty string.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    utf8StringValue UTF8String ::= \"\"\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "utf8StringValue");
  ut_assert_non_null_object(assignment2);
  ut_assert_true(ut_object_is_asn1_utf8_string_type(
      ut_asn1_type_value_get_type(assignment2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_implements_string(value2));
  ut_assert_cstring_equal(ut_string_get_text(value2), "");

  // Escaped quotes.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    utf8StringValue UTF8String ::= \"Hello \"\"World\"\"\"\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "utf8StringValue");
  ut_assert_non_null_object(assignment3);
  ut_assert_true(ut_object_is_asn1_utf8_string_type(
      ut_asn1_type_value_get_type(assignment3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_implements_string(value3));
  ut_assert_cstring_equal(ut_string_get_text(value3), "Hello \"World\"");

  // Referenced type.
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    UTF8StringType ::= UTF8String\n"
      "    utf8StringValue UTF8StringType ::= \"Hello World!\"\n"
      "END");
  ut_assert_is_not_error(module4);
  UtObject *assignment4 =
      ut_asn1_module_definition_get_assignment(module4, "utf8StringValue");
  ut_assert_non_null_object(assignment4);
  UtObject *type4 = ut_asn1_type_value_get_type(assignment4);
  ut_assert_true(ut_object_is_asn1_referenced_type(type4));
  ut_assert_true(ut_object_is_asn1_utf8_string_type(
      ut_asn1_referenced_type_get_type(type4)));
  UtObject *value4 = ut_asn1_type_value_get_value(assignment4);
  ut_assert_true(ut_object_implements_string(value4));
  ut_assert_cstring_equal(ut_string_get_text(value4), "Hello World!");

  // Referenced value.
  UtObjectRef module5 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    utf8StringValue1 UTF8String ::= \"Hello World!\"\n"
      "    utf8StringValue2 UTF8String ::= utf8StringValue1\n"
      "END");
  ut_assert_is_not_error(module5);
  UtObject *assignment5 =
      ut_asn1_module_definition_get_assignment(module5, "utf8StringValue2");
  ut_assert_non_null_object(assignment5);
  ut_assert_true(ut_object_is_asn1_utf8_string_type(
      ut_asn1_type_value_get_type(assignment5)));
  UtObject *value5 = ut_asn1_type_value_get_value(assignment5);
  ut_assert_true(ut_object_implements_string(value5));
  ut_assert_cstring_equal(ut_string_get_text(value5), "Hello World!");
}

static void test_relative_oid_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    RelativeOidType ::= RELATIVE-OID\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "RelativeOidType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_relative_oid_type(type1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    RelativeOidType1 ::= RELATIVE-OID\n"
      "    RelativeOidType2 ::= RelativeOidType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "RelativeOidType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_relative_oid_type(
      ut_asn1_referenced_type_get_type(type2)));

  // Constrained to single value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    RelativeOidType ::= RELATIVE-OID ({ 4 3 })\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *type3 =
      ut_asn1_module_definition_get_assignment(module3, "RelativeOidType");
  ut_assert_non_null_object(type3);
  ut_assert_true(ut_object_is_asn1_constrained_type(type3));
  ut_assert_true(ut_object_is_asn1_relative_oid_type(
      ut_asn1_constrained_type_get_type(type3)));
  UtObject *constraint3 = ut_asn1_constrained_type_get_constraint(type3);
  ut_assert_true(ut_object_is_asn1_value_constraint(constraint3));
  UtObject *constraint_value3 = ut_asn1_value_constraint_get_value(constraint3);
  ut_assert_true(ut_object_implements_uint32_list(constraint_value3));
  uint32_t expected_values3[2] = {4, 3};
  ut_assert_uint32_list_equal(constraint_value3, expected_values3, 2);

  // Invalid type in constraint.
  UtObjectRef module10 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    RelativeOidType ::= RELATIVE-OID (NULL)\n"
      "END");
  ut_assert_is_error_with_description(module10, "Expected {, got NULL");
}

static void test_relative_oid_value_assignment() {
  // Numbers.
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    relativeOIDValue RELATIVE-OID ::= { 4 3 } \n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "relativeOIDValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(ut_object_is_asn1_relative_oid_type(
      ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_uint32_list(value1));
  uint32_t expected_values1[2] = {4, 3};
  ut_assert_uint32_list_equal(value1, expected_values1, 2);

  // Names and numbers.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    relativeOIDValue RELATIVE-OID ::= { science-fac (4) "
      "maths-dept (3) } \n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "relativeOIDValue");
  ut_assert_non_null_object(assignment2);
  ut_assert_true(ut_object_is_asn1_relative_oid_type(
      ut_asn1_type_value_get_type(assignment2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_implements_uint32_list(value2));
  uint32_t expected_values2[2] = {4, 3};
  ut_assert_uint32_list_equal(value2, expected_values2, 2);

  // Extend existing relative OID.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    baseValue RELATIVE-OID ::= { science-fac (4) }\n"
      "    relativeOIDValue RELATIVE-OID ::= { baseValue maths-dept (3) } \n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "relativeOIDValue");
  ut_assert_non_null_object(assignment3);
  ut_assert_true(ut_object_is_asn1_relative_oid_type(
      ut_asn1_type_value_get_type(assignment3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_implements_uint32_list(value3));
  uint32_t expected_values3[2] = {4, 3};
  ut_assert_uint32_list_equal(value3, expected_values3, 2);
}

static void test_sequence_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SequenceType ::= SEQUENCE {\n"
      "        value1 BOOLEAN,\n"
      "        value2 REAL OPTIONAL,\n"
      "        value3 INTEGER DEFAULT 42\n"
      "    }\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "SequenceType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_sequence_type(type1));
  ut_assert_false(ut_asn1_sequence_type_get_extensible(type1));

  ut_assert_int_equal(
      ut_map_get_length(ut_asn1_sequence_type_get_components(type1)), 3);

  UtObject *type1a = ut_asn1_sequence_type_get_component(type1, "value1");
  ut_assert_non_null_object(type1a);
  ut_assert_true(ut_object_is_asn1_boolean_type(type1a));

  UtObject *type1b = ut_asn1_sequence_type_get_component(type1, "value2");
  ut_assert_non_null_object(type1b);
  ut_assert_true(ut_object_is_asn1_optional_type(type1b));
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_optional_type_get_type(type1b)));

  UtObject *type1c = ut_asn1_sequence_type_get_component(type1, "value3");
  ut_assert_non_null_object(type1c);
  ut_assert_true(ut_object_is_asn1_default_type(type1c));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_default_type_get_type(type1c)));
  UtObject *default_value1c = ut_asn1_default_type_get_default_value(type1c);
  ut_assert_true(ut_object_is_int64(default_value1c));
  ut_assert_int_equal(ut_int64_get_value(default_value1c), 42);

  // Empty sequence.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SequenceType ::= SEQUENCE {}\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "SequenceType");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_sequence_type(type2));
  ut_assert_int_equal(
      ut_map_get_length(ut_asn1_sequence_type_get_components(type2)), 0);

  // Referenced type.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SequenceType1 ::= SEQUENCE {\n"
      "        name UTF8String,\n"
      "        age INTEGER\n"
      "    }\n"
      "    SequenceType2 ::= SequenceType1\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *type3 =
      ut_asn1_module_definition_get_assignment(module3, "SequenceType2");
  ut_assert_non_null_object(type3);
  ut_assert_true(ut_object_is_asn1_referenced_type(type3));
  ut_assert_true(
      ut_object_is_asn1_sequence_type(ut_asn1_referenced_type_get_type(type3)));

  // Automatic tags.
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS AUTOMATIC TAGS ::= BEGIN\n"
      "    SequenceType ::= SEQUENCE {\n"
      "        value1 INTEGER,\n"
      "        value2 INTEGER\n"
      "    }\n"
      "END");
  ut_assert_is_not_error(module4);
  UtObject *type4 =
      ut_asn1_module_definition_get_assignment(module4, "SequenceType");
  ut_assert_non_null_object(type4);
  ut_assert_true(ut_object_is_asn1_sequence_type(type4));
  ut_assert_int_equal(
      ut_map_get_length(ut_asn1_sequence_type_get_components(type4)), 2);
  UtObject *type4a = ut_asn1_sequence_type_get_component(type4, "value1");
  ut_assert_non_null_object(type4a);
  ut_assert_true(ut_object_is_asn1_tagged_type(type4a));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_tagged_type_get_type(type4a)));
  UtObject *type4b = ut_asn1_sequence_type_get_component(type4, "value2");
  ut_assert_non_null_object(type4b);
  ut_assert_true(ut_object_is_asn1_tagged_type(type4b));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_tagged_type_get_type(type4b)));

  // Automatic tags when tags set.
  UtObjectRef module5 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS AUTOMATIC TAGS ::= BEGIN\n"
      "    SequenceType ::= SEQUENCE {\n"
      "        value1 [1] INTEGER,\n"
      "        value2 INTEGER\n"
      "    }\n"
      "END");
  ut_assert_is_not_error(module5);
  UtObject *type5 =
      ut_asn1_module_definition_get_assignment(module5, "SequenceType");
  ut_assert_non_null_object(type5);
  ut_assert_true(ut_object_is_asn1_sequence_type(type5));
  ut_assert_int_equal(
      ut_map_get_length(ut_asn1_sequence_type_get_components(type5)), 2);
  UtObject *type5a = ut_asn1_sequence_type_get_component(type5, "value1");
  ut_assert_non_null_object(type5a);
  ut_assert_true(ut_object_is_asn1_tagged_type(type5a));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_tagged_type_get_type(type5a)));
  UtObject *type5b = ut_asn1_sequence_type_get_component(type5, "value2");
  ut_assert_non_null_object(type5b);
  ut_assert_true(ut_object_is_asn1_integer_type(type5b));

  // Extensible.
  UtObjectRef module6 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SequenceType ::= SEQUENCE {\n"
      "        name UTF8String,\n"
      "        age INTEGER,\n"
      "        ...\n"
      "    }\n"
      "END");
  ut_assert_is_not_error(module6);
  UtObject *type6 =
      ut_asn1_module_definition_get_assignment(module6, "SequenceType");
  ut_assert_non_null_object(type6);
  ut_assert_true(ut_object_is_asn1_sequence_type(type6));
  ut_assert_true(ut_asn1_sequence_type_get_extensible(type6));

  // Extensibility implied.
  UtObjectRef module7 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS EXTENSIBILITY IMPLIED ::= BEGIN\n"
      "    SequenceType ::= SEQUENCE {\n"
      "        name UTF8String,\n"
      "        age INTEGER\n"
      "    }\n"
      "END");
  ut_assert_is_not_error(module7);
  UtObject *type7 =
      ut_asn1_module_definition_get_assignment(module7, "SequenceType");
  ut_assert_non_null_object(type7);
  ut_assert_true(ut_object_is_asn1_sequence_type(type7));
  ut_assert_true(ut_asn1_sequence_type_get_extensible(type7));

  // FIXME: Constrained to single value.

  // Missing comma.
  UtObjectRef module10 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SequenceType ::= SEQUENCE {\n"
      "        name UTF8String\n"
      "        age INTEGER\n"
      "    }\n"
      "END");
  ut_assert_is_error_with_description(module10, "Expected ,, got age");

  // Trailing comma.
  UtObjectRef module11 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SequenceType ::= SEQUENCE {\n"
      "        name UTF8String,\n"
      "        age INTEGER,\n"
      "    }\n"
      "END");
  ut_assert_is_error_with_description(module11, "Expected identifier, got }");

  // Duplicate components.
  UtObjectRef module12 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SequenceType ::= SEQUENCE {\n"
      "        name UTF8String,\n"
      "        name INTEGER\n"
      "    }\n"
      "END");
  ut_assert_is_error_with_description(module12,
                                      "Duplicate component identifier name");

  // Duplicate type (needs tags).
  UtObjectRef module13 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SequenceType ::= SEQUENCE {\n"
      "        value1 INTEGER OPTIONAL,\n"
      "        value2 INTEGER OPTIONAL\n"
      "    }\n"
      "END");
  ut_assert_is_error_with_description(
      module13, "Components value1 and value2 has the same tag");

  // Duplicate extensible.
  UtObjectRef module14 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SequenceType ::= SEQUENCE {\n"
      "        name UTF8String,\n"
      "        age INTEGER,\n"
      "        ...,\n"
      "        ...\n"
      "    }\n"
      "END");
  ut_assert_is_error_with_description(module14, "Duplicate extensible marker");

  // FIXME: Constrained to single value.
}

static void test_sequence_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    sequenceValue SEQUENCE {\n"
      "        name UTF8String,\n"
      "        age INTEGER\n"
      "    } ::= { name \"Arthur Dent\", age 42 }\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "sequenceValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(ut_object_is_asn1_sequence_type(
      ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_map(value1));
  ut_assert_int_equal(ut_map_get_length(value1), 2);
  UtObject *value1a = ut_map_lookup_string(value1, "name");
  ut_assert_non_null_object(value1a);
  ut_assert_true(ut_object_implements_string(value1a));
  ut_assert_cstring_equal(ut_string_get_text(value1a), "Arthur Dent");
  UtObject *value1b = ut_map_lookup_string(value1, "age");
  ut_assert_non_null_object(value1b);
  ut_assert_true(ut_object_is_int64(value1b));
  ut_assert_int_equal(ut_int64_get_value(value1b), 42);
}

static void test_sequence_of_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SequenceOfType ::= SEQUENCE OF INTEGER\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "SequenceOfType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_sequence_of_type(type1));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_sequence_of_type_get_type(type1)));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SequenceOfType1 ::= SEQUENCE OF INTEGER\n"
      "    SequenceOfType2 ::= SequenceOfType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "SequenceOfType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  UtObject *type2i = ut_asn1_referenced_type_get_type(type2);
  ut_assert_true(ut_object_is_asn1_sequence_of_type(type2i));
  ut_assert_true(ut_object_is_asn1_integer_type(
      ut_asn1_sequence_of_type_get_type(type2i)));

  // FIXME: Constrained to single value.

  // Missing child type.
  UtObjectRef module3 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    SetOfType ::= SEQUENCE OF \n"
                                              "END");
  ut_assert_is_error_with_description(module3,
                                      "Expected typereference, got END");
}

static void test_sequence_of_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    sequenceOfValue SEQUENCE OF INTEGER ::= { 1, 2, 3, 4 }\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "sequenceOfValue");
  ut_assert_non_null_object(assignment1);
  UtObject *type1 = ut_asn1_type_value_get_type(assignment1);
  ut_assert_true(ut_object_is_asn1_sequence_of_type(type1));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_list(value1));
  ut_assert_int_equal(ut_list_get_length(value1), 4);
  for (size_t i = 0; i < 4; i++) {
    UtObjectRef value = ut_list_get_element(value1, i);
    ut_assert_true(ut_object_is_int64(value));
    ut_assert_int_equal(ut_int64_get_value(value), i + 1);
  }

  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    sequenceOfValue SEQUENCE OF INTEGER ::= {}\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "sequenceOfValue");
  ut_assert_non_null_object(assignment2);
  UtObject *type2 = ut_asn1_type_value_get_type(assignment2);
  ut_assert_true(ut_object_is_asn1_sequence_of_type(type2));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_implements_list(value2));
  ut_assert_int_equal(ut_list_get_length(value2), 0);

  // Referenced type.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SequenceOfType ::= SEQUENCE OF INTEGER\n"
      "    sequenceOfValue SequenceOfType ::= { 1, 2, 3, 4 }\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "sequenceOfValue");
  ut_assert_non_null_object(assignment3);
  UtObject *type3 = ut_asn1_type_value_get_type(assignment3);
  ut_assert_true(ut_object_is_asn1_referenced_type(type3));
  ut_assert_true(ut_object_is_asn1_sequence_of_type(
      ut_asn1_referenced_type_get_type(type3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_implements_list(value3));
  ut_assert_int_equal(ut_list_get_length(value3), 4);
  for (size_t i = 0; i < 4; i++) {
    UtObjectRef value = ut_list_get_element(value3, i);
    ut_assert_true(ut_object_is_int64(value));
    ut_assert_int_equal(ut_int64_get_value(value), i + 1);
  }
}

static void test_set_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SetType ::= SET {\n"
      "        value1 BOOLEAN,\n"
      "        value2 REAL OPTIONAL,\n"
      "        value3 INTEGER DEFAULT 42\n"
      "    }\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "SetType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_set_type(type1));

  ut_assert_int_equal(ut_map_get_length(ut_asn1_set_type_get_components(type1)),
                      3);

  UtObject *type1a = ut_asn1_set_type_get_component(type1, "value1");
  ut_assert_non_null_object(type1a);
  ut_assert_true(ut_object_is_asn1_boolean_type(type1a));

  UtObject *type1b = ut_asn1_set_type_get_component(type1, "value2");
  ut_assert_non_null_object(type1b);
  ut_assert_true(ut_object_is_asn1_optional_type(type1b));
  ut_assert_true(
      ut_object_is_asn1_real_type(ut_asn1_optional_type_get_type(type1b)));

  UtObject *type1c = ut_asn1_set_type_get_component(type1, "value3");
  ut_assert_non_null_object(type1c);
  ut_assert_true(ut_object_is_asn1_default_type(type1c));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_default_type_get_type(type1c)));
  UtObject *default_value1c = ut_asn1_default_type_get_default_value(type1c);
  ut_assert_true(ut_object_is_int64(default_value1c));
  ut_assert_int_equal(ut_int64_get_value(default_value1c), 42);

  // Empty set.
  UtObjectRef module2 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    SetType ::= SET {}\n"
                                              "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "SetType");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_set_type(type2));
  ut_assert_int_equal(ut_map_get_length(ut_asn1_set_type_get_components(type2)),
                      0);

  // Referenced type.
  UtObjectRef module3 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    SetType1 ::= SET {\n"
                                              "        name UTF8String,\n"
                                              "        age INTEGER\n"
                                              "    }\n"
                                              "    SetType2 ::= SetType1\n"
                                              "END");
  ut_assert_is_not_error(module3);
  UtObject *type3 =
      ut_asn1_module_definition_get_assignment(module3, "SetType2");
  ut_assert_non_null_object(type3);
  ut_assert_true(ut_object_is_asn1_referenced_type(type3));
  ut_assert_true(
      ut_object_is_asn1_set_type(ut_asn1_referenced_type_get_type(type3)));

  // Automatic tags.
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS AUTOMATIC TAGS ::= BEGIN\n"
      "    SetType ::= SET {\n"
      "        value1 INTEGER,\n"
      "        value2 INTEGER\n"
      "    }\n"
      "END");
  ut_assert_is_not_error(module4);
  UtObject *type4 =
      ut_asn1_module_definition_get_assignment(module4, "SetType");
  ut_assert_non_null_object(type4);
  ut_assert_true(ut_object_is_asn1_set_type(type4));
  ut_assert_int_equal(ut_map_get_length(ut_asn1_set_type_get_components(type4)),
                      2);
  UtObject *type4a = ut_asn1_set_type_get_component(type4, "value1");
  ut_assert_non_null_object(type4a);
  ut_assert_true(ut_object_is_asn1_tagged_type(type4a));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_tagged_type_get_type(type4a)));
  UtObject *type4b = ut_asn1_set_type_get_component(type4, "value2");
  ut_assert_non_null_object(type4b);
  ut_assert_true(ut_object_is_asn1_tagged_type(type4b));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_tagged_type_get_type(type4b)));

  // Automatic tags when tags set.
  UtObjectRef module5 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS AUTOMATIC TAGS ::= BEGIN\n"
      "    SetType ::= SET {\n"
      "        value1 [1] INTEGER,\n"
      "        value2 INTEGER\n"
      "    }\n"
      "END");
  ut_assert_is_not_error(module5);
  UtObject *type5 =
      ut_asn1_module_definition_get_assignment(module5, "SetType");
  ut_assert_non_null_object(type5);
  ut_assert_true(ut_object_is_asn1_set_type(type5));
  ut_assert_int_equal(ut_map_get_length(ut_asn1_set_type_get_components(type5)),
                      2);
  UtObject *type5a = ut_asn1_set_type_get_component(type5, "value1");
  ut_assert_non_null_object(type5a);
  ut_assert_true(ut_object_is_asn1_tagged_type(type5a));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_tagged_type_get_type(type5a)));
  UtObject *type5b = ut_asn1_set_type_get_component(type5, "value2");
  ut_assert_non_null_object(type5b);
  ut_assert_true(ut_object_is_asn1_integer_type(type5b));

  // Extensible.
  UtObjectRef module6 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    SetType ::= SET {\n"
                                              "        name UTF8String,\n"
                                              "        age INTEGER,\n"
                                              "        ...\n"
                                              "    }\n"
                                              "END");
  ut_assert_is_not_error(module6);
  UtObject *type6 =
      ut_asn1_module_definition_get_assignment(module6, "SetType");
  ut_assert_non_null_object(type6);
  ut_assert_true(ut_object_is_asn1_set_type(type6));
  ut_assert_true(ut_asn1_set_type_get_extensible(type6));

  // Extensibility implied.
  UtObjectRef module7 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS EXTENSIBILITY IMPLIED ::= BEGIN\n"
      "    SetType ::= SET {\n"
      "        name UTF8String,\n"
      "        age INTEGER\n"
      "    }\n"
      "END");
  ut_assert_is_not_error(module7);
  UtObject *type7 =
      ut_asn1_module_definition_get_assignment(module7, "SetType");
  ut_assert_non_null_object(type7);
  ut_assert_true(ut_object_is_asn1_set_type(type7));
  ut_assert_true(ut_asn1_set_type_get_extensible(type7));

  // FIXME: Constrained to single value.

  // Missing comma.
  UtObjectRef module10 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    SetType ::= SET {\n"
                                              "        name UTF8String\n"
                                              "        age INTEGER\n"
                                              "    }\n"
                                              "END");
  ut_assert_is_error_with_description(module10, "Expected ,, got age");

  // Trailing comma.
  UtObjectRef module11 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    SetType ::= SET {\n"
                                              "        name UTF8String,\n"
                                              "        age INTEGER,\n"
                                              "    }\n"
                                              "END");
  ut_assert_is_error_with_description(module11, "Expected identifier, got }");

  // Duplicate components.
  UtObjectRef module12 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    SetType ::= SET {\n"
                                              "        name UTF8String,\n"
                                              "        name INTEGER\n"
                                              "    }\n"
                                              "END");
  ut_assert_is_error_with_description(module12,
                                      "Duplicate component identifier name");

  // Duplicate type (needs tags).
  UtObjectRef module13 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    SetType ::= SET {\n"
                                              "        value1 INTEGER,\n"
                                              "        value2 INTEGER\n"
                                              "    }\n"
                                              "END");
  ut_assert_is_error_with_description(
      module13, "Components value1 and value2 has the same tag");

  // Duplicate extensible.
  UtObjectRef module14 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    SetType ::= SET {\n"
                                              "        name UTF8String,\n"
                                              "        age INTEGER,\n"
                                              "        ...,\n"
                                              "        ...\n"
                                              "    }\n"
                                              "END");
  ut_assert_is_error_with_description(module14, "Duplicate extensible marker");
}

static void test_set_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    setValue SET {\n"
      "        name UTF8String,\n"
      "        age INTEGER\n"
      "    } ::= { name \"Arthur Dent\", age 42 }\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "setValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(
      ut_object_is_asn1_set_type(ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_map(value1));
  ut_assert_int_equal(ut_map_get_length(value1), 2);
  UtObject *value1a = ut_map_lookup_string(value1, "name");
  ut_assert_non_null_object(value1a);
  ut_assert_true(ut_object_implements_string(value1a));
  ut_assert_cstring_equal(ut_string_get_text(value1a), "Arthur Dent");
  UtObject *value1b = ut_map_lookup_string(value1, "age");
  ut_assert_non_null_object(value1b);
  ut_assert_true(ut_object_is_int64(value1b));
  ut_assert_int_equal(ut_int64_get_value(value1b), 42);
}

static void test_set_of_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SetOfType ::= SET OF INTEGER\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "SetOfType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_set_of_type(type1));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_set_of_type_get_type(type1)));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SetOfType1 ::= SET OF INTEGER\n"
      "    SetOfType2 ::= SetOfType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "SetOfType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  UtObject *type2i = ut_asn1_referenced_type_get_type(type2);
  ut_assert_true(ut_object_is_asn1_set_of_type(type2i));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_set_of_type_get_type(type2i)));

  // FIXME: Constrained to single value.

  // Missing child type.
  UtObjectRef module3 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    SetOfType ::= SET OF \n"
                                              "END");
  ut_assert_is_error_with_description(module3,
                                      "Expected typereference, got END");
}

static void test_set_of_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    setOfValue SET OF INTEGER ::= { 1, 2, 3, 4 }\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "setOfValue");
  ut_assert_non_null_object(assignment1);
  UtObject *type1 = ut_asn1_type_value_get_type(assignment1);
  ut_assert_true(ut_object_is_asn1_set_of_type(type1));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_list(value1));
  ut_assert_int_equal(ut_list_get_length(value1), 4);
  for (size_t i = 0; i < 4; i++) {
    UtObjectRef value = ut_list_get_element(value1, i);
    ut_assert_true(ut_object_is_int64(value));
    ut_assert_int_equal(ut_int64_get_value(value), i + 1);
  }

  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    setOfValue SET OF INTEGER ::= {}\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "setOfValue");
  ut_assert_non_null_object(assignment2);
  UtObject *type2 = ut_asn1_type_value_get_type(assignment2);
  ut_assert_true(ut_object_is_asn1_set_of_type(type2));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_implements_list(value2));
  ut_assert_int_equal(ut_list_get_length(value2), 0);

  // Referenced type.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    SetOfType ::= SET OF INTEGER\n"
      "    setOfValue SetOfType ::= { 1, 2, 3, 4 }\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "setOfValue");
  ut_assert_non_null_object(assignment3);
  UtObject *type3 = ut_asn1_type_value_get_type(assignment3);
  ut_assert_true(ut_object_is_asn1_referenced_type(type3));
  ut_assert_true(
      ut_object_is_asn1_set_of_type(ut_asn1_referenced_type_get_type(type3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_implements_list(value3));
  ut_assert_int_equal(ut_list_get_length(value3), 4);
  for (size_t i = 0; i < 4; i++) {
    UtObjectRef value = ut_list_get_element(value3, i);
    ut_assert_true(ut_object_is_int64(value));
    ut_assert_int_equal(ut_int64_get_value(value), i + 1);
  }
}

static void test_choice_type_assignment() {
  UtObjectRef module1 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    ChoiceType ::= CHOICE {\n"
                                              "        value1 BOOLEAN,\n"
                                              "        value2 INTEGER\n"
                                              "    }\n"
                                              "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "ChoiceType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_choice_type(type1));
  ut_assert_int_equal(
      ut_map_get_length(ut_asn1_choice_type_get_components(type1)), 2);
  UtObject *type1a = ut_asn1_choice_type_get_component(type1, "value1");
  ut_assert_non_null_object(type1a);
  ut_assert_true(ut_object_is_asn1_boolean_type(type1a));
  UtObject *type1b = ut_asn1_choice_type_get_component(type1, "value2");
  ut_assert_non_null_object(type1b);
  ut_assert_true(ut_object_is_asn1_integer_type(type1b));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ChoiceType1 ::= CHOICE {\n"
      "        value1 BOOLEAN,\n"
      "        value2 INTEGER\n"
      "    }\n"
      "    ChoiceType2 ::= ChoiceType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "ChoiceType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  UtObject *type2i = ut_asn1_referenced_type_get_type(type2);
  ut_assert_true(ut_object_is_asn1_choice_type(type2i));
  ut_assert_int_equal(
      ut_map_get_length(ut_asn1_choice_type_get_components(type2i)), 2);
  UtObject *type2a = ut_asn1_choice_type_get_component(type2i, "value1");
  ut_assert_non_null_object(type2a);
  ut_assert_true(ut_object_is_asn1_boolean_type(type2a));
  UtObject *type2b = ut_asn1_choice_type_get_component(type2i, "value2");
  ut_assert_non_null_object(type2b);
  ut_assert_true(ut_object_is_asn1_integer_type(type2b));

  // Use of tags for duplicate type
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ChoiceType1 ::= CHOICE {\n"
      "        value1 INTEGER,\n"
      "        value2 [1] INTEGER\n"
      "    }\n"
      "    ChoiceType2 ::= ChoiceType1\n"
      "END");
  ut_assert_is_not_error(module2);

  // Automatic tags.
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS AUTOMATIC TAGS ::= BEGIN\n"
      "    ChoiceType ::= CHOICE {\n"
      "        value1 INTEGER,\n"
      "        value2 INTEGER\n"
      "    }\n"
      "END");
  ut_assert_is_not_error(module4);
  UtObject *type4 =
      ut_asn1_module_definition_get_assignment(module4, "ChoiceType");
  ut_assert_non_null_object(type4);
  ut_assert_true(ut_object_is_asn1_choice_type(type4));
  ut_assert_int_equal(
      ut_map_get_length(ut_asn1_choice_type_get_components(type4)), 2);
  UtObject *type4a = ut_asn1_choice_type_get_component(type4, "value1");
  ut_assert_non_null_object(type4a);
  ut_assert_true(ut_object_is_asn1_tagged_type(type4a));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_tagged_type_get_type(type4a)));
  UtObject *type4b = ut_asn1_choice_type_get_component(type4, "value2");
  ut_assert_non_null_object(type4b);
  ut_assert_true(ut_object_is_asn1_tagged_type(type4b));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_tagged_type_get_type(type4b)));

  // Automatic tags when tags set.
  UtObjectRef module5 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS AUTOMATIC TAGS ::= BEGIN\n"
      "    ChoiceType ::= CHOICE {\n"
      "        value1 [1] INTEGER,\n"
      "        value2 INTEGER\n"
      "    }\n"
      "END");
  ut_assert_is_not_error(module5);
  UtObject *type5 =
      ut_asn1_module_definition_get_assignment(module5, "ChoiceType");
  ut_assert_non_null_object(type5);
  ut_assert_true(ut_object_is_asn1_choice_type(type5));
  ut_assert_int_equal(
      ut_map_get_length(ut_asn1_choice_type_get_components(type5)), 2);
  UtObject *type5a = ut_asn1_choice_type_get_component(type5, "value1");
  ut_assert_non_null_object(type5a);
  ut_assert_true(ut_object_is_asn1_tagged_type(type5a));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_tagged_type_get_type(type5a)));
  UtObject *type5b = ut_asn1_choice_type_get_component(type5, "value2");
  ut_assert_non_null_object(type5b);
  ut_assert_true(ut_object_is_asn1_integer_type(type5b));

  // Extensible.
  UtObjectRef module6 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    ChoiceType ::= CHOICE {\n"
                                              "        value1 BOOLEAN,\n"
                                              "        value2 INTEGER,\n"
                                              "        ...\n"
                                              "    }\n"
                                              "END");
  ut_assert_is_not_error(module6);
  UtObject *type6 =
      ut_asn1_module_definition_get_assignment(module6, "ChoiceType");
  ut_assert_non_null_object(type6);
  ut_assert_true(ut_object_is_asn1_choice_type(type6));
  ut_assert_true(ut_asn1_choice_type_get_extensible(type6));

  // Extensibility implied.
  UtObjectRef module7 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS EXTENSIBILITY IMPLIED ::= BEGIN\n"
      "    ChoiceType ::= CHOICE {\n"
      "        value1 BOOLEAN,\n"
      "        value2 INTEGER\n"
      "    }\n"
      "END");
  ut_assert_is_not_error(module7);
  UtObject *type7 =
      ut_asn1_module_definition_get_assignment(module7, "ChoiceType");
  ut_assert_non_null_object(type7);
  ut_assert_true(ut_object_is_asn1_choice_type(type7));
  ut_assert_true(ut_asn1_choice_type_get_extensible(type7));

  // Empty choice.
  UtObjectRef module9 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    ChoiceType ::= CHOICE {}\n"
                                              "END");
  ut_assert_is_error_with_description(module9, "Empty choice");

  // Missing comma.
  UtObjectRef module10 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    ChoiceType ::= CHOICE {\n"
                                              "        value1 BOOLEAN\n"
                                              "        value2 INTEGER\n"
                                              "    }\n"
                                              "END");
  ut_assert_is_error_with_description(module10, "Expected ,, got value2");

  // Trailing comma.
  UtObjectRef module11 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    ChoiceType ::= CHOICE {\n"
                                              "        value1 BOOLEAN,\n"
                                              "        value2 INTEGER,\n"
                                              "    }\n"
                                              "END");
  ut_assert_is_error_with_description(module11, "Expected identifier, got }");

  // Duplicate components.
  UtObjectRef module12 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    ChoiceType ::= CHOICE {\n"
                                              "        value1 BOOLEAN,\n"
                                              "        value1 INTEGER\n"
                                              "    }\n"
                                              "END");
  ut_assert_is_error_with_description(module12,
                                      "Duplicate choice option value1");

  // Duplicate type (needs tags).
  UtObjectRef module13 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    ChoiceType ::= CHOICE {\n"
                                              "        value1 INTEGER,\n"
                                              "        value2 INTEGER\n"
                                              "    }\n"
                                              "END");
  ut_assert_is_error_with_description(
      module13, "Components value1 and value2 has the same tag");

  // Duplicate extensible.
  UtObjectRef module14 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    ChoiceType ::= CHOICE {\n"
                                              "        value1 BOOLEAN,\n"
                                              "        value2 INTEGER,\n"
                                              "        ...,\n"
                                              "        ...\n"
                                              "    }\n"
                                              "END");
  ut_assert_is_error_with_description(module14, "Duplicate extensible marker");

  // Empty but extensible.
  UtObjectRef module15 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    ChoiceType ::= CHOICE {\n"
                                              "        ...\n"
                                              "    }\n"
                                              "END");
  ut_assert_is_error_with_description(module15, "Empty choice");
}

static void test_choice_value_assignment() {
  UtObjectRef module1 =
      ut_asn1_module_definition_new_from_text("Test DEFINITIONS ::= BEGIN\n"
                                              "    choiceValue CHOICE {\n"
                                              "        value1 BOOLEAN,\n"
                                              "        value2 INTEGER\n"
                                              "    } ::= value2 : 42\n"
                                              "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "choiceValue");
  ut_assert_non_null_object(assignment1);
  UtObject *type1 = ut_asn1_type_value_get_type(assignment1);
  ut_assert_true(ut_object_is_asn1_choice_type(type1));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_is_int64(value1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ChoiceType ::= CHOICE {\n"
      "        value1 BOOLEAN,\n"
      "        value2 INTEGER\n"
      "    }\n"
      "    choiceValue ChoiceType ::= value2 : 42\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "choiceValue");
  ut_assert_non_null_object(assignment2);
  UtObject *type2 = ut_asn1_type_value_get_type(assignment2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(
      ut_object_is_asn1_choice_type(ut_asn1_referenced_type_get_type(type2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_is_int64(value2));

  // Referenced value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ChoiceType ::= CHOICE {\n"
      "        value1 BOOLEAN,\n"
      "        value2 INTEGER\n"
      "    }\n"
      "    choiceValue1 ChoiceType ::= value2 : 42\n"
      "    choiceValue2 ChoiceType ::= choiceValue1\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "choiceValue2");
  ut_assert_non_null_object(assignment3);
  UtObject *type3 = ut_asn1_type_value_get_type(assignment3);
  ut_assert_true(ut_object_is_asn1_referenced_type(type3));
  ut_assert_true(
      ut_object_is_asn1_choice_type(ut_asn1_referenced_type_get_type(type3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_is_int64(value3));
}

static void test_numeric_string_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    NumericStringType ::= NumericString\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "NumericStringType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_numeric_string_type(type1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    NumericStringType1 ::= NumericString\n"
      "    NumericStringType2 ::= NumericStringType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "NumericStringType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_numeric_string_type(
      ut_asn1_referenced_type_get_type(type2)));
}

static void test_numeric_string_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    numericStringValue NumericString ::= \"12345 67890\"\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "numericStringValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(ut_object_is_asn1_numeric_string_type(
      ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_string(value1));
  ut_assert_cstring_equal(ut_string_get_text(value1), "12345 67890");

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    NumericStringType ::= NumericString\n"
      "    numericStringValue NumericStringType ::= \"12345 67890\"\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "numericStringValue");
  ut_assert_non_null_object(assignment2);
  UtObject *type2 = ut_asn1_type_value_get_type(assignment2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_numeric_string_type(
      ut_asn1_referenced_type_get_type(type2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_implements_string(value2));
  ut_assert_cstring_equal(ut_string_get_text(value2), "12345 67890");

  // Referenced value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    numericStringValue1 NumericString ::= \"12345 67890\"\n"
      "    numericStringValue2 NumericString ::= numericStringValue1\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "numericStringValue2");
  ut_assert_non_null_object(assignment3);
  ut_assert_true(ut_object_is_asn1_numeric_string_type(
      ut_asn1_type_value_get_type(assignment3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_implements_string(value3));
  ut_assert_cstring_equal(ut_string_get_text(value3), "12345 67890");
}

static void test_printable_string_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    PrintableStringType ::= PrintableString\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "PrintableStringType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_printable_string_type(type1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    PrintableStringType1 ::= PrintableString\n"
      "    PrintableStringType2 ::= PrintableStringType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "PrintableStringType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_printable_string_type(
      ut_asn1_referenced_type_get_type(type2)));
}

static void test_printable_string_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    printableStringValue PrintableString ::= \"Hello World\"\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "printableStringValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(ut_object_is_asn1_printable_string_type(
      ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_string(value1));
  ut_assert_cstring_equal(ut_string_get_text(value1), "Hello World");

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    PrintableStringType ::= PrintableString\n"
      "    printableStringValue PrintableStringType ::= \"Hello World\"\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "printableStringValue");
  ut_assert_non_null_object(assignment2);
  UtObject *type2 = ut_asn1_type_value_get_type(assignment2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_printable_string_type(
      ut_asn1_referenced_type_get_type(type2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_implements_string(value2));
  ut_assert_cstring_equal(ut_string_get_text(value2), "Hello World");

  // Referenced value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    printableStringValue1 PrintableString ::= \"Hello World\"\n"
      "    printableStringValue2 PrintableString ::= printableStringValue1\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 = ut_asn1_module_definition_get_assignment(
      module3, "printableStringValue2");
  ut_assert_non_null_object(assignment3);
  ut_assert_true(ut_object_is_asn1_printable_string_type(
      ut_asn1_type_value_get_type(assignment3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_implements_string(value3));
  ut_assert_cstring_equal(ut_string_get_text(value3), "Hello World");
}

static void test_ia5_string_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    IA5StringType ::= IA5String\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "IA5StringType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_ia5_string_type(type1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    IA5StringType1 ::= IA5String\n"
      "    IA5StringType2 ::= IA5StringType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "IA5StringType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_ia5_string_type(
      ut_asn1_referenced_type_get_type(type2)));
}

static void test_ia5_string_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ia5StringValue IA5String ::= \"Hello World\"\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "ia5StringValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(ut_object_is_asn1_ia5_string_type(
      ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_string(value1));
  ut_assert_cstring_equal(ut_string_get_text(value1), "Hello World");

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    IA5StringType ::= IA5String\n"
      "    ia5StringValue IA5StringType ::= \"Hello World\"\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "ia5StringValue");
  ut_assert_non_null_object(assignment2);
  UtObject *type2 = ut_asn1_type_value_get_type(assignment2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_ia5_string_type(
      ut_asn1_referenced_type_get_type(type2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_implements_string(value2));
  ut_assert_cstring_equal(ut_string_get_text(value2), "Hello World");

  // Referenced value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ia5StringValue1 IA5String ::= \"Hello World\"\n"
      "    ia5StringValue2 IA5String ::= ia5StringValue1\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "ia5StringValue2");
  ut_assert_non_null_object(assignment3);
  ut_assert_true(ut_object_is_asn1_ia5_string_type(
      ut_asn1_type_value_get_type(assignment3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_implements_string(value3));
  ut_assert_cstring_equal(ut_string_get_text(value3), "Hello World");
}

static void test_graphic_string_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    GraphicStringType ::= GraphicString\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "GraphicStringType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_graphic_string_type(type1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    GraphicStringType1 ::= GraphicString\n"
      "    GraphicStringType2 ::= GraphicStringType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "GraphicStringType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_graphic_string_type(
      ut_asn1_referenced_type_get_type(type2)));
}

static void test_graphic_string_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    graphicStringValue GraphicString ::= \"Hello World\"\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "graphicStringValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(ut_object_is_asn1_graphic_string_type(
      ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_string(value1));
  ut_assert_cstring_equal(ut_string_get_text(value1), "Hello World");

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    GraphicStringType ::= GraphicString\n"
      "    graphicStringValue GraphicStringType ::= \"Hello World\"\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "graphicStringValue");
  ut_assert_non_null_object(assignment2);
  UtObject *type2 = ut_asn1_type_value_get_type(assignment2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_graphic_string_type(
      ut_asn1_referenced_type_get_type(type2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_implements_string(value2));
  ut_assert_cstring_equal(ut_string_get_text(value2), "Hello World");

  // Referenced value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    graphicStringValue1 GraphicString ::= \"Hello World\"\n"
      "    graphicStringValue2 GraphicString ::= graphicStringValue1\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "graphicStringValue2");
  ut_assert_non_null_object(assignment3);
  ut_assert_true(ut_object_is_asn1_graphic_string_type(
      ut_asn1_type_value_get_type(assignment3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_implements_string(value3));
  ut_assert_cstring_equal(ut_string_get_text(value3), "Hello World");
}

static void test_visible_string_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    VisibleStringType ::= VisibleString\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "VisibleStringType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_visible_string_type(type1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    VisibleStringType1 ::= VisibleString\n"
      "    VisibleStringType2 ::= VisibleStringType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "VisibleStringType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_visible_string_type(
      ut_asn1_referenced_type_get_type(type2)));
}

static void test_visible_string_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    visibleStringValue VisibleString ::= \"Hello World\"\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "visibleStringValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(ut_object_is_asn1_visible_string_type(
      ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_string(value1));
  ut_assert_cstring_equal(ut_string_get_text(value1), "Hello World");

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    VisibleStringType ::= VisibleString\n"
      "    visibleStringValue VisibleStringType ::= \"Hello World\"\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "visibleStringValue");
  ut_assert_non_null_object(assignment2);
  UtObject *type2 = ut_asn1_type_value_get_type(assignment2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_visible_string_type(
      ut_asn1_referenced_type_get_type(type2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_implements_string(value2));
  ut_assert_cstring_equal(ut_string_get_text(value2), "Hello World");

  // Referenced value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    visibleStringValue1 VisibleString ::= \"Hello World\"\n"
      "    visibleStringValue2 VisibleString ::= visibleStringValue1\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "visibleStringValue2");
  ut_assert_non_null_object(assignment3);
  ut_assert_true(ut_object_is_asn1_visible_string_type(
      ut_asn1_type_value_get_type(assignment3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_implements_string(value3));
  ut_assert_cstring_equal(ut_string_get_text(value3), "Hello World");
}

static void test_general_string_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    GeneralStringType ::= GeneralString\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *type1 =
      ut_asn1_module_definition_get_assignment(module1, "GeneralStringType");
  ut_assert_non_null_object(type1);
  ut_assert_true(ut_object_is_asn1_general_string_type(type1));

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    GeneralStringType1 ::= GeneralString\n"
      "    GeneralStringType2 ::= GeneralStringType1\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *type2 =
      ut_asn1_module_definition_get_assignment(module2, "GeneralStringType2");
  ut_assert_non_null_object(type2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_general_string_type(
      ut_asn1_referenced_type_get_type(type2)));
}

static void test_general_string_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    generalStringValue GeneralString ::= \"Hello World\"\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "generalStringValue");
  ut_assert_non_null_object(assignment1);
  ut_assert_true(ut_object_is_asn1_general_string_type(
      ut_asn1_type_value_get_type(assignment1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_implements_string(value1));
  ut_assert_cstring_equal(ut_string_get_text(value1), "Hello World");

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    GeneralStringType ::= GeneralString\n"
      "    generalStringValue GeneralStringType ::= \"Hello World\"\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "generalStringValue");
  ut_assert_non_null_object(assignment2);
  UtObject *type2 = ut_asn1_type_value_get_type(assignment2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  ut_assert_true(ut_object_is_asn1_general_string_type(
      ut_asn1_referenced_type_get_type(type2)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_implements_string(value2));
  ut_assert_cstring_equal(ut_string_get_text(value2), "Hello World");

  // Referenced value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    generalStringValue1 GeneralString ::= \"Hello World\"\n"
      "    generalStringValue2 GeneralString ::= generalStringValue1\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "generalStringValue2");
  ut_assert_non_null_object(assignment3);
  ut_assert_true(ut_object_is_asn1_general_string_type(
      ut_asn1_type_value_get_type(assignment3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_implements_string(value3));
  ut_assert_cstring_equal(ut_string_get_text(value3), "Hello World");
}

static void test_tagged_type_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    ContextTaggedType ::= [42] OCTET STRING\n"
      "    UniversalTaggedType ::= [UNIVERSAL 12] OCTET STRING\n"
      "    ApplicationTaggedType ::= [APPLICATION 123] OCTET STRING\n"
      "    PrivateTaggedType ::= [PRIVATE 3] OCTET STRING\n"
      "END");
  ut_assert_is_not_error(module1);

  UtObject *type1a =
      ut_asn1_module_definition_get_assignment(module1, "ContextTaggedType");
  ut_assert_non_null_object(type1a);
  ut_assert_true(ut_object_is_asn1_tagged_type(type1a));
  ut_assert_int_equal(ut_asn1_tagged_type_get_class(type1a),
                      UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC);
  ut_assert_int_equal(ut_asn1_tagged_type_get_number(type1a), 42);

  UtObject *type1b =
      ut_asn1_module_definition_get_assignment(module1, "UniversalTaggedType");
  ut_assert_non_null_object(type1b);
  ut_assert_true(ut_object_is_asn1_tagged_type(type1b));
  ut_assert_int_equal(ut_asn1_tagged_type_get_class(type1b),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_tagged_type_get_number(type1b), 12);

  UtObject *type1c = ut_asn1_module_definition_get_assignment(
      module1, "ApplicationTaggedType");
  ut_assert_non_null_object(type1c);
  ut_assert_true(ut_object_is_asn1_tagged_type(type1c));
  ut_assert_int_equal(ut_asn1_tagged_type_get_class(type1c),
                      UT_ASN1_TAG_CLASS_APPLICATION);
  ut_assert_int_equal(ut_asn1_tagged_type_get_number(type1c), 123);

  UtObject *type1d =
      ut_asn1_module_definition_get_assignment(module1, "PrivateTaggedType");
  ut_assert_non_null_object(type1d);
  ut_assert_true(ut_object_is_asn1_tagged_type(type1d));
  ut_assert_int_equal(ut_asn1_tagged_type_get_class(type1d),
                      UT_ASN1_TAG_CLASS_PRIVATE);
  ut_assert_int_equal(ut_asn1_tagged_type_get_number(type1d), 3);

  // Explicit/implicit tagging - unset.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    TaggedType1 ::= [1] INTEGER\n"
      "    TaggedType2 ::= [2] EXPLICIT INTEGER\n"
      "    TaggedType3 ::= [3] IMPLICIT INTEGER\n"
      "END");
  ut_assert_is_not_error(module2);

  UtObject *type2a =
      ut_asn1_module_definition_get_assignment(module2, "TaggedType1");
  ut_assert_non_null_object(type2a);
  ut_assert_true(ut_object_is_asn1_tagged_type(type2a));
  ut_assert_true(ut_asn1_tagged_type_get_is_explicit(type2a));

  UtObject *type2b =
      ut_asn1_module_definition_get_assignment(module2, "TaggedType2");
  ut_assert_non_null_object(type2b);
  ut_assert_true(ut_object_is_asn1_tagged_type(type2b));
  ut_assert_true(ut_asn1_tagged_type_get_is_explicit(type2b));

  UtObject *type2c =
      ut_asn1_module_definition_get_assignment(module2, "TaggedType3");
  ut_assert_non_null_object(type2c);
  ut_assert_true(ut_object_is_asn1_tagged_type(type2c));
  ut_assert_false(ut_asn1_tagged_type_get_is_explicit(type2c));

  // Explicit/implicit tagging - default explicit.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS EXPLICIT TAGS ::= BEGIN\n"
      "    TaggedType1 ::= [1] INTEGER\n"
      "    TaggedType2 ::= [2] EXPLICIT INTEGER\n"
      "    TaggedType3 ::= [3] IMPLICIT INTEGER\n"
      "END");
  ut_assert_is_not_error(module3);

  UtObject *type3a =
      ut_asn1_module_definition_get_assignment(module3, "TaggedType1");
  ut_assert_non_null_object(type3a);
  ut_assert_true(ut_object_is_asn1_tagged_type(type3a));
  ut_assert_true(ut_asn1_tagged_type_get_is_explicit(type3a));

  UtObject *type3b =
      ut_asn1_module_definition_get_assignment(module3, "TaggedType2");
  ut_assert_non_null_object(type3b);
  ut_assert_true(ut_object_is_asn1_tagged_type(type3b));
  ut_assert_true(ut_asn1_tagged_type_get_is_explicit(type3b));

  UtObject *type3c =
      ut_asn1_module_definition_get_assignment(module3, "TaggedType3");
  ut_assert_non_null_object(type3c);
  ut_assert_true(ut_object_is_asn1_tagged_type(type3c));
  ut_assert_false(ut_asn1_tagged_type_get_is_explicit(type3c));

  // Explicit/implicit tagging - default implicit.
  UtObjectRef module4 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS IMPLICIT TAGS ::= BEGIN\n"
      "    TaggedType1 ::= [1] INTEGER\n"
      "    TaggedType2 ::= [2] EXPLICIT INTEGER\n"
      "    TaggedType3 ::= [3] IMPLICIT INTEGER\n"
      "END");
  ut_assert_is_not_error(module4);

  UtObject *type4a =
      ut_asn1_module_definition_get_assignment(module4, "TaggedType1");
  ut_assert_non_null_object(type4a);
  ut_assert_true(ut_object_is_asn1_tagged_type(type4a));
  ut_assert_false(ut_asn1_tagged_type_get_is_explicit(type4a));

  UtObject *type4b =
      ut_asn1_module_definition_get_assignment(module4, "TaggedType2");
  ut_assert_non_null_object(type4b);
  ut_assert_true(ut_object_is_asn1_tagged_type(type4b));
  ut_assert_true(ut_asn1_tagged_type_get_is_explicit(type4b));

  UtObject *type4c =
      ut_asn1_module_definition_get_assignment(module4, "TaggedType3");
  ut_assert_non_null_object(type4c);
  ut_assert_true(ut_object_is_asn1_tagged_type(type4c));
  ut_assert_false(ut_asn1_tagged_type_get_is_explicit(type4c));

  // Automatic tagging - default explicit.
  UtObjectRef module5 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS AUTOMATIC TAGS ::= BEGIN\n"
      "    TaggedType1 ::= [1] INTEGER\n"
      "    TaggedType2 ::= [2] EXPLICIT INTEGER\n"
      "    TaggedType3 ::= [3] IMPLICIT INTEGER\n"
      "END");
  ut_assert_is_not_error(module5);

  UtObject *type5a =
      ut_asn1_module_definition_get_assignment(module5, "TaggedType1");
  ut_assert_non_null_object(type5a);
  ut_assert_true(ut_object_is_asn1_tagged_type(type5a));
  ut_assert_true(ut_asn1_tagged_type_get_is_explicit(type5a));

  UtObject *type5b =
      ut_asn1_module_definition_get_assignment(module5, "TaggedType2");
  ut_assert_non_null_object(type5b);
  ut_assert_true(ut_object_is_asn1_tagged_type(type5b));
  ut_assert_true(ut_asn1_tagged_type_get_is_explicit(type5b));

  UtObject *type5c =
      ut_asn1_module_definition_get_assignment(module5, "TaggedType3");
  ut_assert_non_null_object(type5c);
  ut_assert_true(ut_object_is_asn1_tagged_type(type5c));
  ut_assert_false(ut_asn1_tagged_type_get_is_explicit(type5c));

  // FIXME: Constrained to single value.
}

static void test_tagged_value_assignment() {
  UtObjectRef module1 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    integerValue [1] INTEGER ::= 42\n"
      "END");
  ut_assert_is_not_error(module1);
  UtObject *assignment1 =
      ut_asn1_module_definition_get_assignment(module1, "integerValue");
  ut_assert_non_null_object(assignment1);
  UtObject *type1 = ut_asn1_type_value_get_type(assignment1);
  ut_assert_true(ut_object_is_asn1_tagged_type(type1));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_tagged_type_get_type(type1)));
  UtObject *value1 = ut_asn1_type_value_get_value(assignment1);
  ut_assert_true(ut_object_is_int64(value1));
  ut_assert_int_equal(ut_int64_get_value(value1), 42);

  // Referenced type.
  UtObjectRef module2 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    TaggedType ::= [1] INTEGER\n"
      "    integerValue TaggedType ::= 42\n"
      "END");
  ut_assert_is_not_error(module2);
  UtObject *assignment2 =
      ut_asn1_module_definition_get_assignment(module2, "integerValue");
  ut_assert_non_null_object(assignment2);
  UtObject *type2 = ut_asn1_type_value_get_type(assignment2);
  ut_assert_true(ut_object_is_asn1_referenced_type(type2));
  UtObject *type2i = ut_asn1_referenced_type_get_type(type2);
  ut_assert_true(ut_object_is_asn1_tagged_type(type2i));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_tagged_type_get_type(type2i)));
  UtObject *value2 = ut_asn1_type_value_get_value(assignment2);
  ut_assert_true(ut_object_is_int64(value2));
  ut_assert_int_equal(ut_int64_get_value(value2), 42);

  // Referenced value.
  UtObjectRef module3 = ut_asn1_module_definition_new_from_text(
      "Test DEFINITIONS ::= BEGIN\n"
      "    integerValue1 [1] INTEGER ::= 42\n"
      "    integerValue2 [1] INTEGER ::= integerValue1\n"
      "END");
  ut_assert_is_not_error(module3);
  UtObject *assignment3 =
      ut_asn1_module_definition_get_assignment(module3, "integerValue2");
  ut_assert_non_null_object(assignment3);
  UtObject *type3 = ut_asn1_type_value_get_type(assignment3);
  ut_assert_true(ut_object_is_asn1_tagged_type(type3));
  ut_assert_true(
      ut_object_is_asn1_integer_type(ut_asn1_tagged_type_get_type(type3)));
  UtObject *value3 = ut_asn1_type_value_get_value(assignment3);
  ut_assert_true(ut_object_is_int64(value3));
  ut_assert_int_equal(ut_int64_get_value(value3), 42);
}

int main(int argc, char **argv) {
  test_module_identifier();
  test_whitespace();
  test_module_definitions();
  test_comments();
  test_boolean_type_assignment();
  test_boolean_value_assignment();
  test_integer_type_assignment();
  test_integer_value_assignment();
  test_bit_string_type_assignment();
  test_bit_string_value_assignment();
  test_octet_string_type_assignment();
  test_octet_string_value_assignment();
  test_null_type_assignment();
  test_null_value_assignment();
  test_object_identifier_type_assignment();
  test_object_identifier_value_assignment();
  test_object_descriptor_type_assignment();
  test_object_descriptor_value_assignment();
  test_real_type_assignment();
  test_real_value_assignment();
  test_enumerated_type_assignment();
  test_enumerated_value_assignment();
  test_utf8_string_type_assignment();
  test_utf8_string_value_assignment();
  test_relative_oid_type_assignment();
  test_relative_oid_value_assignment();
  test_sequence_type_assignment();
  test_sequence_value_assignment();
  test_sequence_of_type_assignment();
  test_sequence_of_value_assignment();
  test_set_type_assignment();
  test_set_value_assignment();
  test_set_of_type_assignment();
  test_set_of_value_assignment();
  test_choice_type_assignment();
  test_choice_value_assignment();
  test_numeric_string_type_assignment();
  test_numeric_string_value_assignment();
  test_printable_string_type_assignment();
  test_printable_string_value_assignment();
  test_ia5_string_type_assignment();
  test_ia5_string_value_assignment();
  test_graphic_string_type_assignment();
  test_graphic_string_value_assignment();
  test_visible_string_type_assignment();
  test_visible_string_value_assignment();
  test_general_string_type_assignment();
  test_general_string_value_assignment();
  test_tagged_type_assignment();
  test_tagged_value_assignment();
}
