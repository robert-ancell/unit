#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t root;
  uint32_t default_colormap;
  uint32_t white_pixel;
  uint32_t black_pixel;
  uint32_t current_input_masks;
  uint16_t width_in_pixels;
  uint16_t height_in_pixels;
  uint16_t width_in_millimeters;
  uint16_t height_in_millimeters;
  UtObject *root_visual;
  UtObject *visuals;
} UtX11Screen;

static void ut_x11_screen_cleanup(UtObject *object) {
  UtX11Screen *self = (UtX11Screen *)object;
  ut_object_unref(self->root_visual);
  ut_object_unref(self->visuals);
}

static UtObjectInterface object_interface = {.type_name = "UtX11Screen",
                                             .cleanup = ut_x11_screen_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_x11_screen_new(uint32_t root, uint32_t default_colormap,
                            uint32_t white_pixel, uint32_t black_pixel,
                            uint32_t current_input_masks,
                            uint16_t width_in_pixels, uint16_t height_in_pixels,
                            uint16_t width_in_millimeters,
                            uint16_t height_in_millimeters,
                            UtObject *root_visual, UtObject *visuals) {
  UtObject *object = ut_object_new(sizeof(UtX11Screen), &object_interface);
  UtX11Screen *self = (UtX11Screen *)object;
  self->root = root;
  self->default_colormap = default_colormap;
  self->white_pixel = white_pixel;
  self->black_pixel = black_pixel;
  self->current_input_masks = current_input_masks;
  self->width_in_pixels = width_in_pixels;
  self->height_in_pixels = height_in_pixels;
  self->width_in_millimeters = width_in_millimeters;
  self->height_in_millimeters = height_in_millimeters;
  self->root_visual = ut_object_ref(root_visual);
  self->visuals = ut_object_ref(visuals);
  return object;
}

uint32_t ut_x11_screen_get_root(UtObject *object) {
  assert(ut_object_is_x11_screen(object));
  UtX11Screen *self = (UtX11Screen *)object;
  return self->root;
}

UtObject *ut_x11_screen_get_root_visual(UtObject *object) {
  assert(ut_object_is_x11_screen(object));
  UtX11Screen *self = (UtX11Screen *)object;
  return self->root_visual;
}

UtObject *ut_x11_screen_get_visuals(UtObject *object) {
  assert(ut_object_is_x11_screen(object));
  UtX11Screen *self = (UtX11Screen *)object;
  return self->visuals;
}

bool ut_object_is_x11_screen(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
