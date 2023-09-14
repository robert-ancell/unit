#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// The action to take when rendering the next GIF image.
typedef enum {
  UT_GIF_DISPOSAL_METHOD_NONE,
  UT_GIF_DISPOSAL_METHOD_DO_NOT_DISPOSE,
  UT_GIF_DISPOSAL_METHOD_RESTORE_TO_BACKGROUND,
  UT_GIF_DISPOSAL_METHOD_RESTORE_TO_PREVIOUS
} UtGifDisposalMethod;

/// Creates a new GIF image of size [width]x[height] pixels offset [left],[top].
/// Each pixel is stored as an index in [data] to the RGB values in
/// [color_table].
///
/// !arg-type color_table UtUint8List
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtGifImage
UtObject *ut_gif_image_new(uint16_t left, uint16_t top, uint16_t width,
                           uint16_t height, UtObject *color_table,
                           UtObject *data);

/// Returns the left pixel position of this image.
uint16_t ut_gif_image_get_left(UtObject *object);

/// Returns the left top position of this image.
uint16_t ut_gif_image_get_top(UtObject *object);

/// Returns the width of this image in pixels.
uint16_t ut_gif_image_get_width(UtObject *object);

/// Returns the height of this image in pixels.
uint16_t ut_gif_image_get_height(UtObject *object);

/// Sets the [disposal_method] to use when rendering the next image.
void ut_gif_image_set_disposal_method(UtObject *object,
                                      UtGifDisposalMethod disposal_method);

/// Returns the action to take when rendering the next image.
UtGifDisposalMethod ut_gif_image_get_disposal_method(UtObject *object);

/// Set the [delay_time] wait before showing this frame in 1/100th of a second.
void ut_gif_image_set_delay_time(UtObject *object, uint16_t delay_time);

/// Returns the amount of time to wait before showing this frame in 1/100th of a
/// second.
uint16_t ut_gif_image_get_delay_time(UtObject *object);

/// Returns the color table for this image.
///
/// !return-type UtUint8List
UtObject *ut_gif_image_get_color_table(UtObject *object);

/// Returns the image data.
///
/// !return-type UtUint8List
UtObject *ut_gif_image_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtGifImage].
bool ut_object_is_gif_image(UtObject *object);
