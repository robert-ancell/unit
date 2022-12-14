#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint8_t depth;
  uint8_t bits_per_pixel;
  uint8_t scanline_pad;
} UtX11PixmapFormat;

static UtObjectInterface object_interface = {.type_name = "UtX11PixmapFormat",
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_x11_pixmap_format_new(uint8_t depth, uint8_t bits_per_pixel,
                                   uint8_t scanline_pad) {
  UtObject *object =
      ut_object_new(sizeof(UtX11PixmapFormat), &object_interface);
  UtX11PixmapFormat *self = (UtX11PixmapFormat *)object;
  self->depth = depth;
  self->bits_per_pixel = bits_per_pixel;
  self->scanline_pad = scanline_pad;
  return object;
}

uint8_t ut_x11_pixmap_format_get_depth(UtObject *object) {
  assert(ut_object_is_x11_pixmap_format(object));
  UtX11PixmapFormat *self = (UtX11PixmapFormat *)object;
  return self->depth;
}

uint8_t ut_x11_pixmap_format_get_bits_per_pixel(UtObject *object) {
  assert(ut_object_is_x11_pixmap_format(object));
  UtX11PixmapFormat *self = (UtX11PixmapFormat *)object;
  return self->bits_per_pixel;
}

uint8_t ut_x11_pixmap_format_get_scanline_pad(UtObject *object) {
  assert(ut_object_is_x11_pixmap_format(object));
  UtX11PixmapFormat *self = (UtX11PixmapFormat *)object;
  return self->scanline_pad;
}

bool ut_object_is_x11_pixmap_format(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
