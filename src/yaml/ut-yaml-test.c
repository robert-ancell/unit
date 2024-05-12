#include <stdio.h>
#include <stdlib.h>

#include "ut.h"

static void test_decode(const char *yaml, UtObject *expected) {
  UtObjectRef object = ut_yaml_decode(yaml);
  ut_assert_equal(object, expected);
  ut_object_unref(expected);
}

#if 0
static void test_decode_error(const char *yaml) {
}
#endif

int main(int argc, char **argv) {
  // Empty documents
  test_decode("", ut_list_new_from_elements(ut_string_new(""), NULL));
  test_decode(" ", ut_list_new_from_elements_take(ut_string_new(""), NULL));
  test_decode("  ", ut_list_new_from_elements_take(ut_string_new(""), NULL));
  test_decode("\n", ut_list_new_from_elements_take(ut_string_new(""), NULL));

  // Whitespace
  test_decode("foo",
              ut_list_new_from_elements_take(ut_string_new("foo"), NULL));
  test_decode("foo\n",
              ut_list_new_from_elements_take(ut_string_new("foo"), NULL));
  test_decode(" foo",
              ut_list_new_from_elements_take(ut_string_new("foo"), NULL));
  test_decode("foo ",
              ut_list_new_from_elements_take(ut_string_new("foo"), NULL));
  test_decode(" foo ",
              ut_list_new_from_elements_take(ut_string_new("foo"), NULL));
  test_decode("foo bar",
              ut_list_new_from_elements_take(ut_string_new("foo bar"), NULL));
  test_decode("\nfoo",
              ut_list_new_from_elements_take(ut_string_new("foo"), NULL));
  test_decode("\n\nfoo",
              ut_list_new_from_elements_take(ut_string_new("foo"), NULL));
  test_decode("\nfoo\n",
              ut_list_new_from_elements_take(ut_string_new("foo"), NULL));
  test_decode("foo  bar",
              ut_list_new_from_elements_take(ut_string_new("foo  bar"), NULL));
  // test_decode("foo\nbar",
  //             ut_list_new_from_elements_take(ut_string_new("foo bar"),
  //             NULL));

  // Quotes
  test_decode("\"foo\"",
              ut_list_new_from_elements_take(ut_string_new("foo"), NULL));
  test_decode("\"f'oo\"",
              ut_list_new_from_elements_take(ut_string_new("f'oo"), NULL));
  test_decode("\"f\\\"oo\"",
              ut_list_new_from_elements_take(ut_string_new("f\"oo"), NULL));
  test_decode("\"\\\"foo\\\"\"",
              ut_list_new_from_elements_take(ut_string_new("\"foo\""), NULL));
  test_decode("'foo'",
              ut_list_new_from_elements_take(ut_string_new("foo"), NULL));
  test_decode("'f''oo'",
              ut_list_new_from_elements_take(ut_string_new("f'oo"), NULL));
  test_decode("'''foo'''",
              ut_list_new_from_elements_take(ut_string_new("'foo'"), NULL));
#if 0
  test_decode_error("\"foo");
  test_decode_error("foo\"");
  test_decode_error("f\"oo");
  test_decode_error("\"f\"oo\"");
  test_decode_error("'foo");
  test_decode_error("foo'");
  test_decode_error("f'oo");
  test_decode_error("'f'oo'");
#endif

  // Comments
  test_decode("# Comment",
              ut_list_new_from_elements_take(ut_string_new(""), NULL));
  test_decode(" # Comment",
              ut_list_new_from_elements_take(ut_string_new(""), NULL));
  test_decode(" #Comment",
              ut_list_new_from_elements_take(ut_string_new(""), NULL));
  test_decode("foo # Comment",
              ut_list_new_from_elements_take(ut_string_new("foo"), NULL));

  // Sequences
  test_decode("- one\n"
              "- two\n"
              "- three",
              ut_list_new_from_elements_take(
                  ut_list_new_from_elements_take(ut_string_new("one"),
                                                 ut_string_new("two"),
                                                 ut_string_new("three"), NULL),
                  NULL));

  // Maps
  test_decode("one: 1", ut_list_new_from_elements_take(
                            ut_map_new_string_from_elements_take(
                                "one", ut_string_new("1"), NULL),
                            NULL));
  test_decode("one: 1\n"
              "two: 2\n"
              "three: 3",
              ut_list_new_from_elements_take(
                  ut_map_new_string_from_elements_take(
                      "one", ut_string_new("1"), "two", ut_string_new("2"),
                      "three", ut_string_new("3"), NULL),
                  NULL));
  test_decode("name:\nfoo", ut_list_new_from_elements_take(
                                ut_map_new_string_from_elements_take(
                                    "name", ut_string_new("foo"), NULL),
                                NULL));

  // Map of sequences
  test_decode("english:\n"
              "- one\n"
              "- two\n"
              "- three\n"
              "german:\n"
              "- eins\n"
              "- zwei\n"
              "- drei",
              ut_list_new_from_elements_take(
                  ut_map_new_string_from_elements_take(
                      "english",
                      ut_list_new_from_elements_take(
                          ut_string_new("one"), ut_string_new("two"),
                          ut_string_new("three"), NULL),
                      "german",
                      ut_list_new_from_elements_take(
                          ut_string_new("eins"), ut_string_new("zwei"),
                          ut_string_new("drei"), NULL),
                      NULL),
                  NULL));
#if 0
  test_decode_error("english: - one\n"
		    "         - two\n"
		    "         - three");
#endif

  // Sequence of maps
  test_decode(
      "- name:  one\n"
      "  value: 1\n"
      "- name:  two\n"
      "  value: 2",
      ut_list_new_from_elements_take(
          ut_list_new_from_elements_take(ut_map_new_string_from_elements_take(
                                             "name", ut_string_new("one"),
                                             "value", ut_string_new("1"), NULL),
                                         ut_map_new_string_from_elements_take(
                                             "name", ut_string_new("two"),
                                             "value", ut_string_new("2"), NULL),
                                         NULL),
          NULL));
}
