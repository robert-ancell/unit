#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// TIFF tags:
/// - [UT_TIFF_TAG_NEW_SUBFILE_TYPE] - A general indication of the kind of data
/// contained in this subfile.
/// - [UT_TIFF_TAG_SUBFILE_TYPE] - A general indication of the kind of data
/// contained in this subfile. This field is deprecated. The
/// [UT_TIFF_TAG_NEW_SUBFILE_TYPE] field should be used instead.
/// - [UT_TIFF_TAG_IMAGE_WIDTH] - The number of columns in the image, i.e., the
/// number of pixels per row.
/// - [UT_TIFF_TAG_IMAGE_LENGTH] - The number of rows of pixels in the image.
/// - [UT_TIFF_TAG_BITS_PER_SAMPLE] - Number of bits per component.
/// - [UT_TIFF_TAG_COMPRESSION] - Compression scheme used on the image data.
/// - [UT_TIFF_TAG_PHOTOMETRIC_INTERPRETATION] - The color space of the image
/// data.
/// - [UT_TIFF_TAG_THRESHOLDING] - For black and white TIFF files that represent
/// shades of gray, the technique used to convert from gray to black and white
/// pixels
/// - [UT_TIFF_TAG_CELL_WIDTH] - The width of the dithering or halftoning matrix
/// used to create a dithered or halftoned bilevel file.
/// - [UT_TIFF_TAG_CELL_LENGTH] - The length of the dithering or halftoning
/// matrix used to create a dithered or halftoned bilevel file.
/// - [UT_TIFF_TAG_FILL_ORDER] - The logical order of bits within a byte.
/// - [UT_TIFF_TAG_DOCUMENT_NAME] - The name of the document from which this
/// image was scanned..
/// - [UT_TIFF_TAG_IMAGE_DESCRIPTION] - A string that describes the subject of
/// the image.
/// - [UT_TIFF_TAG_MAKE] - The manufacturer of the device that produced the
/// image.
/// - [UT_TIFF_TAG_MODEL] - The model name or number of the device that produced
/// the image.
/// - [UT_TIFF_TAG_STRIP_OFFSETS] - For each strip, the byte offset of that
/// strip.
/// - [UT_TIFF_TAG_ORIENTATION] - The orientation of the image with respect to
/// the rows and columns.
/// - [UT_TIFF_TAG_SAMPLES_PER_PIXEL] - The number of components per pixel.
/// - [UT_TIFF_TAG_ROWS_PER_STRIP] - The number of rows in each strip.
/// - [UT_TIFF_TAG_STRIP_BYTE_COUNTS] - For each strip, the number of bytes in
/// that strip after any compression.
/// - [UT_TIFF_TAG_MIN_SAMPLE_VALUE] - The minimum component value used.
/// - [UT_TIFF_TAG_MAX_SAMPLE_VALUE] - The maximum component value used.
/// - [UT_TIFF_TAG_X_RESOLUTION] - The number of pixels per
/// [UT_TIFF_TAG_RESOLUTION_UNIT] in the [UT_TIFF_TAG_IMAGE_WIDTH].
/// - [UT_TIFF_TAG_Y_RESOLUTION] - The number of pixels per
/// [UT_TIFF_TAG_RESOLUTION_UNIT] in the [UT_TIFF_TAG_IMAGE_LENGTH].
/// - [UT_TIFF_TAG_PLANAR_CONFIGURATION] - How the components of each pixel are
/// stored.
/// - [UT_TIFF_TAG_PAGE_NAME] - The name of the page from which this image was
/// scanned.
/// - [UT_TIFF_TAG_X_POSITION] - X position of the image.
/// - [UT_TIFF_TAG_Y_POSITION] - Y position of the image.
/// - [UT_TIFF_TAG_FREE_OFFSETS] - For each string of contiguous unused bytes in
/// a TIFF file, the byte offset of the string.
/// - [UT_TIFF_TAG_FREE_BYTE_COUNTS] - For each string of contiguous unused
/// bytes in a TIFF file, the number of bytes in the string.
/// - [UT_TIFF_TAG_GRAY_RESPONSE_UNIT] - The precision of the information
/// contained in the [UT_TIFF_TAG_GRAY_RESPONSE_CURVE].
/// - [UT_TIFF_TAG_GRAY_RESPONSE_CURVE] - For grayscale data, the optical
/// density of each possible pixel value.
/// - [UT_TIFF_TAG_T4_OPTIONS] - Compression options for
/// [UT_TIFF_COMPRESSION_GROUP_3_FAX].
/// - [UT_TIFF_TAG_T6_OPTIONS] - Compression options for
/// [UT_TIFF_COMPRESSION_GROUP_4_FAX].
/// - [UT_TIFF_TAG_RESOLUTION_UNIT] - The unit of measurement for
/// [UT_TIFF_TAG_X_RESOLUTION] and [UT_TIFF_TAG_Y_RESOLUTION].
/// - [UT_TIFF_TAG_PAGE_NUMBER] - The page number of the page from which this
/// image was scanned.
/// - [UT_TIFF_TAG_TRANSFER_FUNCTION] - Transfer function for the image.
/// - [UT_TIFF_TAG_SOFTWARE] - Name and version number of the software
/// package(s) used to create the image.
/// - [UT_TIFF_TAG_DATE_TIME] - Date and time of image creation.
/// - [UT_TIFF_TAG_ARTIST] - Person who created the image.
/// - [UT_TIFF_TAG_HOST_COMPUTER] - The computer and/or operating system in use
/// at the time of image creation.
/// - [UT_TIFF_TAG_PREDICTOR] - Mathematical operator that is applied to the
/// image data before an encoding scheme is applied.
/// - [UT_TIFF_TAG_WHITE_POINT] - Chromaticity of the white point of the image.
/// - [UT_TIFF_TAG_PRIMARY_CHROMATICITIES] - The chromaticities of the primaries
/// of the image.
/// - [UT_TIFF_TAG_COLOR_MAP] - RGB color map for palette color images.
/// - [UT_TIFF_TAG_HALFTONE_HINTS] - Conveys to the halftone function the range
/// of gray levels within a colorimetrically-specified image that should retain
/// tonal detail.
/// - [UT_TIFF_TAG_TILE_WIDTH] - The tile width in pixels.
/// - [UT_TIFF_TAG_TILE_LENGTH] - The tile length (height) in pixels.
/// - [UT_TIFF_TAG_TILE_OFFSETS] - For each tile, the byte offset of that tile.
/// - [UT_TIFF_TAG_TILE_BYTE_COUNTS] - For each tile, the number of (compressed)
/// bytes in that tile.
/// - [UT_TIFF_TAG_INK_SET] - The set of inks used in a
/// [UT_TIFF_PHOTOMETRIC_INTERPRETATION_SEPERATED] image.
/// - [UT_TIFF_TAG_INK_NAMES] - The name of each ink used in a
/// [UT_TIFF_PHOTOMETRIC_INTERPRETATION_SEPERATED] image.
/// - [UT_TIFF_TAG_NUMBER_OF_INKS] - The number of inks. Usually equal to
/// [UT_TIFF_TAG_SAMPLES_PER_PIXEL], unless there are extra samples.
/// - [UT_TIFF_TAG_DOT_RANGE] - The component values that correspond to a 0% dot
/// and 100% dot.
/// - [UT_TIFF_TAG_TARGET_PRINTER] - A description of the printing environment
/// for which this separation is intended.
/// - [UT_TIFF_TAG_EXTRA_SAMPLES] - Number of extra samples.
/// - [UT_TIFF_TAG_SAMPLE_FORMAT] - How to interpret each data sample in a
/// pixel.
/// - [UT_TIFF_TAG_S_MIN_SAMPLE_VALUE] - The minimum sample value.
/// - [UT_TIFF_TAG_S_MAX_SAMPLE_VALUE] - The maximum sample value.
/// - [UT_TIFF_TAG_TRANSFER_RANGE] - Expands the range of the
/// [UT_TIFF_TAG_TRANSFER_FUNCTION].
/// - [UT_TIFF_TAG_REFERENCE_BLACK_WHITE] - A pair of headroom and footroom
/// image data values (codes) for each pixel component.
/// - [UT_TIFF_TAG_COPYRIGHT] - Copyright notice.
/// - [UT_TIFF_TAG_EXIF_IFD] - Offset to EXIF data.
/// - [UT_TIFF_TAG_GPS_IFD] - Offset to GPS data.
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
  UT_TIFF_TAG_REFERENCE_BLACK_WHITE = 0x0214,
  UT_TIFF_TAG_COPYRIGHT = 0x8298,
  UT_TIFF_TAG_EXIF_IFD = 0x8769,
  UT_TIFF_TAG_GPS_IFD = 0x8825
} UtTiffTagId;

