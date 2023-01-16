#include "ut-huffman-encoder.h"
#include "ut.h"

static UtObject *message_to_symbols(const char *symbols, const char *message) {
  UtObjectRef message_symbols = ut_uint16_list_new();
  for (size_t i = 0; message[i] != '\0'; i++) {
    for (size_t j = 0; symbols[j] != '\0'; j++) {
      if (message[i] == symbols[j]) {
        ut_uint16_list_append(message_symbols, j);
      }
    }
  }

  return ut_object_ref(message_symbols);
}

static UtObject *encode(UtObject *encoder, const char *symbols,
                        const char *message) {
  UtObjectRef message_symbols = message_to_symbols(symbols, message);
  size_t symbols_length = ut_list_get_length(message_symbols);
  UtObjectRef bits = ut_uint8_list_new();
  for (size_t i = 0; i < symbols_length; i++) {
    uint16_t symbol = ut_uint16_list_get_element(message_symbols, i);
    uint16_t code;
    size_t code_width;
    ut_huffman_encoder_get_code(encoder, symbol, &code, &code_width);

    for (size_t j = 0; j < code_width; j++) {
      uint16_t mask = 1 << (code_width - j - 1);
      uint8_t bit = (code & mask) != 0 ? 1 : 0;
      ut_uint8_list_append(bits, bit);
    }
  }

  return ut_object_ref(bits);
}

static void test_encode() {
  // This is an example from https://en.wikipedia.org/wiki/Huffman_coding
  const char *symbols = "CBE_DA";
  UtObjectRef symbol_weights = ut_float64_list_new_from_elements(
      6, 2.0, 6.0, 7.0, 10.0, 10.0, 10.0, 11.0);
  UtObjectRef encoder = ut_huffman_encoder_new(symbol_weights);

  const char *message = "A_DEAD_DAD_CEDED_A_BAD_BABE_A_BEADED_ABACA_BED";
  UtObjectRef bits = encode(encoder, symbols, message);

  uint8_t expected_bits[115] = {
      1, 0,       // 'A'
      0, 0,       // '_'
      0, 1,       // 'D'
      1, 1, 0,    // 'E'
      1, 0,       // 'A'
      0, 1,       // 'D'
      0, 0,       // '_'
      0, 1,       // 'D'
      1, 0,       // 'A'
      0, 1,       // 'D'
      0, 0,       // '_'
      1, 1, 1, 0, // 'C'
      1, 1, 0,    // 'E'
      0, 1,       // 'D'
      1, 1, 0,    // 'E'
      0, 1,       // 'D'
      0, 0,       // '_'
      1, 0,       // 'A'
      0, 0,       // '_'
      1, 1, 1, 1, // 'B'
      1, 0,       // 'A'
      0, 1,       // 'D'
      0, 0,       // '_'
      1, 1, 1, 1, // 'B'
      1, 0,       // 'A'
      1, 1, 1, 1, // 'B'
      1, 1, 0,    // 'E'
      0, 0,       // '_'
      1, 0,       // 'A'
      0, 0,       // '_'
      1, 1, 1, 1, // 'B'
      1, 1, 0,    // 'E'
      1, 0,       // 'A'
      0, 1,       // 'D'
      1, 1, 0,    // 'E'
      0, 1,       // 'D'
      0, 0,       // '_'
      1, 0,       // 'A'
      1, 1, 1, 1, // 'B'
      1, 0,       // 'A'
      1, 1, 1, 0, // 'C'
      1, 0,       // 'A'
      0, 0,       // '_'
      1, 1, 1, 1, // 'B'
      1, 1, 0,    // 'E'
      0, 1        // 'D'
  };
  ut_assert_uint8_list_equal(bits, expected_bits, 115);
}

static void test_encode_canonical() {
  // This is an example from https://en.wikipedia.org/wiki/Huffman_coding
  const char *symbols = " aefhimnstloprux";
  UtObjectRef code_widths = ut_uint8_list_new_from_elements(
      16, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5);
  UtObjectRef encoder = ut_huffman_encoder_new_canonical(code_widths);

  const char *message = "this is an example of a huffman tree";
  UtObjectRef bits = encode(encoder, symbols, message);

  uint8_t expected_bits[135] = {
      1, 1, 0, 0,    // 't'
      0, 1, 1, 1,    // 'h'
      1, 0, 0, 0,    // 'i'
      1, 0, 1, 1,    // 's'
      0, 0, 0,       // ' '
      1, 0, 0, 0,    // 'i'
      1, 0, 1, 1,    // 's'
      0, 0, 0,       // ' '
      0, 0, 1,       // 'a'
      1, 0, 1, 0,    // 'n'
      0, 0, 0,       // ' '
      0, 1, 0,       // 'e'
      1, 1, 1, 1, 1, // 'x'
      0, 0, 1,       // 'a'
      1, 0, 0, 1,    // 'm'
      1, 1, 1, 0, 0, // 'p'
      1, 1, 0, 1, 0, // 'l'
      0, 1, 0,       // 'e'
      0, 0, 0,       // ' '
      1, 1, 0, 1, 1, // 'o'
      0, 1, 1, 0,    // 'f'
      0, 0, 0,       // ' '
      0, 0, 1,       // 'a'
      0, 0, 0,       // ' '
      0, 1, 1, 1,    // 'h'
      1, 1, 1, 1, 0, // 'u'
      0, 1, 1, 0,    // 'f'
      0, 1, 1, 0,    // 'f'
      1, 0, 0, 1,    // 'm'
      0, 0, 1,       // 'a'
      1, 0, 1, 0,    // 'n'
      0, 0, 0,       // ' '
      1, 1, 0, 0,    // 't'
      1, 1, 1, 0, 1, // 'r'
      0, 1, 0,       // 'e'
      0, 1, 0        // 'e'
  };
  ut_assert_uint8_list_equal(bits, expected_bits, 135);
}

int main(int argc, char **argv) {
  test_encode();
  test_encode_canonical();
}
