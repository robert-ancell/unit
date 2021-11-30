#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TrueType glyph.
///
/// !arg-type contours UtObjectList
/// !return-ref
/// !return-type UtTrueTypeSimpleGlyph
UtObject *ut_true_type_simple_glyph_new(int16_t x_min, int16_t y_min,
                                        int16_t x_max, int16_t y_max,
                                        uint16_t advance_width,
                                        int16_t left_side_bearing,
                                        UtObject *contours);

int16_t ut_true_type_simple_glyph_get_x_min(UtObject *object);

int16_t ut_true_type_simple_glyph_get_y_min(UtObject *object);

int16_t ut_true_type_simple_glyph_get_x_max(UtObject *object);

int16_t ut_true_type_simple_glyph_get_y_max(UtObject *object);

UtObject *ut_true_type_simple_glyph_get_contours(UtObject *object);

/// Returns [true] if [object] is a [UtTrueTypeSimpleGlyph].
bool ut_object_is_true_type_simple_glyph(UtObject *object);