/// Format of contents of a TIFF tag:
/// - [UT_TIFF_TAG_TYPE_BYTE] - contains 8 bit unsigned integer values.
/// - [UT_TIFF_TAG_TYPE_ASCII] - contains an ASCII string.
/// - [UT_TIFF_TAG_TYPE_SHORT] - contains 16 bit unsigned integer values.
/// - [UT_TIFF_TAG_TYPE_LONG] - contains 32 bit unsigned integer values.
/// - [UT_TIFF_TAG_TYPE_RATIONAL] - contains a fractional number constructed
/// from a 32 bit unsigned integer numerator and a 32 bit unsigned integer
/// denominator.
/// - [UT_TIFF_TAG_TYPE_SBYTE] - contains 8 bit signed integer values.
/// - [UT_TIFF_TAG_TYPE_UNDEFINED] - contents are in undefined format.
/// - [UT_TIFF_TAG_TYPE_SSHORT] - contains 16 bit signed integer values.
/// - [UT_TIFF_TAG_TYPE_SLONG] - contains 32 bit signed integer values.
/// - [UT_TIFF_TAG_TYPE_SRATIONAL] - contains a fractional number constructed
/// from a 32 bit signed integer numerator and a 32 bit signed integer
/// denominator.
/// - [UT_TIFF_TAG_TYPE_FLOAT] - contains a 32 bit floating point value.
/// - [UT_TIFF_TAG_TYPE_DOUBLE] - contains a 64 bit floating point value.
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

