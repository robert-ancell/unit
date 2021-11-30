#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TrueType composite glyph component.
///
/// !return-ref
/// !return-type UtTrueTypeCompositeGlyphComponent
UtObject *ut_true_type_composite_glyph_component_new(int16_t x_offset,
                                                     int16_t y_offset,
                                                     uint16_t index);

int16_t ut_true_type_composite_glyph_component_get_x_offset(UtObject *object);

int16_t ut_true_type_composite_glyph_component_get_y_offset(UtObject *object);

uint16_t ut_true_type_composite_glyph_component_get_index(UtObject *object);

/// Returns [true] if [object] is a [UtTrueTypeCompositeGlyphComponent].
bool ut_object_is_true_type_composite_glyph_component(UtObject *object);
