#include <stdarg.h>
#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  const char *(*get_element)(UtObject *object, size_t index);
  char **(*take_data)(UtObject *object);
  void (*insert)(UtObject *object, size_t index, const char *value);
} UtStringListInterface;

extern int ut_string_list_id;

/// Creates a new list that contains strings.
/// This creates a [UtStringrray].
///
/// !return-ref
/// !return-type UtStringList
UtObject *ut_string_list_new();

/// Creates a new list from the [NULL] terminated list of strings [value].
///
/// !return-ref
/// !return-type UtStringList
UtObject *ut_string_list_new_from_elements(const char *value, ...);

/// Returns the value at [index] of this list.
const char *ut_string_list_get_element(UtObject *object, size_t index);

/// Returns the memory containing the contents of this list.
/// The list will be empty after this call.
/// If the list supports it, this data will be the data that was used in the
/// list - no copy will be performed. When no longer required used [free] to
/// release the memory.
char **ut_string_list_take_data(UtObject *object);

/// Add [value] to the end of this list.
void ut_string_list_append(UtObject *object, const char *value);

/// Make printf [format] text and add it to the end of this list.
void ut_string_list_append_printf(UtObject *object, const char *format, ...)
    __attribute__((format(printf, 2, 3)));

/// Add [value] to the start of this list.
void ut_string_list_prepend(UtObject *object, const char *value);

/// Make printf [format] text and add it to the start of this list.
void ut_string_list_prepend_printf(UtObject *object, const char *format, ...)
    __attribute__((format(printf, 2, 3)));

/// Insert [value] in this list at position [index]. The remaining elements will
/// be moved down.
void ut_string_list_insert(UtObject *object, size_t index, const char *value);

/// Return a string containing the elements in this list with joined by
/// [separator]. e.g. "one", "two", "three" and separator "," will make
/// "one,two,three".
char *ut_string_list_join(UtObject *object, const char *separator);

/// Returns [true] if [object] is a [UtStringList].
bool ut_object_implements_string_list(UtObject *object);
