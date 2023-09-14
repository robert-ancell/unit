#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a color containing [red], [green], [blue] and [alpha]
/// values. The values are in the range [0.0, 1.0] and represent the
///
/// !return-type UtColor
/// !return-ref
UtObject *ut_color_new_rgba(double red, double green, double blue,
                            double alpha);

/// Creates a color from a color hash.
/// [hex_string] is in the form `#rrggbb` where `rr`, `gg` and `bb` are the red,
/// green and blue components in hexadecimal, e.g. `#E95420`
///
/// !return-type UtColor
/// !return-ref
UtObject *ut_color_new_from_hex_string(const char *hex_string);

/// Returns the red component of this color in the range [0.0, 1.0]
double ut_color_get_red(UtObject *object);

/// Returns the green component of this color in the range [0.0, 1.0]
double ut_color_get_green(UtObject *object);

/// Returns the blue component of this color in the range [0.0, 1.0]
double ut_color_get_blue(UtObject *object);

/// Returns the alpha component of this color in the range [0.0, 1.0]
double ut_color_get_alpha(UtObject *object);

/// Returns [true] if [object] is a [UtColor].
bool ut_object_is_color(UtObject *object);