/// Compression applied to TIFF image data.
/// - [UT_TIFF_COMPRESSION_UNCOMPRESSED] - no compression.
/// - [UT_TIFF_COMPRESSION_CCITT_1D] - .
/// - [UT_TIFF_COMPRESSION_GROUP_3_FAX] - .
/// - [UT_TIFF_COMPRESSION_GROUP_4_FAX] - .
/// - [UT_TIFF_COMPRESSION_LZW] - LZW compression.
/// - [UT_TIFF_COMPRESSION_JPEG] - JPEG compression.
/// - [UT_TIFF_COMPRESSION_DEFLATE] - deflate compression.
/// - [UT_TIFF_COMPRESSION_PACK_BITS] - .
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

/// The color space of the image data:
/// - [UT_TIFF_PHOTOMETRIC_INTERPRETATION_WHITE_IS_ZERO] - Bilevel and greyscale
/// - 0 is white.
/// - [UT_TIFF_PHOTOMETRIC_INTERPRETATION_BLACK_IS_ZERO] - Bilevel and greyscale
/// - 0 is black.
/// - [UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB] - Color images with red, green
/// and blue values.
/// - [UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB_PALETTE] - Color images that index
/// red, green, blue values.
/// - [UT_TIFF_PHOTOMETRIC_INTERPRETATION_TRANSPARENCY_MASK] - Transparency bit
/// mask with 1 values for interior and 0 for exterior.
/// - [UT_TIFF_PHOTOMETRIC_INTERPRETATION_SEPERATED] - Color images with inks,
/// usually CMYK.
/// - [UT_TIFF_PHOTOMETRIC_INTERPRETATION_YCBCR] - YCbCr.
/// - [UT_TIFF_PHOTOMETRIC_INTERPRETATION_CIELAB] - CIE L*a*b*.
typedef enum {
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_WHITE_IS_ZERO = 0,
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_BLACK_IS_ZERO = 1,
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB = 2,
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB_PALETTE = 3,
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_TRANSPARENCY_MASK = 4,
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_SEPERATED = 5,
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_YCBCR = 6,
  UT_TIFF_PHOTOMETRIC_INTERPRETATION_CIELAB = 8
} UtTiffPhotometricInterpretation;

/// For black and white TIFF files that represent shades of gray, the technique
/// used to convert from gray to black and white pixels:
/// - [UT_TIFF_THRESHOLDING_NONE] - No dithering or halftoning has been applied
/// to the image data.
/// - [UT_TIFF_THRESHOLDING_HALFTONE] - An ordered dither or halftone technique
/// has been applied to the image data.
/// - [UT_TIFF_THRESHOLDING_RANDOMIZED] - A randomized process such as error
/// diffusion has been applied to the image data.
typedef enum {
  UT_TIFF_THRESHOLDING_NONE = 1,
  UT_TIFF_THRESHOLDING_HALFTONE = 2,
  UT_TIFF_THRESHOLDING_RANDOMIZED = 3
} UtTiffThresholding;

/// The logical order of bits within a byte:
/// - [UT_TIFF_FILL_ORDER_MOST_SIGNFICANT_FIRST] - Values are written to the
/// higher bits of each byte first.
/// - [UT_TIFF_FILL_ORDER_LEAST_SIGNIFICANT_FIRST] - Values are written to the
/// lower bits of each byte first.
typedef enum {
  UT_TIFF_FILL_ORDER_MOST_SIGNFICANT_FIRST = 1,
  UT_TIFF_FILL_ORDER_LEAST_SIGNIFICANT_FIRST = 2
} UtTiffFillOrder;

