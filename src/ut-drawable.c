#include <assert.h>

#include "ut.h"

int ut_drawable_id = 0;

void ut_drawable_clear(UtObject *object, double red, double green, double blue,
                       double alpha) {
  UtDrawableInterface *drawable_interface =
      ut_object_get_interface(object, &ut_drawable_id);
  assert(drawable_interface != NULL);
  drawable_interface->clear(object, red, green, blue, alpha);
}

bool ut_object_implements_drawable(UtObject *object) {
  return ut_object_get_interface(object, &ut_drawable_id) != NULL;
}
