#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint16_t id;
  uint16_t type;
  UtObject *data;
} UtTiffTag;

static char *ut_tiff_tag_to_string(UtObject *object) {
  UtTiffTag *self = (UtTiffTag *)object;
  ut_cstring_ref data_string = ut_object_to_string(self->data);
  return ut_cstring_new_printf("<UtTiffTag>(0x%04x, 0x%04x, %s)", self->id,
                               self->type, data_string);
}

static void ut_tiff_tag_cleanup(UtObject *object) {
  UtTiffTag *self = (UtTiffTag *)object;
  ut_object_unref(self->data);
}

static UtObjectInterface object_interface = {.type_name = "UtTiffTag",
                                             .to_string = ut_tiff_tag_to_string,
                                             .cleanup = ut_tiff_tag_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_tiff_tag_new(uint16_t id, uint16_t type, UtObject *data) {
  UtObject *object = ut_object_new(sizeof(UtTiffTag), &object_interface);
  UtTiffTag *self = (UtTiffTag *)object;
  self->id = id;
  self->type = type;
  self->data = ut_object_ref(data);
  return object;
}

UtObject *ut_tiff_tag_new_single_byte(uint16_t id, uint8_t value) {
  UtObjectRef data = ut_uint8_list_new_from_elements(1, value);
  return ut_tiff_tag_new(id, UT_TIFF_TAG_TYPE_BYTE, data);
}

UtObject *ut_tiff_tag_new_byte(uint16_t id, UtObject *value) {
  return ut_tiff_tag_new(id, UT_TIFF_TAG_TYPE_BYTE, value);
}

UtObject *ut_tiff_tag_new_ascii(uint16_t id, const char *value) {
  UtObjectRef data = ut_uint8_list_new();
  for (const char *c = value; *c != '\0'; c++) {
    if (*c > 127) {
      return ut_tiff_error_new("Invalid ASCII string");
    }
    ut_uint8_list_append(data, *c);
  }
  ut_uint8_list_append(data, '\0');
  return ut_tiff_tag_new(id, UT_TIFF_TAG_TYPE_ASCII, data);
}

uint16_t ut_tiff_tag_get_id(UtObject *object) {
  assert(ut_object_is_tiff_tag(object));
  UtTiffTag *self = (UtTiffTag *)object;
  return self->id;
}

uint16_t ut_tiff_tag_get_type(UtObject *object) {
  assert(ut_object_is_tiff_tag(object));
  UtTiffTag *self = (UtTiffTag *)object;
  return self->type;
}

size_t ut_tiff_tag_get_count(UtObject *object) {
  assert(ut_object_is_tiff_tag(object));
  UtTiffTag *self = (UtTiffTag *)object;
  return ut_list_get_length(self->data);
}

UtObject *ut_tiff_tag_get_data(UtObject *object) {
  assert(ut_object_is_tiff_tag(object));
  UtTiffTag *self = (UtTiffTag *)object;
  return self->data;
}

uint8_t ut_tiff_tag_get_byte(UtObject *object, size_t index) {
  assert(ut_object_is_tiff_tag(object));
  UtTiffTag *self = (UtTiffTag *)object;
  assert(self->type == UT_TIFF_TAG_TYPE_BYTE);
  return ut_uint8_list_get_element(self->data, index);
}

char *ut_tiff_tag_get_ascii(UtObject *object) {
  assert(ut_object_is_tiff_tag(object));
  UtTiffTag *self = (UtTiffTag *)object;
  assert(self->type == UT_TIFF_TAG_TYPE_ASCII);
  UtObjectRef string =
      ut_string_new_from_utf8(self->data); // FIXME: _from_ascii
  return ut_string_take_text(string);
}

uint16_t ut_tiff_tag_get_short(UtObject *object, size_t index) {
  assert(ut_object_is_tiff_tag(object));
  UtTiffTag *self = (UtTiffTag *)object;
  assert(self->type == UT_TIFF_TAG_TYPE_SHORT);
  return ut_uint16_list_get_element(self->data, index);
}

uint32_t ut_tiff_tag_get_long(UtObject *object, size_t index) {
  assert(ut_object_is_tiff_tag(object));
  UtTiffTag *self = (UtTiffTag *)object;
  assert(self->type == UT_TIFF_TAG_TYPE_LONG);
  return ut_uint32_list_get_element(self->data, index);
}

uint32_t ut_tiff_tag_get_short_or_long(UtObject *object, size_t index) {
  assert(ut_object_is_tiff_tag(object));
  UtTiffTag *self = (UtTiffTag *)object;
  assert(self->type == UT_TIFF_TAG_TYPE_SHORT ||
         self->type == UT_TIFF_TAG_TYPE_LONG);
  return self->type == UT_TIFF_TAG_TYPE_SHORT
             ? ut_uint16_list_get_element(self->data, index)
             : ut_uint32_list_get_element(self->data, index);
}

bool ut_object_is_tiff_tag(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
