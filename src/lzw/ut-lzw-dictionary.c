#include <assert.h>

#include "ut-lzw-dictionary.h"
#include "ut.h"

// Number of entries in default LZW dictionary (literal 0-255, clear and end of
// information).
#define MIN_CODES 258

// Number of bits required for default dictionary (258 codes).
#define MIN_CODE_LENGTH 9

typedef struct {
  UtObject object;

  // Mapping from codes to symbols.
  UtObject *entries;

  // Number of bits required to represent number of codes in dictionary.
  size_t code_length;
} UtLzwDictionary;

static void ut_lzw_dictionary_init(UtObject *object) {
  UtLzwDictionary *self = (UtLzwDictionary *)object;

  self->entries = ut_object_list_new();
  // Codes 0-255 are literal bytes.
  for (size_t i = 0; i < 256; i++) {
    UtObjectRef entry = ut_uint8_list_new_from_elements(1, i);
    ut_list_append(self->entries, entry);
  }
  // Clear.
  UtObjectRef clear_entry = ut_uint8_list_new();
  ut_list_append(self->entries, clear_entry);
  // End of Information.
  UtObjectRef eoi_entry = ut_uint8_list_new();
  ut_list_append(self->entries, eoi_entry);

  self->code_length = MIN_CODE_LENGTH;
}

static void ut_lzw_dictionary_cleanup(UtObject *object) {
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  ut_object_unref(self->entries);
}

static UtObjectInterface object_interface = {.type_name = "UtLzwDictionary",
                                             .init = ut_lzw_dictionary_init,
                                             .cleanup =
                                                 ut_lzw_dictionary_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_lzw_dictionary_new() {
  return ut_object_new(sizeof(UtLzwDictionary), &object_interface);
}

size_t ut_lzw_dictionary_get_length(UtObject *object) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  return ut_list_get_length(self->entries);
}

UtObject *ut_lzw_dictionary_lookup(UtObject *object, uint16_t code) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  return ut_object_list_get_element(self->entries, code);
}

size_t ut_lzw_dictionary_get_code_length(UtObject *object) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  return self->code_length;
}

void ut_lzw_dictionary_clear(UtObject *object) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  ut_list_resize(self->entries, MIN_CODES);
  self->code_length = MIN_CODE_LENGTH;
}

void ut_lzw_dictionary_append(UtObject *object, uint16_t code, uint8_t b) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  UtObject *entry = ut_object_list_get_element(self->entries, code);
  UtObjectRef new_entry = ut_list_copy(entry);
  ut_uint8_list_append(new_entry, b);
  ut_list_append(self->entries, new_entry);

  // Need to use more bits as the dictionary gets bigger.
  if (ut_list_get_length(self->entries) == 1 << self->code_length) {
    self->code_length++;
  }
}

bool ut_object_is_lzw_dictionary(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
