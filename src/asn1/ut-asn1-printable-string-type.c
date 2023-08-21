#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
} UtAsn1PrintableStringType;

static bool valid_printable_character(char c) {
  return (c >= '0' && c <= '9') || c == ' ';
}

static UtObject *ut_asn1_printable_string_type_get_tags(UtObject *object) {
  return ut_list_new_from_elements_take(
      ut_asn1_tag_new_universal(UT_ASN1_TAG_UNIVERSAL_PRINTABLE_STRING), NULL);
}

static bool ut_asn1_printable_string_type_matches_tag(UtObject *object,
                                                      UtObject *tag) {
  return ut_asn1_tag_matches(tag, UT_ASN1_TAG_CLASS_UNIVERSAL,
                             UT_ASN1_TAG_UNIVERSAL_PRINTABLE_STRING);
}

static UtObject *ut_asn1_printable_string_type_check_value(UtObject *object,
                                                           UtObject *value) {
  if (!ut_object_implements_string(value)) {
    return ut_asn1_error_new("Invalid type for PrintableString");
  }

  const char *text = ut_string_get_text(value);
  for (const char *c = text; *c != '\0'; c++) {
    if (!valid_printable_character(*c)) {
      return ut_asn1_error_new("Invalid character in PrintableString");
    }
  }

  return NULL;
}

static UtAsn1TypeInterface asn1_type_interface = {
    .get_tags = ut_asn1_printable_string_type_get_tags,
    .matches_tag = ut_asn1_printable_string_type_matches_tag,
    .check_value = ut_asn1_printable_string_type_check_value};

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1PrintableStringType",
    .interfaces = {{&ut_asn1_type_id, &asn1_type_interface}, {NULL, NULL}}};

UtObject *ut_asn1_printable_string_type_new() {
  return ut_object_new(sizeof(UtAsn1PrintableStringType), &object_interface);
}

bool ut_object_is_asn1_printable_string_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
