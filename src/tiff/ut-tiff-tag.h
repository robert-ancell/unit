#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef enum {
  UT_TIFF_TAG_NEW_SUBFILE_TYPE = 0x00fe,
  UT_TIFF_TAG_SUBFILE_TYPE = 0x00ff,
  UT_TIFF_TAG_IMAGE_WIDTH = 0x0100,
  UT_TIFF_TAG_IMAGE_LENGTH = 0x0101,
  UT_TIFF_TAG_BITS_PER_SAMPLE = 0x0102,
  UT_TIFF_TAG_COMPRESSION = 0x0103,
  UT_TIFF_TAG_PHOTOMETRIC_INTERPRETATION = 0x0106,
  UT_TIFF_TAG_THRESHOLDING = 0x0107,
  UT_TIFF_TAG_CELL_WIDTH = 0x0108,
  UT_TIFF_TAG_CELL_LENGTH = 0x0109,
  UT_TIFF_TAG_FILL_ORDER = 0x010a,
  UT_TIFF_TAG_DOCUMENT_NAME = 0x010d,
  UT_TIFF_TAG_IMAGE_DESCRIPTION = 0x010e,
  UT_TIFF_TAG_MAKE = 0x010f,
  UT_TIFF_TAG_MODEL = 0x0110,
  UT_TIFF_TAG_STRIP_OFFSETS = 0x0111,
  UT_TIFF_TAG_ORIENTATION = 0x0112,
  UT_TIFF_TAG_SAMPLES_PER_PIXEL = 0x0115,
  UT_TIFF_TAG_ROWS_PER_STRIP = 0x0116,
  UT_TIFF_TAG_STRIP_BYTE_COUNTS = 0x0117,
  UT_TIFF_TAG_MIN_SAMPLE_VALUE = 0x0118,
  UT_TIFF_TAG_MAX_SAMPLE_VALUE = 0x0119,
  UT_TIFF_TAG_X_RESOLUTION = 0x011a,
  UT_TIFF_TAG_Y_RESOLUTION = 0x011b,
  UT_TIFF_TAG_PLANAR_CONFIGURATION = 0x011c,
  UT_TIFF_TAG_PAGE_NAME = 0x011d,
  UT_TIFF_TAG_X_POSITION = 0x011e,
  UT_TIFF_TAG_Y_POSITION = 0x011f,
  UT_TIFF_TAG_FREE_OFFSETS = 0x0120,
  UT_TIFF_TAG_FREE_BYTE_COUNTS = 0x0121,
  UT_TIFF_TAG_GRAY_RESPONSE_UNIT = 0x0122,
  UT_TIFF_TAG_GRAY_RESPONSE_CURVE = 0x0123,
  UT_TIFF_TAG_T4_OPTIONS = 0x0124,
  UT_TIFF_TAG_T6_OPTIONS = 0x0125,
  UT_TIFF_TAG_RESOLUTION_UNIT = 0x0128,
  UT_TIFF_TAG_PAGE_NUMBER = 0x0129,
  UT_TIFF_TAG_TRANSFER_FUNCTION = 0x012d,
  UT_TIFF_TAG_SOFTWARE = 0x0131,
  UT_TIFF_TAG_DATE_TIME = 0x0132,
  UT_TIFF_TAG_ARTIST = 0x013b,
  UT_TIFF_TAG_HOST_COMPUTER = 0x013c,
  UT_TIFF_TAG_PREDICTOR = 0x013d,
  UT_TIFF_TAG_WHITE_POINT = 0x013e,
  UT_TIFF_TAG_PRIMARY_CHROMATICITIES = 0x013f,
  UT_TIFF_TAG_COLOR_MAP = 0x0140,
  UT_TIFF_TAG_HALFTONE_HINTS = 0x0141,
  UT_TIFF_TAG_TILE_WIDTH = 0x0142,
  UT_TIFF_TAG_TILE_LENGTH = 0x0143,
  UT_TIFF_TAG_TILE_OFFSETS = 0x0144,
  UT_TIFF_TAG_TILE_BYTE_COUNTS = 0x0145,
  UT_TIFF_TAG_INK_SET = 0x014c,
  UT_TIFF_TAG_INK_NAMES = 0x014d,
  UT_TIFF_TAG_NUMBER_OF_INKS = 0x014e,
  UT_TIFF_TAG_DOT_RANGE = 0x0150,
  UT_TIFF_TAG_TARGET_PRINTER = 0x0151,
  UT_TIFF_TAG_EXTRA_SAMPLES = 0x0152,
  UT_TIFF_TAG_SAMPLE_FORMAT = 0x0153,
  UT_TIFF_TAG_S_MIN_SAMPLE_VALUE = 0x0154,
  UT_TIFF_TAG_S_MAX_SAMPLE_VALUE = 0x0155,
  UT_TIFF_TAG_TRANSFER_RANGE = 0x0156,
  UT_TIFF_TAG_JPEG_PROC = 0x0200,
  UT_TIFF_TAG_JPEG_INTERCHANGE_FORMAT = 0x0201,
  UT_TIFF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH = 0x0202,
  UT_TIFF_TAG_JPEG_RESTART_INTERVAL = 0x0203,
  UT_TIFF_TAG_JPEG_LOSSLESS_PREDICTORS = 0x0205,
  UT_TIFF_TAG_JPEG_POINT_TRANSFORMS = 0x0206,
  UT_TIFF_TAG_JPEG_Q_TABLES = 0x0207,
  UT_TIFF_TAG_JPEG_DC_TABLES = 0x0208,
  UT_TIFF_TAG_JPEG_AC_TABLES = 0x0209,
  UT_TIFF_TAG_JPEG_YCBCR_COEFFICIENTS = 0x0211,
  UT_TIFF_TAG_JPEG_YCBCR_SUBSAMPLING = 0x0212,
  UT_TIFF_TAG_JPEG_YCBCR_POSITIONING = 0x0213,
  UT_TIFF_TAG_REFERENCE_BLACK_WHITE = 0x0214,
  UT_TIFF_TAG_COPYRIGHT = 0x8298,
  UT_TIFF_TAG_EXIF_IFD = 0x8769,
  UT_TIFF_TAG_GPS_IFD = 0x8825
} UtTiffTagId;

