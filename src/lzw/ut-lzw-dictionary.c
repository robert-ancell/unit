#include <assert.h>

#include "ut-lzw-dictionary.h"
#include "ut.h"

typedef struct {
  UtObject object;

  // Number of symbols being encoded.
  size_t n_symbols;

  // Mapping from codes to symbols.
  UtObject *codes;

  // Number of bits required to represent number of codes in dictionary.
  size_t code_length;
} UtLzwDictionary;

// Get number of bits required to send all code words.
static size_t get_code_length(UtLzwDictionary *self) {
  size_t code_length = 1;
  size_t max_code = ut_list_get_length(self->codes) - 1;
  while (max_code >= (1 << code_length)) {
    code_length++;
  }
  return code_length;
}

static void ut_lzw_dictionary_init(UtObject *object) {
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  self->codes = ut_object_list_new();
}

static void ut_lzw_dictionary_cleanup(UtObject *object) {
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  ut_object_unref(self->codes);
}

static UtObjectInterface object_interface = {.type_name = "UtLzwDictionary",
                                             .init = ut_lzw_dictionary_init,
                                             .cleanup =
                                                 ut_lzw_dictionary_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_lzw_dictionary_new(size_t n_symbols) {
  UtObject *object = ut_object_new(sizeof(UtLzwDictionary), &object_interface);
  UtLzwDictionary *self = (UtLzwDictionary *)object;

  self->n_symbols = n_symbols;
  // Symbols.
  for (size_t i = 0; i < self->n_symbols; i++) {
    UtObjectRef entry = ut_uint8_list_new_from_elements(1, i);
    ut_list_append(self->codes, entry);
  }
  // Clear.
  UtObjectRef clear_entry = ut_uint8_list_new();
  ut_list_append(self->codes, clear_entry);
  // End of Information.
  UtObjectRef eoi_entry = ut_uint8_list_new();
  ut_list_append(self->codes, eoi_entry);
  self->code_length = get_code_length(self);

  return object;
}

uint16_t ut_lzw_dictionary_get_clear_code(UtObject *object) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  return self->n_symbols;
}

uint16_t ut_lzw_dictionary_get_end_of_information_code(UtObject *object) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  return self->n_symbols + 1;
}

size_t ut_lzw_dictionary_get_length(UtObject *object) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  return ut_list_get_length(self->codes);
}

UtObject *ut_lzw_dictionary_lookup(UtObject *object, uint16_t code) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  return ut_object_list_get_element(self->codes, code);
}

size_t ut_lzw_dictionary_get_code_length(UtObject *object) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  return self->code_length;
}

void ut_lzw_dictionary_clear(UtObject *object) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  // Return to the original symobls, clear, end of information.
  ut_list_resize(self->codes, self->n_symbols + 2);
  self->code_length = get_code_length(self);
}

void ut_lzw_dictionary_append(UtObject *object, uint16_t code, uint8_t b) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  UtObject *entry = ut_object_list_get_element(self->codes, code);
  UtObjectRef new_entry = ut_list_copy(entry);
  ut_uint8_list_append(new_entry, b);
  ut_list_append(self->codes, new_entry);

  // Need to use more bits as the dictionary gets bigger.
  if (ut_list_get_length(self->codes) == 1 << self->code_length) {
    self->code_length++;
  }
}

bool ut_object_is_lzw_dictionary(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
