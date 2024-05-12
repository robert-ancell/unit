#include "ut.h"

static UtObject *decode_node(const char *text, size_t *offset, size_t indent);

static bool is_whitespace(char c) { return c == ' ' || c == '\t'; }

static bool is_newline(char c) { return c == '\n'; }

static bool is_comment(char c) { return c == '#'; }

static bool indent_matches(const char *text, size_t *offset, size_t indent) {
  for (size_t i = 0; i < indent; i++) {
    if (text[*offset + i] != ' ') {
      return false;
    }
  }
  if (text[*offset + indent] == ' ') {
    return false;
  }

  *offset += indent;
  return true;
}

static bool decode_sequence_item(const char *text, size_t *offset,
                                 size_t indent, UtObject **value) {
  size_t o = *offset;
  if (!indent_matches(text, &o, indent)) {
    return false;
  }
  if (text[o] != '-') {
    return false;
  }
  o++;

  *value = decode_node(text, &o, indent + (o - *offset));
  *offset = o;
  return true;
}

static bool decode_mapping_item(const char *text, size_t *offset, size_t indent,
                                char **key, UtObject **value) {
  size_t o = *offset;
  if (!indent_matches(text, &o, indent)) {
    return false;
  }

  size_t key_start = o;
  while (text[o] != '\0' && !is_newline(text[o]) && !is_comment(text[o]) &&
         text[o] != ':') {
    o++;
  }
  size_t key_end = o;

  if (text[o] != ':') {
    return false;
  }
  o++;

  UtObjectRef v = decode_node(text, &o, indent + (o - *offset));
  if (v == NULL) {
    return false;
  }

  *offset = o;
  *key = ut_cstring_new_sized(text + key_start, key_end - key_start);
  *value = ut_object_ref(v);
  return true;
}

static UtObject *decode_literal(const char *text, size_t *offset) {
  return NULL;
}

static UtObject *decode_folded(const char *text, size_t *offset) {
  return NULL;
}

// FIXME: Use line_start instead of indent?
static UtObject *decode_node(const char *text, size_t *offset, size_t indent) {
  size_t start = *offset;

  // Skip leading whitespace.
  while (is_whitespace(text[*offset]) || text[*offset] == '\n') {
    if (text[*offset] == '\n') {
      start = *offset + 1;
      indent = 0;
    }
    (*offset)++;
  }

  size_t node_start = *offset;
  UtObjectRef node_value = NULL;
  if (text[*offset] == '|') {
    (*offset)++;
    return decode_literal(text, offset);
  } else if (text[*offset] == '>') {
    (*offset)++;
    return decode_folded(text, offset);
  } else if (text[*offset] == '"') {
    // FIXME
    ut_assert_true(false);
  } else if (text[*offset] == '\'') {
    // FIXME
    ut_assert_true(false);
  } else if (text[*offset] == '-') {
    size_t child_indent = indent + (*offset - start);
    (*offset)++;
    UtObjectRef value = decode_node(text, offset, indent + (*offset - start));
    UtObjectRef sequence = ut_list_new();
    ut_list_append(sequence, value);
    UtObject *item;
    while (decode_sequence_item(text, offset, child_indent, &item)) {
      ut_list_append(sequence, item);
    }
    return ut_object_ref(sequence);
  } else {
    while (text[*offset] != '\0' && !is_newline(text[*offset]) &&
           !is_comment(text[*offset]) && text[*offset] != ':') {
      (*offset)++;
    }
    size_t end = *offset;
    // Trim trailing whitespace.
    while (end > node_start && is_whitespace(text[end - 1])) {
      end--;
    }
    node_value = ut_string_new_sized(text + node_start, end - node_start);
  }

  if (text[*offset] == ':') {
    size_t key_start = node_start;
    size_t key_end = *offset;
    while (key_end > key_start && is_whitespace(text[key_end - 1])) {
      key_end--;
    }
    ut_cstring_ref key =
        ut_cstring_new_sized(text + key_start, key_end - key_start);
    (*offset)++;

    UtObjectRef value = decode_node(text, offset, indent + (*offset - start));

    UtObjectRef map = ut_map_new();
    ut_map_insert_string(map, key, value);
    char *k;
    UtObject *v;
    size_t child_indent = indent + (node_start - start);
    while (decode_mapping_item(text, offset, child_indent, &k, &v)) {
      ut_map_insert_string(map, k, v);
    }

    return ut_object_ref(map);
  }

  // Skip comments
  if (is_comment(text[*offset])) {
    while (text[*offset] != '\0' && !is_newline(text[*offset])) {
      (*offset)++;
    }
  }

  if (is_newline(text[*offset])) {
    (*offset)++;
  }

  return ut_object_ref(node_value);
}

UtObject *ut_yaml_decode(const char *text) {
  UtObjectRef documents = ut_list_new();

  size_t offset = 0;
  ut_list_append_take(documents, decode_node(text, &offset, 0));

  return ut_object_ref(documents);
}
