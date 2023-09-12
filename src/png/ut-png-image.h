#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef enum {
  UT_PNG_COLOR_TYPE_GREYSCALE,
  UT_PNG_COLOR_TYPE_TRUECOLOR,
  UT_PNG_COLOR_TYPE_INDEXED_COLOR,
  UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA,
  UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA
} UtPngColorType;

/// Creates a new PNG image with dimensions [width] and [height].
/// [data] contains the packed image samples that match [bit_depth] and
/// [color_type].
///
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtPngImage
UtObject *ut_png_image_new(uint32_t width, uint32_t height, uint8_t bit_depth,
                           UtPngColorType color_type, UtObject *data);

/// Returns the width of the image in pixels.
uint32_t ut_png_image_get_width(UtObject *object);

/// Returns the height of the image in pixels.
uint32_t ut_png_image_get_height(UtObject *object);

/// Returns the number of bits in each sample.
uint8_t ut_png_image_get_bit_depth(UtObject *object);

/// Returns the color type used in the image data.
UtPngColorType ut_png_image_get_color_type(UtObject *object);

/// Returns the number of channels per pixel.
size_t ut_png_image_get_n_channels(UtObject *object);

/// Returns the number of channels per pixel.
size_t ut_png_image_get_row_stride(UtObject *object);

/// Sets the RGB [palette] for this image.
///
/// !arg-type palette UtUint8List
void ut_png_image_set_palette(UtObject *object, UtObject *palette);

/// Returns the RGB palette for this image or [NULL] if it has none.
///
/// !returng-type UtUint8List NULL
UtObject *ut_png_image_get_palette(UtObject *object);

/// Sets the RGB [background_color].
///
/// !arg-type background_color UtUint8List
void ut_png_image_set_background_color(UtObject *object,
                                       UtObject *background_color);

/// Returns the RGB background color.
///
/// !return-type UtUint8List
UtObject *ut_png_image_get_background_color(UtObject *object);

/// Set metadata [keyword] to [text].
void ut_png_image_set_text(UtObject *object, const char *keyword,
                           const char *text);

/// Set metadata [keyword] and [language] to [translated_keyword] and [text].
void ut_png_image_set_international_text(UtObject *object, const char *keyword,
                                         const char *language,
                                         const char *translated_keyword,
                                         const char *text);

/// Returns metadata that matches [keyword] or NULL if none found.
const char *ut_png_image_lookup_text(UtObject *object, const char *keyword);

/// Returns metadata that matches [keyword] and [language] or NULL if none
/// found. The translated form of [keyword] will be returned in
/// [translated_keyword].
const char *
ut_png_image_lookup_international_text(UtObject *object, const char *keyword,
                                       const char *language,
                                       const char **translated_keyword);

/// Returns the image data.
///
/// !return-type UtUint8List
UtObject *ut_png_image_get_data(UtObject *object);

/// Returns the image data converted to RGBA form.
///
/// !return-ref
/// !return-type UtUint8List
UtObject *ut_png_image_to_rgba(UtObject *object);

/// Returns [true] if [object] is a [UtPngImage].
bool ut_object_is_png_image(UtObject *object);
