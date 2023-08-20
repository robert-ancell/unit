#include <stdio.h>
#include <stdlib.h>

#include "ut.h"

static void test_encode() {
  UtObjectRef empty_element_root = ut_xml_element_new("tag", NULL, NULL);
  UtObjectRef empty_element_document = ut_xml_document_new(empty_element_root);
  ut_cstring_ref empty_element_text =
      ut_xml_document_to_text(empty_element_document);
  ut_assert_cstring_equal(empty_element_text, "<tag/>");

  UtObjectRef attribute_attributes = ut_map_new_string_from_elements_take(
      "name", ut_string_new("value"), NULL);
  UtObjectRef attribute_root =
      ut_xml_element_new("tag", attribute_attributes, NULL);
  UtObjectRef attribute_document = ut_xml_document_new(attribute_root);
  ut_cstring_ref attribute_text = ut_xml_document_to_text(attribute_document);
  ut_assert_cstring_equal(attribute_text, "<tag name=\"value\"/>");

  UtObjectRef multiple_attribute_attributes =
      ut_map_new_string_from_elements_take("name1", ut_string_new("'value1'"),
                                           "name2", ut_string_new("\"value2\""),
                                           NULL);
  UtObjectRef multiple_attribute_root =
      ut_xml_element_new("tag", multiple_attribute_attributes, NULL);
  UtObjectRef multiple_attribute_document =
      ut_xml_document_new(multiple_attribute_root);
  ut_cstring_ref multiple_attribute_text =
      ut_xml_document_to_text(multiple_attribute_document);
  ut_assert_cstring_equal(multiple_attribute_text,
                          "<tag name1=\"'value1'\" name2='\"value2\"'/>");

  UtObjectRef content_content = ut_list_new();
  ut_list_append_take(content_content, ut_string_new("Hello World!"));
  UtObjectRef content_root = ut_xml_element_new("tag", NULL, content_content);
  UtObjectRef content_document = ut_xml_document_new(content_root);
  ut_cstring_ref content_text = ut_xml_document_to_text(content_document);
  ut_assert_cstring_equal(content_text, "<tag>Hello World!</tag>");

  UtObjectRef escaped_content = ut_list_new();
  ut_list_append_take(escaped_content,
                      ut_string_new("<\"Fast\" & 'Efficient'>"));
  UtObjectRef escaped_root = ut_xml_element_new("tag", NULL, escaped_content);
  UtObjectRef escaped_document = ut_xml_document_new(escaped_root);
  ut_cstring_ref escaped_text = ut_xml_document_to_text(escaped_document);
  ut_assert_cstring_equal(escaped_text,
                          "<tag>&lt;\"Fast\" &amp; 'Efficient'&gt;</tag>");
}

static void test_decode() {
  UtObjectRef empty_document = ut_xml_document_new_from_text("");
  ut_assert_null_object(empty_document);

  UtObjectRef empty_element_document = ut_xml_document_new_from_text("<tag/>");
  ut_assert_non_null_object(empty_element_document);
  UtObject *root = ut_xml_document_get_root(empty_element_document);
  ut_assert_cstring_equal(ut_xml_element_get_name(root), "tag");
  ut_assert_null_object(ut_xml_element_get_content(root));

  UtObjectRef tag_name_leading_whitespace_document =
      ut_xml_document_new_from_text("< tag/>");
  ut_assert_null_object(tag_name_leading_whitespace_document);

  UtObjectRef tag_name_trailing_whitespace_document =
      ut_xml_document_new_from_text("<tag />");
  ut_assert_non_null_object(tag_name_trailing_whitespace_document);
  root = ut_xml_document_get_root(tag_name_trailing_whitespace_document);
  ut_assert_cstring_equal(ut_xml_element_get_name(root), "tag");
  ut_assert_null_object(ut_xml_element_get_content(root));

  UtObjectRef attribute_document =
      ut_xml_document_new_from_text("<tag name=\"value\"/>");
  ut_assert_non_null_object(attribute_document);

  UtObjectRef multiple_attribute_document = ut_xml_document_new_from_text(
      "<foo name1=\"'value1'\" name2='\"value2\"'/>");
  ut_assert_non_null_object(multiple_attribute_document);

  UtObjectRef escaped_attribute_document =
      ut_xml_document_new_from_text("<tag name=\"&quot;value&quot;\"/>");
  ut_assert_non_null_object(escaped_attribute_document);

  UtObjectRef no_content_document =
      ut_xml_document_new_from_text("<tag></tag>");
  ut_assert_non_null_object(no_content_document);
  root = ut_xml_document_get_root(no_content_document);
  ut_assert_cstring_equal(ut_xml_element_get_name(root), "tag");
  ut_assert_null_object(ut_xml_element_get_content(root));

  UtObjectRef content_document =
      ut_xml_document_new_from_text("<tag>Hello World!</tag>");
  ut_assert_non_null_object(content_document);
  root = ut_xml_document_get_root(content_document);
  ut_assert_cstring_equal(ut_xml_element_get_name(root), "tag");
  UtObject *content = ut_xml_element_get_content(root);
  ut_assert_non_null_object(content);
  ut_assert_int_equal(ut_list_get_length(content), 1);
  UtObjectRef content_document_content = ut_list_get_element(content, 0);
  ut_assert_cstring_equal(ut_string_get_text(content_document_content),
                          "Hello World!");

  UtObjectRef escaped_document = ut_xml_document_new_from_text(
      "<tag>&lt;&quot;Fast&quot; &amp; &apos;Efficient&apos;&gt;</tag>");
  ut_assert_non_null_object(escaped_document);
  root = ut_xml_document_get_root(escaped_document);
  ut_assert_cstring_equal(ut_xml_element_get_name(root), "tag");
  ut_assert_non_null_object(ut_xml_element_get_content(root));
  content = ut_xml_element_get_content(root);
  ut_assert_non_null_object(content);
  ut_assert_int_equal(ut_list_get_length(content), 1);
  UtObjectRef escaped_document_content = ut_list_get_element(content, 0);
  ut_assert_cstring_equal(ut_string_get_text(escaped_document_content),
                          "<\"Fast\" & 'Efficient'>");

  UtObjectRef mismatched_tags_document =
      ut_xml_document_new_from_text("<foo></bar>");
  ut_assert_null_object(mismatched_tags_document);

  UtObjectRef interleaved_tags_document =
      ut_xml_document_new_from_text("<foo><bar></foo></bar>");
  ut_assert_null_object(interleaved_tags_document);
}

int main(int argc, char **argv) {
  test_encode();
  test_decode();
}
