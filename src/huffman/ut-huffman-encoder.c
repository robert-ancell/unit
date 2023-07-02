#include <assert.h>
#include <stdlib.h>

#include "ut-huffman-code.h"
#include "ut.h"

typedef struct {
  UtObject object;
  uint16_t *codes;
  size_t codes_length;
  size_t *code_widths;
} UtHuffmanEncoder;

static void ut_huffman_encoder_cleanup(UtObject *object) {
  UtHuffmanEncoder *self = (UtHuffmanEncoder *)object;
  free(self->codes);
  free(self->code_widths);
}

static UtObjectInterface object_interface = {
    .type_name = "UtHuffmanEncoder", .cleanup = ut_huffman_encoder_cleanup};

static UtObject *create_encoder(size_t symbols_length) {
  UtObject *object = ut_object_new(sizeof(UtHuffmanEncoder), &object_interface);
  UtHuffmanEncoder *self = (UtHuffmanEncoder *)object;

  self->codes = malloc(sizeof(uint16_t) * symbols_length);
  self->codes_length = symbols_length;
  self->code_widths = malloc(sizeof(size_t) * symbols_length);

  return object;
}

UtObject *ut_huffman_encoder_new(UtObject *symbol_weights) {
  size_t symbols_length = ut_list_get_length(symbol_weights);

  UtObject *object = create_encoder(symbols_length);
  UtHuffmanEncoder *self = (UtHuffmanEncoder *)object;
  ut_huffman_code_generate(symbol_weights, self->codes, self->code_widths);

  return object;
}

UtObject *ut_huffman_encoder_new_canonical(UtObject *code_widths) {
  size_t symbols_length = ut_list_get_length(code_widths);

  UtObject *object = create_encoder(symbols_length);
  UtHuffmanEncoder *self = (UtHuffmanEncoder *)object;
  ut_huffman_code_generate_canonical(code_widths, self->codes);
  for (size_t i = 0; i < symbols_length; i++) {
    self->code_widths[i] = ut_uint8_list_get_element(code_widths, i);
  }

  return object;
}

void ut_huffman_encoder_get_code(UtObject *object, uint16_t symbol,
                                 uint16_t *code, size_t *code_width) {
  assert(ut_object_is_huffman_encoder(object));
  UtHuffmanEncoder *self = (UtHuffmanEncoder *)object;

  assert(symbol < self->codes_length);
  *code = self->codes[symbol];
  *code_width = self->code_widths[symbol];
}

bool ut_object_is_huffman_encoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
