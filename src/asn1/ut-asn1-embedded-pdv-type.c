#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
} UtAsn1EmbeddedPdvType;

static UtObject *ut_asn1_embedded_pdv_type_get_tags(UtObject *object) {
  return ut_list_new_from_elements_take(
      ut_asn1_tag_new_universal(UT_ASN1_TAG_UNIVERSAL_EMBEDDED_PDV), NULL);
}

static bool ut_asn1_embedded_pdv_type_matches_tag(UtObject *object,
                                                  UtObject *tag) {
  return ut_asn1_tag_matches(tag, UT_ASN1_TAG_CLASS_UNIVERSAL,
                             UT_ASN1_TAG_UNIVERSAL_EMBEDDED_PDV);
}

static UtAsn1TypeInterface asn1_type_interface = {
    .get_tags = ut_asn1_embedded_pdv_type_get_tags,
    .matches_tag = ut_asn1_embedded_pdv_type_matches_tag};

static bool ut_asn1_embedded_pdv_type_equal(UtObject *object, UtObject *other) {
  return ut_object_is_asn1_embedded_pdv_type(other);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1EmbeddedPdvType",
    .equal = ut_asn1_embedded_pdv_type_equal,
    .interfaces = {{&ut_asn1_type_id, &asn1_type_interface}, {NULL, NULL}}};

UtObject *ut_asn1_embedded_pdv_type_new() {
  return ut_object_new(sizeof(UtAsn1EmbeddedPdvType), &object_interface);
}

UtObject *ut_asn1_embedded_pdv_type_get_associated_type(UtObject *object) {
  assert(ut_object_is_asn1_embedded_pdv_type(object));
  // EMBEDDED PDV is the following type (with automatic tagging).
  //
  // SEQUENCE {
  //   identification CHOICE {
  //     syntaxes SEQUENCE {
  //       abstract OBJECT IDENTIFIER,
  //       transfer OBJECT IDENTIFIER
  //     },
  //     syntax OBJECT IDENTIFIER,
  //     presentation-context-id INTEGER,
  //     context-negotiation SEQUENCE {
  //       presentation-context-id INTEGER,
  //       transfer-syntax OBJECT IDENTIFIER
  //     },
  //     transfer-syntax OBJECT IDENTIFIER,
  //     fixed NULL
  //   },
  //   data-value-descriptor ObjectDescriptor OPTIONAL,
  //   data-value OCTET STRING
  // }
  return ut_asn1_sequence_type_new_take(
      ut_map_new_string_from_elements_take(
          "identification",
          ut_asn1_tagged_type_new_take(
              UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 0, true,
              ut_asn1_choice_type_new_take(
                  ut_map_new_string_from_elements_take(
                      "syntaxes",
                      ut_asn1_tagged_type_new_take(
                          UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 0, true,
                          ut_asn1_sequence_type_new_take(
                              ut_map_new_string_from_elements_take(
                                  "abstract",
                                  ut_asn1_object_identifier_type_new(),
                                  "transfer",
                                  ut_asn1_object_identifier_type_new(), NULL),
                              false)),
                      "syntax",
                      ut_asn1_tagged_type_new_take(
                          UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 1, true,
                          ut_asn1_object_identifier_type_new()),
                      "presentation-context-id",
                      ut_asn1_tagged_type_new_take(
                          UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 2, true,
                          ut_asn1_integer_type_new()),
                      "context-negotiation",
                      ut_asn1_tagged_type_new_take(
                          UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 3, true,
                          ut_asn1_sequence_type_new_take(
                              ut_map_new_string_from_elements_take(
                                  "presentation-context-id",
                                  ut_asn1_integer_type_new(), "transfer-syntax",
                                  ut_asn1_object_identifier_type_new(), NULL),
                              false)),
                      "transfer-syntax",
                      ut_asn1_tagged_type_new_take(
                          UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 4, true,
                          ut_asn1_object_identifier_type_new()),
                      "fixed",
                      ut_asn1_tagged_type_new_take(
                          UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 5, true,
                          ut_asn1_null_type_new()),
                      NULL),
                  false)),
          "data-value-descriptor",
          ut_asn1_optional_type_new_take(ut_asn1_tagged_type_new_take(
              UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 1, true,
              ut_asn1_object_descriptor_type_new())),
          "data-value",
          ut_asn1_tagged_type_new_take(UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 2,
                                       true, ut_asn1_octet_string_type_new()),
          NULL),
      false);
}

