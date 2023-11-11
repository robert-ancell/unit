#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint16_t family;
  UtObject *address;
  char *display;
  char *name;
  UtObject *data;
} UtX11AuthRecord;

static char *ut_x11_auth_record_to_string(UtObject *object) {
  UtX11AuthRecord *self = (UtX11AuthRecord *)object;
  ut_cstring_ref address_string = ut_object_to_string(self->address);
  ut_cstring_ref data_string = ut_object_to_string(self->data);
  return ut_cstring_new_printf("<UtX11AuthRecord>(%d, %s, \"%s\", \"%s\", %s)",
                               self->family, address_string, self->display,
                               self->name, data_string);
}

static void ut_x11_auth_record_cleanup(UtObject *object) {
  UtX11AuthRecord *self = (UtX11AuthRecord *)object;
  ut_object_unref(self->address);
  free(self->display);
  free(self->name);
  ut_object_unref(self->data);
}

static UtObjectInterface object_interface = {
    .type_name = "UtX11AuthRecord",
    .to_string = ut_x11_auth_record_to_string,
    .cleanup = ut_x11_auth_record_cleanup};

UtObject *ut_x11_auth_record_new(uint16_t family, UtObject *address,
                                 const char *display, const char *name,
                                 UtObject *data) {
  UtObject *object = ut_object_new(sizeof(UtX11AuthRecord), &object_interface);
  UtX11AuthRecord *self = (UtX11AuthRecord *)object;

  self->family = family;
  self->address = ut_object_ref(address);
  self->display = ut_cstring_new(display);
  self->name = ut_cstring_new(name);
  self->data = ut_object_ref(data);

  return object;
}

uint16_t ut_x11_auth_record_get_family(UtObject *object) {
  assert(ut_object_is_x11_auth_record(object));
  UtX11AuthRecord *self = (UtX11AuthRecord *)object;
  return self->family;
}

UtObject *ut_x11_auth_record_get_address(UtObject *object) {
  assert(ut_object_is_x11_auth_record(object));
  UtX11AuthRecord *self = (UtX11AuthRecord *)object;
  return self->address;
}

const char *ut_x11_auth_record_get_display(UtObject *object) {
  assert(ut_object_is_x11_auth_record(object));
  UtX11AuthRecord *self = (UtX11AuthRecord *)object;
  return self->display;
}

const char *ut_x11_auth_record_get_name(UtObject *object) {
  assert(ut_object_is_x11_auth_record(object));
  UtX11AuthRecord *self = (UtX11AuthRecord *)object;
  return self->name;
}

UtObject *ut_x11_auth_record_get_data(UtObject *object) {
  assert(ut_object_is_x11_auth_record(object));
  UtX11AuthRecord *self = (UtX11AuthRecord *)object;
  return self->data;
}

bool ut_object_is_x11_auth_record(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
