#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TrueType glyph.
///
/// !arg-type components UtObjectList
/// !return-ref
/// !return-type UtTrueTypeCompositeGlyph
UtObject *ut_true_type_composite_glyph_new(int16_t x_min, int16_t y_min,
                                           int16_t x_max, int16_t y_max,
                                           uint16_t advance_width,
                                           int16_t left_side_bearing,
                                           UtObject *components);

int16_t ut_true_type_composite_glyph_get_x_min(UtObject *object);

int16_t ut_true_type_composite_glyph_get_y_min(UtObject *object);

int16_t ut_true_type_composite_glyph_get_x_max(UtObject *object);

int16_t ut_true_type_composite_glyph_get_y_max(UtObject *object);

UtObject *ut_true_type_composite_glyph_get_components(UtObject *object);

/// Returns [true] if [object] is a [UtTrueTypeCompositeGlyph].
bool ut_object_is_true_type_composite_glyph(UtObject *object);
