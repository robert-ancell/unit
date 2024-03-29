#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-x11-error.h"

#pragma once

typedef struct {
  uint8_t (*get_major_opcode)(UtObject *object);
  uint8_t (*get_first_event)(UtObject *object);
  uint8_t (*get_first_error)(UtObject *object);
  bool (*decode_event)(UtObject *object, uint8_t code, bool from_send_event,
                       uint16_t sequence_number, uint8_t data0, UtObject *data);
  bool (*decode_generic_event)(UtObject *object, uint16_t code, UtObject *data);
  UtX11ErrorCode (*decode_error)(UtObject *object, uint8_t code);
  void (*close)(UtObject *object);
} UtX11ExtensionInterface;

extern int ut_x11_extension_id;

uint8_t ut_x11_extension_get_major_opcode(UtObject *object);

uint8_t ut_x11_extension_get_first_event(UtObject *object);

uint8_t ut_x11_extension_get_first_error(UtObject *object);

bool ut_x11_extension_decode_event(UtObject *object, uint8_t code,
                                   bool from_send_event,
                                   uint16_t sequence_number, uint8_t data0,
                                   UtObject *data);

bool ut_x11_extension_decode_generic_event(UtObject *object, uint16_t code,
                                           UtObject *data);

UtX11ErrorCode ut_x11_extension_decode_error(UtObject *object, uint8_t code);

void ut_x11_extension_close(UtObject *object);

/// Returns [true] if [object] is a [UtX11Extension].
bool ut_object_implements_x11_extension(UtObject *object);