/// The orientation of the image with respect to the rows and columns:
/// - [UT_TIFF_ORIENTATION_TOP_LEFT] - .
/// - [UT_TIFF_ORIENTATION_TOP_RIGHT] - .
/// - [UT_TIFF_ORIENTATION_BOTTOM_RIGHT] - .
/// - [UT_TIFF_ORIENTATION_BOTTOM_LEFT] - .
/// - [UT_TIFF_ORIENTATION_LEFT_TOP] - .
/// - [UT_TIFF_ORIENTATION_RIGHT_TOP] - .
/// - [UT_TIFF_ORIENTATION_RIGHT_BOTTOM] - .
/// - [UT_TIFF_ORIENTATION_LEFT_BOTTOM] - .
typedef enum {
  UT_TIFF_ORIENTATION_TOP_LEFT = 1,
  UT_TIFF_ORIENTATION_TOP_RIGHT = 2,
  UT_TIFF_ORIENTATION_BOTTOM_RIGHT = 3,
  UT_TIFF_ORIENTATION_BOTTOM_LEFT = 4,
  UT_TIFF_ORIENTATION_LEFT_TOP = 5,
  UT_TIFF_ORIENTATION_RIGHT_TOP = 6,
  UT_TIFF_ORIENTATION_RIGHT_BOTTOM = 7,
  UT_TIFF_ORIENTATION_LEFT_BOTTOM = 8
} UtTiffOrientation;

/// How the components of each pixel are stored:
/// - [UT_TIFF_PLANAR_CONFIGURATION_CHUNKY] - All planes for a pixel are stored
/// before moving to the next pixel, e.g. for RGB images the data is in the form
/// RGBRGBRGB...
/// - [UT_TIFF_PLANAR_CONFIGURATION_PLANAR] - The components are stored in each
/// plane, e.g. for RGB images the data is in the form RR..RRGG..GGBB..BB
typedef enum {
  UT_TIFF_PLANAR_CONFIGURATION_CHUNKY = 1,
  UT_TIFF_PLANAR_CONFIGURATION_PLANAR = 2
} UtTiffPlanarConfiguration;

/// Units used in pixel density:
/// - [UT_TIFF_RESOLUTION_UNIT_NONE] - units not specified.
/// - [UT_TIFF_RESOLUTION_UNIT_INCH] - dots per inch.
/// - [UT_TIFF_RESOLUTION_UNIT_CENTIMETER] - dots per centimeter.
typedef enum {
  UT_TIFF_RESOLUTION_UNIT_NONE = 1,
  UT_TIFF_RESOLUTION_UNIT_INCH = 2,
  UT_TIFF_RESOLUTION_UNIT_CENTIMETER = 3
} UtTiffResolutionUnit;

/// Mathematical operator that is applied to the image data before an encoding
/// scheme is applied:
/// - [UT_TIFF_PREDICTOR_NONE] - No prediction scheme used before coding.
/// - [UT_TIFF_PREDICTOR_HORIZONTAL_DIFFERENCING] - Horizontal differencing.
typedef enum {
  UT_TIFF_PREDICTOR_NONE = 1,
  UT_TIFF_PREDICTOR_HORIZONTAL_DIFFERENCING = 2
} UtTiffPredictor;

/// Ink set in use in TIFF images:
/// [UT_TIFF_INK_SET_CMYK] - inks are standard CMYK.
/// [UT_TIFF_INK_SET_NOT_CMYK] - inks are not standard CMYK, see the
/// [UT_TIFF_TAG_INK_NAMES] for the inks used.
typedef enum {
  UT_TIFF_INK_SET_CMYK = 1,
  UT_TIFF_INK_SET_NOT_CMYK = 2
} UtTiffInkSet;

/// Creates a new TIFF tag of [type] with [id] and containing raw [data].
///
/// !arg-type data UtUint8List
/// !return-type UtTiffTag
/// !return-ref
UtObject *ut_tiff_tag_new(uint16_t id, uint16_t type, UtObject *data);

/// Creates a new TIFF tag with [id] containing a single byte [value].
///
/// !return-type UtTiffTag
/// !return-ref
UtObject *ut_tiff_tag_new_single_byte(uint16_t id, uint8_t value);

/// Creates a new TIFF tag with [id] containing multiple byte [value]s.
///
/// !arg-type value UtUint8List
/// !return-type UtTiffTag
/// !return-ref
UtObject *ut_tiff_tag_new_byte(uint16_t id, UtObject *value);

/// Creates a new TIFF tag with [id] containing an ASCII string [value].
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

/// Returns list of text inside this tag.
/// Returns an empty list if this tag is not an ASCII tag.
///
/// !return-ref
/// !return-type UtStringList
UtObject *ut_tiff_tag_get_concatenated_ascii(UtObject *object);

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
