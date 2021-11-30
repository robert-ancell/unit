#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new OpenType font.
///
/// !arg-type glyphs UtObjectList
/// !return-ref
/// !return-type UtOpenTypeFont
UtObject *ut_open_type_font_new(uint16_t units_per_em, UtObject *glyphs);

/// Returns the the number of font units per em.
uint16_t ut_open_type_font_get_units_per_em(UtObject *object);

/// Returns the glyphs this font contains.
///
/// !return-type UtObjectList
UtObject *ut_open_type_font_get_glyphs(UtObject *object);

/// Returns [true] if [object] is a [UtOpenTypeFont].
bool ut_object_is_open_type_font(UtObject *object);
