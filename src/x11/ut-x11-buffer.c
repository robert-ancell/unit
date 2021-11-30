#include <assert.h>

#include "ut-list.h"
#include "ut-uint8-array.h"
#include "ut-uint8-list.h"
#include "ut-x11-buffer.h"

UtObject *ut_x11_buffer_new() { return ut_uint8_array_new(); }

UtObject *ut_x11_buffer_new_with_data(UtObject *data) {
  return ut_object_ref(data);
}

void ut_x11_buffer_append_card8(UtObject *buffer, uint8_t value) {
  ut_uint8_list_append(buffer, value);
}

void ut_x11_buffer_append_bool(UtObject *buffer, bool value) {
  ut_x11_buffer_append_card8(buffer, value ? 1 : 0);
}

void ut_x11_buffer_append_padding(UtObject *buffer, size_t count) {
  for (size_t i = 0; i < count; i++) {
    ut_x11_buffer_append_card8(buffer, 0);
  }
}

void ut_x11_buffer_append_align_padding(UtObject *buffer, size_t alignment) {
  size_t extra = ut_list_get_length(buffer) % alignment;
  if (extra != 0) {
    ut_x11_buffer_append_padding(buffer, alignment - extra);
  }
}

void ut_x11_buffer_append_card16(UtObject *buffer, uint16_t value) {
  ut_uint8_list_append_uint16_le(buffer, value);
}

void ut_x11_buffer_append_int16(UtObject *buffer, int16_t value) {
  ut_x11_buffer_append_card16(buffer, (uint16_t)value);
}

void ut_x11_buffer_append_card32(UtObject *buffer, uint32_t value) {
  ut_uint8_list_append_uint32_le(buffer, value);
}

void ut_x11_buffer_append_value_card16(UtObject *buffer, uint16_t value) {
  ut_x11_buffer_append_card32(buffer, value);
}

void ut_x11_buffer_append_value_int16(UtObject *buffer, int16_t value) {
  ut_x11_buffer_append_value_card16(buffer, (uint16_t)value);
}

void ut_x11_buffer_append_string8(UtObject *buffer, const char *value) {
  for (const char *c = value; *c != '\0'; c++) {
    ut_x11_buffer_append_card8(buffer, *c);
  }
}

uint8_t ut_x11_buffer_get_card8(UtObject *buffer, size_t *offset) {
  uint8_t value = ut_uint8_list_get_element(buffer, *offset);
  (*offset)++;
  return value;
}

bool ut_x11_buffer_get_bool(UtObject *buffer, size_t *offset) {
  return ut_x11_buffer_get_card8(buffer, offset) != 0;
}

void ut_x11_buffer_get_padding(UtObject *buffer, size_t *offset, size_t count) {
  (*offset) += count;
}

void ut_x11_buffer_get_align_padding(UtObject *buffer, size_t *offset,
                                     size_t alignment) {
  while ((*offset) % alignment != 0) {
    (*offset)++;
  }
}

uint16_t ut_x11_buffer_get_card16(UtObject *buffer, size_t *offset) {
  uint16_t byte1 = ut_x11_buffer_get_card8(buffer, offset);
  uint16_t byte2 = ut_x11_buffer_get_card8(buffer, offset);
  return byte1 | byte2 << 8;
}

int16_t ut_x11_buffer_get_int16(UtObject *buffer, size_t *offset) {
  return (int16_t)ut_x11_buffer_get_card16(buffer, offset);
}

uint32_t ut_x11_buffer_get_card32(UtObject *buffer, size_t *offset) {
  uint32_t byte1 = ut_x11_buffer_get_card8(buffer, offset);
  uint32_t byte2 = ut_x11_buffer_get_card8(buffer, offset);
  uint32_t byte3 = ut_x11_buffer_get_card8(buffer, offset);
  uint32_t byte4 = ut_x11_buffer_get_card8(buffer, offset);
  return byte1 | byte2 << 8 | byte3 << 16 | byte4 << 24;
}

char *ut_x11_buffer_get_string8(UtObject *buffer, size_t *offset,
                                size_t length) {
  assert(ut_list_get_length(buffer) >= *offset + length);
  UtObjectRef value = ut_uint8_array_new();
  for (size_t i = 0; i < length; i++) {
    ut_uint8_list_append(value, ut_uint8_list_get_element(buffer, *offset));
    (*offset)++;
  }
  ut_uint8_list_append(value, '\0');
  return (char *)ut_uint8_list_take_data(value);
}

bool ut_object_is_x11_buffer(UtObject *object) {
  return ut_object_is_uint8_array(object);
}