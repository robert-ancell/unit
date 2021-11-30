#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;

  uint16_t units_per_em;
  UtObject *glyphs;
} UtOpenTypeFont;

static void ut_open_type_font_cleanup(UtObject *object) {
  UtOpenTypeFont *self = (UtOpenTypeFont *)object;
  ut_object_unref(self->glyphs);
}

static UtObjectInterface object_interface = {.type_name = "UtOpenTypeFont",
                                             .cleanup =
                                                 ut_open_type_font_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_open_type_font_new(uint16_t units_per_em, UtObject *glyphs) {
  UtObject *object = ut_object_new(sizeof(UtOpenTypeFont), &object_interface);
  UtOpenTypeFont *self = (UtOpenTypeFont *)object;
  self->units_per_em = units_per_em;
  self->glyphs = ut_object_ref(glyphs);
  return object;
}

uint16_t ut_open_type_font_get_units_per_em(UtObject *object) {
  assert(ut_object_is_open_type_font(object));
  UtOpenTypeFont *self = (UtOpenTypeFont *)object;
  return self->units_per_em;
}

UtObject *ut_open_type_font_get_glyphs(UtObject *object) {
  assert(ut_object_is_open_type_font(object));
  UtOpenTypeFont *self = (UtOpenTypeFont *)object;
  return self->glyphs;
}

bool ut_object_is_open_type_font(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
