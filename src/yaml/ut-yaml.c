#include "ut.h"

typedef enum {
  PARENT_TYPE_NONE,
  PARENT_TYPE_SEQUENCE,
  PARENT_TYPE_MAPPING
} ParentType;

static UtObject *decode_node(const char *text, size_t *offset,
                             ParentType parent_type, size_t parent_indent);

static bool is_whitespace(char c) { return c == ' ' || c == '\t'; }

static bool is_newline(char c) { return c == '\n'; }

static bool is_whitespace_or_newline(char c) {
  return is_whitespace(c) || is_newline(c);
}

// Returns the number of characters the current offset is from the start of the
// line.
static size_t get_indent(const char *text, size_t offset) {
  if (offset == 0) {
    return 0;
  }

  if (text[offset] == '\n') {
    offset--;
  }

  size_t indent = 0;
  while (offset > 0) {
    if (text[offset - 1] == '\n') {
      return indent;
    }
    offset--;
    indent++;
  }

  return 0;
}

static bool get_quoted(const char *text, size_t *offset) {
  if (text[*offset] == '"') {
    while (text[*offset] != '\0' && text[*offset] != '"') {
      (*offset)++;
    }

    if (text[*offset] != '"') {
      // ...
    }
    (*offset)++;
    return true;
  } else if (text[*offset] == '\'') {
    while (text[*offset] != '\0' && text[*offset] != '\'') {
      (*offset)++;
    }

    if (text[*offset] != '\'') {
      // ...
    }
    (*offset)++;
    return true;
  } else {
    return false;
  }
}

static bool is_sequence_start(const char *text, size_t offset) {
  return text[offset] == '-' && (is_whitespace_or_newline(text[offset + 1]) ||
                                 text[offset + 1] == '\0');
}

static bool get_next_sequence_item(const char *text, size_t *offset,
                                   size_t indent) {
  size_t o = *offset;

  // Skip leading whitespace
  while (text[o] != '\0' && is_whitespace_or_newline(text[o])) {
    o++;
  }

  if (!is_sequence_start(text, o) || get_indent(text, o) != indent) {
    return false;
  }

  *offset = o + 1;
  return true;
}

static bool get_next_mapping_item(const char *text, size_t *offset,
                                  size_t indent) {
  size_t o = *offset;

  // Skip leading whitespace
  while (text[o] != '\0' && is_whitespace_or_newline(text[o])) {
    o++;
  }

  if (get_indent(text, o) != indent) {
    return false;
  }

  if (text[o] == '\0') {
    return false;
  }

  *offset = o;
  return true;
}

// FIXME: Replace ' \n ' with ' '
// FIXME: Translate quoted values
static UtObject *get_node_value(const char *text, size_t length) {
  // Skip leading whitespace
  size_t start = 0;
  while (length > 0 && is_whitespace_or_newline(text[start])) {
    start++;
    length--;
  }

  // Skip trailing whitespace
  while (length > 0 && is_whitespace_or_newline(text[start + length - 1])) {
    length--;
  }

  return ut_string_new_sized(text + start, length);
}

static bool get_first_word(const char *text, size_t *offset) {
  bool quoted = get_quoted(text, offset);
  if (!quoted) {
    // Read the first word
    // FIXME: Only allow scalar/mapping key characters
    while (text[*offset] != '\0' && !is_whitespace(text[*offset]) &&
           text[*offset] != ':' && !is_newline(text[*offset])) {
      (*offset)++;
    }
  }

  // Skip trailing whitespace
  while (text[*offset] != '\0' && is_whitespace(text[*offset])) {
    (*offset)++;
  }

  return quoted;
}

static UtObject *decode_node(const char *text, size_t *offset,
                             ParentType parent_type, size_t parent_indent) {
  printf("decode_node %d(%zi) '%s'\n", parent_type, parent_indent,
         text + *offset);

  size_t start = *offset;

  // Skip leading whitespace / newlines
  while (text[*offset] != '\0' && (is_whitespace_or_newline(text[*offset]))) {
    (*offset)++;
  }
  size_t indent = get_indent(text, *offset);

  // Empty sequence item
  size_t o = *offset;
  if (parent_type == PARENT_TYPE_SEQUENCE &&
      get_next_sequence_item(text, &o, parent_indent)) {
    return get_node_value(text + start, *offset - start);
  }

  // Sequence
  if (is_sequence_start(text, *offset)) {
    UtObjectRef sequence = ut_list_new();
    while (get_next_sequence_item(text, offset, indent)) {
      ut_list_append_take(
          sequence, decode_node(text, offset, PARENT_TYPE_SEQUENCE, indent));
    }

    return ut_object_ref(sequence);
  }

  bool quoted = get_first_word(text, offset);

  // Check if mapping key
  // FIXME: Abort if parent is map of the same indent
  if (text[*offset] == ':') {
    UtObjectRef key = get_node_value(text + start, *offset - start);
    (*offset)++;
    UtObjectRef value = decode_node(text, offset, PARENT_TYPE_MAPPING, indent);

    if (parent_type == PARENT_TYPE_MAPPING) {
      return ut_object_ref(value);
    }

    UtObjectRef mapping = ut_map_new();
    ut_map_insert(mapping, key, value);
    while (get_next_mapping_item(text, offset, indent)) {
      size_t key2_start = *offset;
      size_t o = *offset;
      get_first_word(text, &o);
      if (text[o] != ':') {
        break;
      }
      *offset = o;
      UtObjectRef key2 =
          get_node_value(text + key2_start, *offset - key2_start);
      (*offset)++;
      UtObjectRef value2 =
          decode_node(text, offset, PARENT_TYPE_MAPPING, indent);
      ut_map_insert(mapping, key2, value2);
    }

    return ut_object_ref(mapping);
  }

  // Read remaining data
  if (!quoted) {
    while (text[*offset] != '\0') {
      // If newline, check enough indent.

      size_t o = *offset;
      if (parent_type == PARENT_TYPE_SEQUENCE &&
          get_next_sequence_item(text, &o, parent_indent)) {
        break;
      }
      if (parent_type == PARENT_TYPE_MAPPING &&
          get_next_mapping_item(text, &o, parent_indent)) {
        break;
      }

      (*offset)++;
    }
  }

  return get_node_value(text + start, *offset - start);
}

UtObject *ut_yaml_decode(const char *text) {
  printf("yaml_decode:\n---\n%s\n---\n", text);
  UtObjectRef documents = ut_list_new();

  size_t offset = 0;
  ut_list_append_take(documents,
                      decode_node(text, &offset, PARENT_TYPE_NONE, 0));

  return ut_object_ref(documents);
}
