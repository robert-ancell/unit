#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new GIF encoder to write [images] to [output_stream].
/// The overall dimnsions of the image are [width]x[height].
/// Colors are provided in [global_color_table].
///
/// !arg-type global_color_table UtUint8List
/// !arg-type images UtObjectList
/// !arg-type output_stream UtOutputStream
/// !return-ref
/// !return-type UtGifEncoder
UtObject *ut_gif_encoder_new(uint16_t width, uint16_t height,
                             UtObject *global_color_table, UtObject *images,
                             UtObject *output_stream);

/// Creates a new GIF encoder to write a single [image] to [output_stream].
///
/// !arg-type image UtGifImage
/// !arg-type output_stream UtOutputStream
/// !return-ref
/// !return-type UtGifEncoder
UtObject *ut_gif_encoder_new_single(UtObject *image, UtObject *output_stream);

/// Add a [comment] to the image to be encoded.
void ut_gif_encoder_add_comment(UtObject *object, const char *comment);

/// Start encoding.
void ut_gif_encoder_encode(UtObject *object);

/// Returns [true] if [object] is a [UtGifEncoder].
bool ut_object_is_gif_encoder(UtObject *object);
