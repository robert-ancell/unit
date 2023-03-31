#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#pragma once

/// Creates a new C string containing [value].
/// !return-ref
char *ut_cstring_new(const char *value);

/// Creates a new C string that copies [end] characters from [value] starting at
/// the [start] position.
///
/// !return-ref
char *ut_cstring_new_substring(const char *value, size_t start, size_t end);

/// Creates a new C string containing the first [length] characters from
/// [value].
///
/// !return-ref
char *ut_cstring_new_sized(const char *value, size_t length);

/// Creates a new C string using the printf [format].
///
/// !return-ref
char *ut_cstring_new_printf(const char *format, ...)
    __attribute__((format(printf, 1, 2)));

/// Creates a new C string using the vprintf [format].
/// Ths is useful if you have a function that takes variable arguments and needs
/// to create a C string. If you already have the arguments, then use
/// [ut_cstring_new_printf].
///
/// !return-ref
char *ut_cstring_new_vprintf(const char *format, va_list ap);

/// Creates a new C string that contains [value] converted to lowercase.
///
/// !return-ref
char *ut_cstring_new_lowercase(const char *value);

/// Creates a new C string that contains [value] converted to uppercase.
///
/// !return-ref
char *ut_cstring_new_uppercase(const char *value);

/// Sets [string] to be a new C string containing [value].
/// If [string] already contains a value, it will be unreferenced first.
void ut_cstring_set(char **string, const char *value);

/// Sets [string] to [NULL] and dereferences the previous value if it was set.
void ut_cstring_clear(char **string);

/// Returns the value of [string] and sets it to [NULL].
///
/// !return-ref
char *ut_cstring_take(char **string);

/// Type used to define C strings so that they are automatically dereferenced.
#define ut_cstring_ref char *__attribute__((__cleanup__(ut_cstring_clear)))

/// Returns the number of characters in [value].
size_t ut_cstring_get_length(const char *value);

/// Returns [true] if the contents of [value1] and [value2] are the same.
bool ut_cstring_equal(const char *value1, const char *value2);

/// Returns [true] if [value] starts with the [prefix].
bool ut_cstring_starts_with(const char *value, const char *prefix);

/// Returns [true] if [value] ends with the [prefix].
bool ut_cstring_ends_with(const char *value, const char *suffix);

/// Creates a new C string containing the provided strings joined together with
/// [separator]. The supplied values are terminated by [NULL]. For example, the
/// following will create a new string conatining `"a:b:c"`.
/// ```c
/// ut_cstring_ref text = ut_cstring_join(":", "a", "b", "c", NULL);
/// ```
///
/// !return-ref
char *ut_cstring_join(const char *separator, const char *value0, ...);
