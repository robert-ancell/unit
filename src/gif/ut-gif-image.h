#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef enum {
  UT_GIF_DISPOSAL_METHOD_NONE,
  UT_GIF_DISPOSAL_METHOD_DO_NOT_DISPOSE,
  UT_GIF_DISPOSAL_METHOD_RESTORE_TO_BACKGROUND,
  UT_GIF_DISPOSAL_METHOD_RESTORE_TO_PREVIOUS
} UtGifDisposalMethod;

UtObject *ut_gif_image_new(uint16_t left, uint16_t top, uint16_t width,
                           uint16_t height, UtObject *color_table,
                           UtObject *data);

uint16_t ut_gif_image_get_left(UtObject *object);

uint16_t ut_gif_image_get_top(UtObject *object);

uint16_t ut_gif_image_get_width(UtObject *object);

uint16_t ut_gif_image_get_height(UtObject *object);

void ut_gif_image_set_disposal_method(UtObject *object,
                                      UtGifDisposalMethod disposal_method);

UtGifDisposalMethod ut_gif_image_get_disposal_method(UtObject *object);

void ut_gif_image_set_delay_time(UtObject *object, uint16_t delay_time);

uint16_t ut_gif_image_get_delay_time(UtObject *object);

UtObject *ut_gif_image_get_color_table(UtObject *object);

UtObject *ut_gif_image_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtGifImage].
bool ut_object_is_gif_image(UtObject *object);
