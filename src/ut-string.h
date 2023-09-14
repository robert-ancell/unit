#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef struct {
  bool is_mutable;
  const char *(*get_text)(UtObject *object);
  char *(*take_text)(UtObject *object);
  UtObject *(*get_utf8)(UtObject *object);
  void (*clear)(UtObject *object);
  void (*prepend)(UtObject *object, const char *text);
  void (*prepend_code_point)(UtObject *object, uint32_t code_point);
  void (*append)(UtObject *object, const char *text, size_t length);
  void (*append_code_point)(UtObject *object, uint32_t code_point);
} UtStringInterface;

extern int ut_string_id;

/// Creates a new string that contains [text].
/// [text] is copied.
///
/// !return-type UtString
/// !return-ref
UtObject *ut_string_new(const char *text);

/// Creates a new string using printf [format].
///
/// !return-type UtString
/// !return-ref
UtObject *ut_string_new_printf(const char *format, ...)
    __attribute__((format(printf, 1, 2)));

/// Creates a new string that contains [text].
/// [text] is not copied and must remain for the lifetime of this object.
///
/// !return-type UtString
/// !return-ref
UtObject *ut_string_new_constant(const char *text);

/// Creates a new string that contains the first [length] characters
/// of [text].
///
/// !return-type UtString
/// !return-ref
UtObject *ut_string_new_sized(const char *text, size_t length);

/// Creates a new string from UTF-8 encoded data in [utf8].
///
/// !arg-type utf8 Uint8List
/// !return-type UtString
/// !return-ref
UtObject *ut_string_new_from_utf8(UtObject *utf8);

/// Creates a new string from UTF-16 [code_units].
///
/// !arg-type code_units Uint16List
/// !return-type UtString
/// !return-ref
UtObject *ut_string_new_from_utf16(UtObject *code_units);

/// Creates a new string from ASCII encoded [characters].
///
/// !arg-type characters Uint8List
/// !return-type UtString
/// !return-ref
UtObject *ut_string_new_from_ascii(UtObject *characters);

/// Creates a new string from ISO 8859-1 encoded [characters].
///
/// !arg-type characters Uint8List
/// !return-type UtString
/// !return-ref
UtObject *ut_string_new_from_iso_8859_1(UtObject *characters);

/// Returns the text inside [object].
const char *ut_string_get_text(UtObject *object);

/// Returns the text inside [object].
/// The returned text is owned by the caller and [object] is now an empty text.
///
/// !return-ref
char *ut_string_take_text(UtObject *object);

/// Generate the Unicode code points of [object].
///
/// !return-type UtUint32List
/// !return-ref
UtObject *ut_string_get_code_points(UtObject *object);

/// Generates a UTF-8 encoding of [object].
///
/// !return-type UtUint8List
/// !return-ref
UtObject *ut_string_get_utf8(UtObject *object);

/// Generates a UTF-16 encoding of [object].
///
/// !return-type UtUint16List
/// !return-ref
UtObject *ut_string_get_utf16(UtObject *object);

/// Generates an ASCII encoding of [object].
///
/// !return-type UtUint8List
/// !return-ref
UtObject *ut_string_get_ascii(UtObject *object);

/// Generates an ISO 8859-1 encoding of [object].
///
/// !return-type UtUint8List
/// !return-ref
UtObject *ut_string_get_iso_8859_1(UtObject *object);

/// Returns [true] if [object] can be modified.
bool ut_string_is_mutable(UtObject *object);

/// Set [object] to an empty string.
void ut_string_clear(UtObject *object);

/// Adds [text] to the end of [object].
void ut_string_prepend(UtObject *object, const char *text);

/// Adds printf [format] text to the start of [object].
void ut_string_prepend_printf(UtObject *object, const char *format, ...)
    __attribute__((format(printf, 2, 3)));

/// Adds the Unicode [code_point] to the start of [object].
void ut_string_prepend_code_point(UtObject *object, uint32_t code_point);

/// Adds [text] to the start of [object].
void ut_string_append(UtObject *object, const char *text);

/// Adds the first [length] characters of [text] to the start of [object].
void ut_string_append_sized(UtObject *object, const char *text, size_t length);

/// Adds printf [format] text to the end of [object].
void ut_string_append_printf(UtObject *object, const char *format, ...)
    __attribute__((format(printf, 2, 3)));

/// Adds the Unicode [code_point] to the end of [object].
void ut_string_append_code_point(UtObject *object, uint32_t code_point);

/// Returns [true] if [object] is a [UtString].
bool ut_object_implements_string(UtObject *object);
