#pragma once

#include "ut-object.h"

UtObject *ut_asn1_encode_numeric_string(const char *string);

UtObject *ut_asn1_decode_numeric_string(UtObject *numeric_string);

UtObject *ut_asn1_encode_printable_string(const char *string);

UtObject *ut_asn1_decode_printable_string(UtObject *printable_string);

UtObject *ut_asn1_encode_ia5_string(UtObject *string);

UtObject *ut_asn1_decode_ia5_string(UtObject *ia5_string);

UtObject *ut_asn1_encode_visible_string(const char *string);

UtObject *ut_asn1_decode_visible_string(UtObject *visible_string);
