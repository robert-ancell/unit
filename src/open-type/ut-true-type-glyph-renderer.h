#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TrueType glyph renderer to render [glyph] from [font] at a
/// [height_in_pixels].
///
/// !arg-type glyph UtTrueTypeSimpleGlyph
/// !return-ref
/// !return-type UtTrueTypeGlyphRenderer
UtObject *ut_true_type_glyph_renderer_new(UtObject *font, size_t glyph,
                                          size_t height_in_pixels);

/// Renders this glyph.
void ut_true_type_glyph_renderer_render(UtObject *object);

/// Returns the width of the rendered glyph in pixels.
size_t ut_true_type_glyph_renderer_get_width(UtObject *object);

/// Returns the height of the rendered glyph in pixels.
size_t ut_true_type_glyph_renderer_get_height(UtObject *object);

/// Returns the data of the rendered glyph.
///
/// !return-ref
/// !return-type UtUint8List
UtObject *ut_true_type_glyph_renderer_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtTrueTypeGlyphRenderer].
bool ut_object_is_true_type_glyph_renderer(UtObject *object);