UtObject *ut_asn1_embedded_pdv_type_encode_value(UtObject *object,
                                                 UtObject *value) {
  assert(ut_object_is_asn1_embedded_pdv_type(object));
  assert(ut_object_is_asn1_embedded_value(value));

  UtObjectRef encoded_value = ut_map_new();
  UtObject *identification = ut_asn1_embedded_value_get_identification(value);
  const char *identification_id;
  UtObjectRef identification_value = NULL;
  if (ut_object_is_asn1_embedded_identification_syntaxes(identification)) {
    identification_id = "syntaxes";
    identification_value = ut_map_new_string_from_elements(
        "abstract",
        ut_asn1_embedded_identification_syntaxes_get_abstract(identification),
        "transfer",
        ut_asn1_embedded_identification_syntaxes_get_transfer(identification),
        NULL);
  } else if (ut_object_is_asn1_embedded_identification_syntax(identification)) {
    identification_id = "syntax";
    identification_value = ut_object_ref(
        ut_asn1_embedded_identification_syntax_get_identifier(identification));
  } else if (ut_object_is_asn1_embedded_identification_presentation_context_id(
                 identification)) {
    identification_id = "presentation-context-id";
    identification_value = ut_int64_new(
        ut_asn1_embedded_identification_presentation_context_id_get_value(
            identification));
  } else if (ut_object_is_asn1_embedded_identification_context_negotiation(
                 identification)) {
    identification_id = "context-negotiation";
    identification_value = ut_map_new_string_from_elements_take(
        "presentation-context-id",
        ut_int64_new(
            ut_asn1_embedded_identification_context_negotiation_get_presentation_context_id(
                identification)),
        "transfer-syntax",
        ut_object_ref(
            ut_asn1_embedded_identification_context_negotiation_get_transfer_syntax(
                identification)),
        NULL);
  } else if (ut_object_is_asn1_embedded_identification_transfer_syntax(
                 identification)) {
    identification_id = "transfer-syntax";
    identification_value = ut_object_ref(
        ut_asn1_embedded_identification_transfer_syntax_get_identifier(
            identification));
  } else if (ut_object_is_asn1_embedded_identification_fixed(identification)) {
    identification_id = "fixed";
    identification_value = ut_null_new();
  } else {
    assert(false);
  }
  ut_map_insert_string_take(
      encoded_value, "identification",
      ut_asn1_choice_value_new(identification_id, identification_value));
  const char *data_value_descriptor =
      ut_asn1_embedded_value_get_data_value_descriptor(value);
  if (data_value_descriptor != NULL) {
    ut_map_insert_string_take(encoded_value, "data-value-descriptor",
                              ut_string_new(data_value_descriptor));
  }
  ut_map_insert_string(encoded_value, "data-value",
                       ut_asn1_embedded_value_get_data_value(value));
  return ut_object_ref(encoded_value);
}

UtObject *ut_asn1_embedded_pdv_type_decode_value(UtObject *object,
                                                 UtObject *value) {
  assert(ut_object_is_asn1_embedded_pdv_type(object));
  assert(ut_object_implements_map(value));

  UtObject *identification_choice_value =
      ut_map_lookup_string(value, "identification");
  assert(identification_choice_value != NULL);
  const char *identification_id =
      ut_asn1_choice_value_get_identifier(identification_choice_value);
  UtObject *identification_value =
      ut_asn1_choice_value_get_value(identification_choice_value);
  UtObjectRef identification = NULL;
  if (ut_cstring_equal(identification_id, "syntaxes")) {
    identification = ut_asn1_embedded_identification_syntaxes_new(
        ut_map_lookup_string(identification_value, "abstract"),
        ut_map_lookup_string(identification_value, "transfer"));
  } else if (ut_cstring_equal(identification_id, "syntax")) {
    identification =
        ut_asn1_embedded_identification_syntax_new(identification_value);
  } else if (ut_cstring_equal(identification_id, "presentation-context-id")) {
    identification =
        ut_asn1_embedded_identification_presentation_context_id_new(
            ut_int64_get_value(identification_value));
  } else if (ut_cstring_equal(identification_id, "context-negotiation")) {
    identification = ut_asn1_embedded_identification_context_negotiation_new(
        ut_int64_get_value(ut_map_lookup_string(identification_value,
                                                "presentation-context-id")),
        ut_map_lookup_string(identification_value, "transfer-syntax"));
  } else if (ut_cstring_equal(identification_id, "transfer-syntax")) {
    identification = ut_asn1_embedded_identification_transfer_syntax_new(
        identification_value);
  } else if (ut_cstring_equal(identification_id, "fixed")) {
    identification = ut_asn1_embedded_identification_fixed_new();
  } else {
    assert(false);
  }
  UtObject *descriptor_value =
      ut_map_lookup_string(value, "data-value-descriptor");
  UtObject *data_value = ut_map_lookup_string(value, "data-value");
  assert(data_value != NULL);

  return ut_asn1_embedded_value_new(
      identification,
      descriptor_value != NULL ? ut_string_get_text(descriptor_value) : NULL,
      data_value);
}

bool ut_object_is_asn1_embedded_pdv_type(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