typedef enum {
  UT_TIFF_TAG_TYPE_BYTE = 1,
  UT_TIFF_TAG_TYPE_ASCII = 2,
  UT_TIFF_TAG_TYPE_SHORT = 3,
  UT_TIFF_TAG_TYPE_LONG = 4,
  UT_TIFF_TAG_TYPE_RATIONAL = 5,
  UT_TIFF_TAG_TYPE_SBYTE = 6,
  UT_TIFF_TAG_TYPE_UNDEFINED = 7,
  UT_TIFF_TAG_TYPE_SSHORT = 8,
  UT_TIFF_TAG_TYPE_SLONG = 9,
  UT_TIFF_TAG_TYPE_SRATIONAL = 10,
  UT_TIFF_TAG_TYPE_FLOAT = 11,
  UT_TIFF_TAG_TYPE_DOUBLE = 12
} UtTiffTagType;

typedef enum {
  UT_TIFF_COMPRESSION_UNCOMPRESSED = 1,
  UT_TIFF_COMPRESSION_CCITT_1D = 2,
  UT_TIFF_COMPRESSION_GROUP_3_FAX = 3,
  UT_TIFF_COMPRESSION_GROUP_4_FAX = 4,
  UT_TIFF_COMPRESSION_LZW = 5,
  UT_TIFF_COMPRESSION_JPEG = 7,
  UT_TIFF_COMPRESSION_DEFLATE = 8,
  UT_TIFF_COMPRESSION_PACK_BITS = 32773
} UtTiffCompression;

typedef enum {
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_WHITE_IS_ZERO = 0,
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_BLACK_IS_ZERO = 1,
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB = 2,
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB_PALETTE = 3,
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_TRANSPARENCY_MASK = 4,
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_CMYK = 5,
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_YCBCR = 6,
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_CIELAB = 8
} UtTiffPhotometricInterpretation;

typedef enum {
  UT_TIFF_PLANAR_CONFIGURATION_CHUNKY = 1,
  UT_TIFF_PLANAR_CONFIGURATION_PLANAR = 2
} UtTiffPlanarConfiguration;

typedef enum {
  UT_TIFF_RESOLUTION_UNIT_NONE = 1,
  UT_TIFF_RESOLUTION_UNIT_INCH = 2,
  UT_TIFF_RESOLUTION_UNIT_CENTIMETER = 3
} UtTiffResolutionUnit;

typedef enum {
  UT_TIFF_PREDICTOR_NONE = 1,
  UT_TIFF_PREDICTOR_HORIZONTAL_DIFFERENCING = 2
} UtTiffPredictor;

/// Creates a new TIFF tag of [type] and containing raw [data].
///
/// !arg-type data UtUint8List
/// !return-type UtTiffTag
/// !return-ref
UtObject *ut_tiff_tag_new(uint16_t id, uint16_t type, UtObject *data);

/// Creates a new TIFF tag containing a single byte [value].
///
/// !return-type UtTiffTag
/// !return-ref
UtObject *ut_tiff_tag_new_single_byte(uint16_t id, uint8_t value);

/// Creates a new TIFF tag containing multiple byte [value]s.
///
/// !arg-type value UtUint8List
/// !return-type UtTiffTag
/// !return-ref
UtObject *ut_tiff_tag_new_byte(uint16_t id, UtObject *value);

/// Creates a new TIFF tag containing an ASCII string [value].
///
/// !return-type UtTiffTag
/// !return-ref
UtObject *ut_tiff_tag_new_ascii(uint16_t id, const char *value);

/// Returns the ID of this tag.
/// Use [UtTiffTagId] for standard IDs.
uint16_t ut_tiff_tag_get_id(UtObject *object);

/// Returns the type of tag this is.
/// Use [UtTiffTagType] for standard tag types.
uint16_t ut_tiff_tag_get_type(UtObject *object);

/// Returns the number of values in this tag.
size_t ut_tiff_tag_get_count(UtObject *object);

/// Returns the data contained in this tag.
///
/// !return-type UtUint8List
UtObject *ut_tiff_tag_get_data(UtObject *object);

/// Returns the byte at position [index] in this tag.
/// Returns 0 if this tag is not a byte tag.
uint8_t ut_tiff_tag_get_byte(UtObject *object, size_t index);

/// Returns the text inside this tag.
/// Returns "" if this tag is not an ASCII tag.
char *ut_tiff_tag_get_ascii(UtObject *object);

/// Returns the short at position [index] in this tag.
/// Returns 0 if this tag is not a short tag.
uint16_t ut_tiff_tag_get_short(UtObject *object, size_t index);

/// Returns the long at position [index] in this tag.
/// Returns 0 if this tag is not a long tag.
uint32_t ut_tiff_tag_get_long(UtObject *object, size_t index);

/// Returns the short or long at position [index] in this tag.
/// Returns 0 if this tag is not a short or long tag.
uint32_t ut_tiff_tag_get_short_or_long(UtObject *object, size_t index);

/// Returns [true] if [object] is a [UtTiffTag].
bool ut_object_is_tiff_tag(UtObject *object);
