#include <assert.h>
#include <stdlib.h>

#include "ut-huffman-code.h"
#include "ut.h"

// Binary tree node.
typedef struct {
  // Index of parent node.
  size_t parent;

  // Index of two children.
  size_t child0, child1;

  // Symbol weighting.
  double weight;
} Node;

void ut_huffman_code_generate(UtObject *symbol_weights, uint16_t *codes,
                              size_t *code_widths) {
  size_t symbols_length = ut_list_get_length(symbol_weights);

  // We only support up to 16 bit symbols.
  assert(symbols_length <= 0xffff);

  // Start tree with leaf nodes for all non-zero weighted elements.
  Node *nodes = malloc(sizeof(Node) * (symbols_length * 2 - 1));
  size_t n_nodes = 0;
  for (size_t i = 0; i < symbols_length; i++) {
    double weight = ut_float64_list_get_element(symbol_weights, i);
    nodes[n_nodes].parent = -1;
    nodes[n_nodes].child0 = -1;
    nodes[n_nodes].child1 = -1;
    nodes[n_nodes].weight = weight;
    n_nodes++;
  }

  // Build binary tree by combining the two nodes with the smallest weights.
  for (size_t i = 0; i < symbols_length - 1; i++) {
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
  for (size_t i = 0; i < symbols_length; i++) {
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
    }
  }
  free(nodes);
}

bool ut_huffman_code_generate_canonical(UtObject *code_widths,
                                        uint16_t *codes) {
  size_t symbols_length = ut_list_get_length(code_widths);

  // Skip unused symbols.
  size_t n_used = 0;
  for (size_t i = 0; i < symbols_length; i++) {
    if (ut_uint8_list_get_element(code_widths, i) == 0) {
      codes[i] = 0;
      n_used++;
    }
  }

  // Special case - if only one symbol, then we have one code for this (0) and
  // one unused code (1). In this case we expect an en/decoder to fail if it
  // attempts to use this unused code.
  if (n_used == symbols_length - 1 &&
      ut_uint8_list_get_element(code_widths, 0) == 1) {
    codes[0] = 0;
    return true;
  }

  // Populate mapping tables.
  uint16_t code = 0;
  uint16_t last_code = 1;
  bool complete = false;
  for (size_t code_width = 1; n_used < symbols_length; code_width++) {
    // Run out of bits.
    if (complete) {
      return false;
    }

    for (size_t i = 0; i < symbols_length; i++) {
      uint8_t code_width_ = ut_uint8_list_get_element(code_widths, i);
      // We only support up to 16 bit codes.
      if (code_width_ > 16) {
        return false;
      }

      if (code_width == code_width_) {
        // Used all the bits for this width - this must be the last code.
        if (code == last_code) {
          complete = true;
        }

        codes[i] = code;
        code++;
        n_used++;
      }
    }
    code <<= 1;
    last_code = last_code << 1 | 0x1;
  }

  // Successful if used all code bits.
  return complete;
}
