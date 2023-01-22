#include <assert.h>
#include <stdlib.h>

#include "ut-huffman-decoder.h"
#include "ut.h"

typedef struct {
  UtObject object;
  uint16_t *code_table_data;
  uint16_t **code_tables;
  size_t min_code_width;
  size_t max_code_width;
} UtHuffmanDecoder;

typedef struct {
  size_t parent;
  size_t child0, child1;
  double weight;
} Node;

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

UtObject *ut_huffman_decoder_new(UtObject *symbols, UtObject *symbol_weights) {
  UtObject *object = ut_object_new(sizeof(UtHuffmanDecoder), &object_interface);
  UtHuffmanDecoder *self = (UtHuffmanDecoder *)object;

  size_t n_symbols = ut_list_get_length(symbols);
  assert(ut_list_get_length(symbol_weights) == n_symbols);

  Node *nodes = malloc(sizeof(Node) * (n_symbols * 2 - 1));
  size_t n_nodes = 0;
  for (size_t i = 0; i < n_symbols; i++) {
    double weight = ut_float64_list_get_element(symbol_weights, i);
    // Ignore zero weighted symbols - these don't need to be encoded.
    if (weight == 0) {
      continue;
    }

    nodes[n_nodes].parent = -1;
    nodes[n_nodes].child0 = -1;
    nodes[n_nodes].child1 = -1;
    nodes[n_nodes].weight = weight;
    n_nodes++;
  }

  // Build binary tree by combining the two nodes with the smallest weights.
  for (size_t i = 0; i < n_symbols - 1; i++) {
    size_t smallest_node = -1;
    for (size_t j = 0; j < n_nodes; j++) {
      if (nodes[j].parent == -1 &&
          (smallest_node == -1 ||
           nodes[j].weight < nodes[smallest_node].weight)) {
        smallest_node = j;
      }
    }
    size_t second_smallest_node = -1;
    for (size_t j = 0; j < n_nodes; j++) {
      if (nodes[j].parent == -1 && j != smallest_node &&
          (second_smallest_node == -1 ||
           nodes[j].weight < nodes[second_smallest_node].weight)) {
        second_smallest_node = j;
      }
    }

    size_t parent_node = n_nodes;
    n_nodes++;

    // Create a new node that combines these two smallest weights.
    nodes[parent_node].parent = -1;
    nodes[parent_node].child0 = smallest_node;
    nodes[parent_node].child1 = second_smallest_node;
    nodes[parent_node].weight =
        nodes[smallest_node].weight + nodes[second_smallest_node].weight;

    // Link the smallest weights to the new node.
    nodes[smallest_node].parent = parent_node;
    nodes[second_smallest_node].parent = parent_node;
  }

  // Read codes from tree.
  uint16_t codes[n_symbols];
  size_t code_widths[n_symbols];
  self->min_code_width = 17;
  self->max_code_width = 0;
  for (size_t i = 0; i < n_symbols; i++) {
    size_t n = i;
    codes[i] = 0;
    code_widths[i] = 0;
    while (nodes[n].parent != -1) {
      size_t p = nodes[n].parent;
      if (nodes[p].child1 == n) {
        codes[i] |= 0x1 << code_widths[i];
      }
      code_widths[i]++;
      n = p;

      if (code_widths[i] > self->max_code_width) {
        self->max_code_width = code_widths[i];
      }
      if (code_widths[i] < self->min_code_width) {
        self->min_code_width = code_widths[i];
      }
    }
  }
  free(nodes);

  // Populate mapping tables.
  allocate_tables(self);
  for (size_t code_width = 1; code_width <= self->max_code_width;
       code_width++) {
    uint16_t *code_table = self->code_tables[code_width - 1];
    for (size_t i = 0; i < n_symbols; i++) {
      uint16_t symbol = ut_uint16_list_get_element(symbols, i);
      if (code_width == code_widths[i]) {
        code_table[codes[i]] = symbol;
      }
    }
  }

  return object;
}

UtObject *ut_huffman_decoder_new_canonical(UtObject *symbols,
                                           UtObject *code_widths) {
  UtObject *object = ut_object_new(sizeof(UtHuffmanDecoder), &object_interface);
  UtHuffmanDecoder *self = (UtHuffmanDecoder *)object;

  size_t n_symbols = ut_list_get_length(symbols);
  assert(ut_list_get_length(code_widths) == n_symbols);

  // Calculate the longest length code.
  self->min_code_width = 17;
  self->max_code_width = 0;
  for (size_t i = 0; i < n_symbols; i++) {
    uint8_t code_width = ut_uint8_list_get_element(code_widths, i);
    assert(code_width <= 16);
    if (code_width != 0 && code_width < self->min_code_width) {
      self->min_code_width = code_width;
    }
    if (code_width > self->max_code_width) {
      self->max_code_width = code_width;
    }
  }

  // Populate mapping tables.
  allocate_tables(self);
  uint16_t code = 0;
  for (size_t code_width = 1; code_width <= self->max_code_width;
       code_width++) {
    uint16_t *code_table = self->code_tables[code_width - 1];
    for (size_t i = 0; i < n_symbols; i++) {
      uint16_t symbol = ut_uint16_list_get_element(symbols, i);
      if (code_width == ut_uint8_list_get_element(code_widths, i)) {
        code_table[code] = symbol;
        code++;
        // FIXME: Check if have run out of codes
      }
    }
    code <<= 1;
  }

  return object;
}

size_t ut_huffman_decoder_get_min_code_width(UtObject *object) {
  assert(ut_object_is_huffman_decoder(object));
  UtHuffmanDecoder *self = (UtHuffmanDecoder *)object;
  return self->min_code_width;
}

size_t ut_huffman_decoder_get_max_code_width(UtObject *object) {
  assert(ut_object_is_huffman_decoder(object));
  UtHuffmanDecoder *self = (UtHuffmanDecoder *)object;
  return self->max_code_width;
}

bool ut_huffman_decoder_get_symbol(UtObject *object, uint16_t code,
                                   size_t code_width, uint16_t *symbol) {
  assert(ut_object_is_huffman_decoder(object));
  UtHuffmanDecoder *self = (UtHuffmanDecoder *)object;

  assert(code_width > 0);
  if (code_width > self->max_code_width) {
    return false;
  }
  assert(code < 1 << code_width);

  uint16_t *code_table = self->code_tables[code_width - 1];
  if (code_table == NULL) {
    return false;
  }
  uint16_t symbol_ = code_table[code];
  if (symbol_ == 65535) {
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
