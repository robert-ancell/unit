#include <assert.h>

#include "ut-lzw-dictionary.h"
#include "ut.h"

typedef struct {
  UtObject object;

  // Number of symbols being encoded.
  size_t n_symbols;

  // Maximum length of dictionary.
  size_t max_length;

  // Mapping from codes to symbols.
  UtObject *codes;
} UtLzwDictionary;

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

UtObject *ut_lzw_dictionary_new(size_t n_symbols, size_t max_length) {
  UtObject *object = ut_object_new(sizeof(UtLzwDictionary), &object_interface);
  UtLzwDictionary *self = (UtLzwDictionary *)object;

  self->n_symbols = n_symbols;
  self->max_length = max_length;
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

bool ut_lzw_dictionary_get_is_full(UtObject *object) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  return ut_list_get_length(self->codes) >= self->max_length;
}

UtObject *ut_lzw_dictionary_lookup(UtObject *object, uint16_t code) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  return ut_object_list_get_element(self->codes, code);
}

void ut_lzw_dictionary_clear(UtObject *object) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;
  // Return to the original symobls, clear, end of information.
  ut_list_resize(self->codes, self->n_symbols + 2);
}

void ut_lzw_dictionary_append(UtObject *object, uint16_t code, uint8_t b) {
  assert(ut_object_is_lzw_dictionary(object));
  UtLzwDictionary *self = (UtLzwDictionary *)object;

  if (ut_list_get_length(self->codes) >= self->max_length) {
    return;
  }

  UtObject *entry = ut_object_list_get_element(self->codes, code);
  UtObjectRef new_entry = ut_list_copy(entry);
  ut_uint8_list_append(new_entry, b);
  ut_list_append(self->codes, new_entry);
}

bool ut_object_is_lzw_dictionary(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
