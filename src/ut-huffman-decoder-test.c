#include "ut-huffman-decoder.h"
#include "ut.h"

int main(int argc, char **argv) {
  // This is the example from https://en.wikipedia.org/wiki/Huffman_coding but
  // converted to a canonical code.
  UtObjectRef symbols = ut_uint16_list_new_from_elements(
      16, ' ', 'a', 'e', 'f', 'h', 'i', 'm', 'n', 's', 't', 'l', 'o', 'p', 'r',
      'u', 'x');
  UtObjectRef code_widths = ut_uint8_list_new_from_elements(
      16, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5);
  UtObjectRef bits = ut_uint8_list_new_from_elements(135, 1, 1, 0, 0, // 't'
                                                     0, 1, 1, 1,      // 'h'
                                                     1, 0, 0, 0,      // 'i'
                                                     1, 0, 1, 1,      // 's'
                                                     0, 0, 0,         // ' '
                                                     1, 0, 0, 0,      // 'i'
                                                     1, 0, 1, 1,      // 's'
                                                     0, 0, 0,         // ' '
                                                     0, 0, 1,         // 'a'
                                                     1, 0, 1, 0,      // 'n'
                                                     0, 0, 0,         // ' '
                                                     0, 1, 0,         // 'e'
                                                     1, 1, 1, 1, 1,   // 'x'
                                                     0, 0, 1,         // 'a'
                                                     1, 0, 0, 1,      // 'm'
                                                     1, 1, 1, 0, 0,   // 'p'
                                                     1, 1, 0, 1, 0,   // 'l'
                                                     0, 1, 0,         // 'e'
                                                     0, 0, 0,         // ' '
                                                     1, 1, 0, 1, 1,   // 'o'
                                                     0, 1, 1, 0,      // 'f'
                                                     0, 0, 0,         // ' '
                                                     0, 0, 1,         // 'a'
                                                     0, 0, 0,         // ' '
                                                     0, 1, 1, 1,      // 'h'
                                                     1, 1, 1, 1, 0,   // 'u'
                                                     0, 1, 1, 0,      // 'f'
                                                     0, 1, 1, 0,      // 'f'
                                                     1, 0, 0, 1,      // 'm'
                                                     0, 0, 1,         // 'a'
                                                     1, 0, 1, 0,      // 'n'
                                                     0, 0, 0,         // ' '
                                                     1, 1, 0, 0,      // 't'
                                                     1, 1, 1, 0, 1,   // 'r'
                                                     0, 1, 0,         // 'e'
                                                     0, 1, 0          // 'e'
  );

  UtObjectRef decoder = ut_huffman_decoder_new_canonical(symbols, code_widths);

  uint16_t code = 0;
  size_t code_width = 0;
  UtObjectRef text = ut_string_new("");
  size_t bits_length = ut_list_get_length(bits);
  for (size_t i = 0; i < bits_length; i++) {
    code = code << 1 | ut_uint8_list_get_element(bits, i);
    code_width++;

    uint16_t symbol;
    if (ut_huffman_decoder_get_symbol(decoder, code, code_width, &symbol)) {
      ut_string_append_code_point(text, symbol);
      code = 0;
      code_width = 0;
    }
  }

  ut_assert_cstring_equal(ut_string_get_text(text),
                          "this is an example of a huffman tree");
}
