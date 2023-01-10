#include <assert.h>

#include "ut.h"

//      "60818561101a044a6f686e1a01501a05536d697468a00a1a084469726563746f72420133"
//      "a10a43083139373130393137a21261101a044d6172791a01541a05536d697468");

int main(int argc, char **argv) {
  // Boolean value.
  UtObjectRef boolean_data = ut_uint8_list_new_from_hex_string("0101ff");
  UtObjectRef boolean_decoder = ut_asn1_ber_decoder_new(boolean_data);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_class(boolean_decoder),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  assert(!ut_asn1_ber_decoder_get_constructed(boolean_decoder));
  ut_assert_int_equal(ut_asn1_ber_decoder_get_number(boolean_decoder),
                      UT_ASN1_TAG_UNIVERSAL_BOOLEAN);
  assert(ut_asn1_ber_decoder_decode_boolean(boolean_decoder));

  // Any non-zero value is true.
  UtObjectRef boolean_data2 = ut_uint8_list_new_from_hex_string("010101");
  UtObjectRef boolean_decoder2 = ut_asn1_ber_decoder_new(boolean_data);
  assert(ut_asn1_ber_decoder_decode_boolean(boolean_decoder2));

  // False is zero.
  UtObjectRef boolean_data3 = ut_uint8_list_new_from_hex_string("010100");
  UtObjectRef boolean_decoder3 = ut_asn1_ber_decoder_new(boolean_data);
  assert(!ut_asn1_ber_decoder_decode_boolean(boolean_decoder3));

  // FIXME: Zero length boolean
  // FIXME: >1 length boolean
  // FIXME: Constructed boolean

  return 0;
}
