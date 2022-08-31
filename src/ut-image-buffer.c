#include <assert.h>

#include "ut.h"

int ut_image_buffer_id = 0;

size_t ut_image_buffer_get_width(UtObject *object) {
  UtImageBufferInterface *image_buffer_interface =
      ut_object_get_interface(object, &ut_image_buffer_id);
  assert(image_buffer_interface != NULL);
  return image_buffer_interface->get_width(object);
}

size_t ut_image_buffer_get_height(UtObject *object) {
  UtImageBufferInterface *image_buffer_interface =
      ut_object_get_interface(object, &ut_image_buffer_id);
  assert(image_buffer_interface != NULL);
  return image_buffer_interface->get_height(object);
}

UtObject *ut_image_buffer_get_data(UtObject *object) {
  UtImageBufferInterface *image_buffer_interface =
      ut_object_get_interface(object, &ut_image_buffer_id);
  assert(image_buffer_interface != NULL);
  return image_buffer_interface->get_data(object);
}

bool ut_object_implements_image_buffer(UtObject *object) {
  return ut_object_get_interface(object, &ut_image_buffer_id) != NULL;
}
