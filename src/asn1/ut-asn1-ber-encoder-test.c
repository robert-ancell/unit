#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      encoder, UT_ASN1_TAG_CLASS_UNIVERSAL, UT_ASN1_TAG_UNIVERSAL_BOOLEAN);
  ut_asn1_ber_encoder_encode_boolean(encoder, true);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder),
                                 "0101ff");

  return 0;
}
