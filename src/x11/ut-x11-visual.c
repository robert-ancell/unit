#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t id;
  uint8_t depth;
  uint8_t class;
  uint8_t bits_per_rgb_value;
  uint16_t colormap_entries;
  uint32_t red_mask;
  uint32_t green_mask;
  uint32_t blue_mask;
} UtX11Visual;

static UtObjectInterface object_interface = {.type_name = "UtX11Visual"};

UtObject *ut_x11_visual_new(uint32_t id, uint8_t depth, uint8_t class,
                            uint8_t bits_per_rgb_value,
                            uint16_t colormap_entries, uint32_t red_mask,
                            uint32_t green_mask, uint32_t blue_mask) {
  UtObject *object = ut_object_new(sizeof(UtX11Visual), &object_interface);
  UtX11Visual *self = (UtX11Visual *)object;
  self->id = id;
  self->depth = depth;
  self->class = class;
  self->bits_per_rgb_value = bits_per_rgb_value;
  self->colormap_entries = colormap_entries;
  self->red_mask = red_mask;
  self->green_mask = green_mask;
  self->blue_mask = blue_mask;
  return object;
}

uint32_t ut_x11_visual_get_id(UtObject *object) {
  assert(ut_object_is_x11_visual(object));
  UtX11Visual *self = (UtX11Visual *)object;
  return self->id;
}

uint8_t ut_x11_visual_get_depth(UtObject *object) {
  assert(ut_object_is_x11_visual(object));
  UtX11Visual *self = (UtX11Visual *)object;
  return self->depth;
}

bool ut_object_is_x11_visual(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
