#include <assert.h>
#include <stdlib.h>

#include "ut-huffman-code.h"
#include "ut.h"

typedef struct {
  UtObject object;
  uint16_t *code_table_data;
  uint16_t **code_tables;
  size_t max_code_width;
} UtHuffmanDecoder;

static void allocate_tables(UtHuffmanDecoder *self) {
  size_t code_table_data_length = 1 << (self->max_code_width + 1);
  self->code_table_data = malloc(sizeof(uint16_t) * code_table_data_length);
  for (size_t i = 0; i < code_table_data_length; i++) {
    self->code_table_data[i] = 65535;
  }
  self->code_tables = malloc(sizeof(uint16_t *) * self->max_code_width);
  size_t offset = 0;
  for (size_t code_width = 1; code_width <= self->max_code_width;
       code_width++) {
    self->code_tables[code_width - 1] = self->code_table_data + offset;
    size_t code_table_length = 1 << code_width;
    offset += code_table_length;
  }
}

static void ut_huffman_decoder_cleanup(UtObject *object) {
  UtHuffmanDecoder *self = (UtHuffmanDecoder *)object;
  free(self->code_table_data);
  free(self->code_tables);
}

static UtObjectInterface object_interface = {.type_name = "UtHuffmanDecoder",
                                             .cleanup =
                                                 ut_huffman_decoder_cleanup,
                                             .interfaces = {{NULL, NULL}}};

static UtObject *create_decoder(size_t symbols_length, uint16_t *codes,
                                size_t *code_widths) {
  UtObject *object = ut_object_new(sizeof(UtHuffmanDecoder), &object_interface);
  UtHuffmanDecoder *self = (UtHuffmanDecoder *)object;

  self->max_code_width = 0;
  for (size_t i = 0; i < symbols_length; i++) {
    uint8_t code_width = code_widths[i];
    assert(code_width <= 16);
    if (code_width > self->max_code_width) {
      self->max_code_width = code_width;
    }
  }

  // Populate mapping tables.
  allocate_tables(self);
  for (size_t code_width = 1; code_width <= self->max_code_width;
       code_width++) {
    uint16_t *code_table = self->code_tables[code_width - 1];
    for (size_t i = 0; i < symbols_length; i++) {
      if (code_width == code_widths[i]) {
        code_table[codes[i]] = i;
      }
    }
  }

  return object;
}

UtObject *ut_huffman_decoder_new(UtObject *symbol_weights) {
  size_t symbols_length = ut_list_get_length(symbol_weights);

  uint16_t codes[symbols_length];
  size_t code_widths[symbols_length];
  ut_huffman_code_generate(symbol_weights, codes, code_widths);

  return create_decoder(symbols_length, codes, code_widths);
}

UtObject *ut_huffman_decoder_new_canonical(UtObject *code_widths) {
  size_t symbols_length = ut_list_get_length(code_widths);

  uint16_t codes[symbols_length];
  if (!ut_huffman_code_generate_canonical(code_widths, codes)) {
    return ut_general_error_new("Invalid Huffman code widths");
  }

  size_t code_widths_[symbols_length];
  for (size_t i = 0; i < symbols_length; i++) {
    code_widths_[i] = ut_uint8_list_get_element(code_widths, i);
  }

  return create_decoder(symbols_length, codes, code_widths_);
}

bool ut_huffman_decoder_get_symbol(UtObject *object, uint16_t code,
                                   size_t code_width, uint16_t *symbol) {
  assert(ut_object_is_huffman_decoder(object));
  UtHuffmanDecoder *self = (UtHuffmanDecoder *)object;

  assert(code_width > 0);
  assert(code < 1 << code_width);

  // Have passed all the valid codes, an error has occurred.
  if (code_width > self->max_code_width) {
    *symbol = 65535;
    return true;
  }

  uint16_t *code_table = self->code_tables[code_width - 1];
  if (code_table == NULL) {
    return false;
  }
  uint16_t symbol_ = code_table[code];
  if (symbol_ == 65535) {
    // Failed to find a code, this only occurs for a code with a single symbol
    // (has one unused code).
    if (code_width == self->max_code_width) {
      *symbol = 65535;
      return true;
    }
    return false;
  }

  if (symbol != NULL) {
    *symbol = symbol_;
  }
  return true;
}

bool ut_object_is_huffman_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
