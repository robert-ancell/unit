#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t pixmap;
} UtX11PixmapError;

static UtX11ErrorInterface x11_error_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtX11PixmapError",
    .interfaces = {{&ut_x11_error_id, &x11_error_interface}, {NULL, NULL}}};

UtObject *ut_x11_pixmap_error_new(uint32_t pixmap) {
  UtObject *object = ut_object_new(sizeof(UtX11PixmapError), &object_interface);
  UtX11PixmapError *self = (UtX11PixmapError *)object;
  self->pixmap = pixmap;
  return object;
}

uint32_t ut_x11_pixmap_error_get_pixmap(UtObject *object) {
  assert(ut_object_is_x11_pixmap_error(object));
  UtX11PixmapError *self = (UtX11PixmapError *)object;
  return self->pixmap;
}

bool ut_object_is_x11_pixmap_error(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
