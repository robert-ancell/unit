#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t id;
  char *name;
  uint16_t type;
  uint32_t attachment;
  UtObject *classes;
  bool enabled;
} UtX11InputDeviceInfo;

static void ut_x11_input_device_info_cleanup(UtObject *object) {
  UtX11InputDeviceInfo *self = (UtX11InputDeviceInfo *)object;
  ut_object_unref(self->classes);
  free(self->name);
}

static UtObjectInterface object_interface = {
    .type_name = "UtX11InputDeviceInfo",
    .cleanup = ut_x11_input_device_info_cleanup,
    .interfaces = {{NULL, NULL}}};

UtObject *ut_x11_input_device_info_new(uint32_t id, const char *name,
                                       uint16_t type, uint32_t attachment,
                                       UtObject *classes, bool enabled) {
  UtObject *object =
      ut_object_new(sizeof(UtX11InputDeviceInfo), &object_interface);
  UtX11InputDeviceInfo *self = (UtX11InputDeviceInfo *)object;
  self->id = id;
  self->name = ut_cstring_new(name);
  self->type = type;
  self->attachment = attachment;
  self->classes = ut_object_ref(classes);
  self->enabled = enabled;
  return object;
}

uint32_t ut_x11_input_device_info_get_id(UtObject *object) {
  assert(ut_object_is_x11_input_device_info(object));
  UtX11InputDeviceInfo *self = (UtX11InputDeviceInfo *)object;
  return self->id;
}

const char *ut_x11_input_device_info_get_name(UtObject *object) {
  assert(ut_object_is_x11_input_device_info(object));
  UtX11InputDeviceInfo *self = (UtX11InputDeviceInfo *)object;
  return self->name;
}

uint16_t ut_x11_input_device_info_get_type(UtObject *object) {
  assert(ut_object_is_x11_input_device_info(object));
  UtX11InputDeviceInfo *self = (UtX11InputDeviceInfo *)object;
  return self->type;
}

uint32_t ut_x11_input_device_info_get_attachent(UtObject *object) {
  assert(ut_object_is_x11_input_device_info(object));
  UtX11InputDeviceInfo *self = (UtX11InputDeviceInfo *)object;
  return self->attachment;
}

UtObject *ut_x11_input_device_info_get_classes(UtObject *object) {
  assert(ut_object_is_x11_input_device_info(object));
  UtX11InputDeviceInfo *self = (UtX11InputDeviceInfo *)object;
  return self->classes;
}

bool ut_x11_input_device_info_get_enabled(UtObject *object) {
  assert(ut_object_is_x11_input_device_info(object));
  UtX11InputDeviceInfo *self = (UtX11InputDeviceInfo *)object;
  return self->enabled;
}

bool ut_object_is_x11_input_device_info(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
