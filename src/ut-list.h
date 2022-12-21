#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

typedef struct {
  bool is_mutable;
  size_t (*get_length)(UtObject *object);
  UtObject *(*get_element)(UtObject *object, size_t index);
  UtObject *(*get_sublist)(UtObject *object, size_t index, size_t count);
  UtObject *(*copy)(UtObject *object);
  void (*insert)(UtObject *object, size_t index, UtObject *item);
  void (*insert_list)(UtObject *object, size_t index, UtObject *list);
  void (*remove)(UtObject *object, size_t start, size_t count);
  void (*resize)(UtObject *object, size_t length);
} UtListInterface;

extern int ut_list_id;

/// Creates a new object that implements [UtList].
/// !returns-ref
UtObject *ut_list_new();

/// Creates a new object that implements [UtList] and contains the passed
/// elements. The elements are terminated by [NULL]:
/// ```c
/// UtObjectRef list = ut_list_new_from_elements(item0, item1, item2, NULL);
/// ```
/// !returns-ref
UtObject *ut_list_new_from_elements(UtObject *item0, ...);

/// Creates a new object that implements [UtList] and contains the passed
/// elements. The elements are terminated by [NULL]. The is useful when the
/// elements are is being created only for this list:
/// ```c
/// UtObjectRef list = ut_list_new_from_elements_take(ut_string_new("Hello"),
/// ut_string_new("World"), NULL);
/// ```
/// !take-ref item0
/// !take-ref ...
/// !returns-ref
UtObject *ut_list_new_from_elements_take(UtObject *item0, ...);

/// Gets the number of elements in [object].
size_t ut_list_get_length(UtObject *object);

/// Gets the element at the given [index] in [object].
/// !returns-ref
UtObject *ut_list_get_element(UtObject *object, size_t index);

/// Gets the first element in [object].
/// !returns-ref
UtObject *ut_list_get_first(UtObject *object);

/// Gets the last element in [object].
/// !returns-ref
UtObject *ut_list_get_last(UtObject *object);

/// Creates a new list containing [count] elements from [start] in [object].
/// If the new list can refer to the content of [object] it will do this instead
/// of copying. This means any updates to [object] will also occur in the
/// sublist. If this behaviour is not desired, then use [ut_list_copy] to ensure
/// the returned sublist has separate content. Passing a range that is not valid
/// will trigger assertions. !returns-ref
UtObject *ut_list_get_sublist(UtObject *object, size_t start, size_t count);

/// Creates a new list containing the same contents as [object].
/// If [object] was immutable, the new list will be mutable.
/// !returns-ref
UtObject *ut_list_copy(UtObject *object);

/// Returns [true] if the contents of [object] can be changed.
/// Attempting to modify an immutable list will trigger assertions.
bool ut_list_is_mutable(UtObject *object);

/// Adds an [item] to the end of [object].
void ut_list_append(UtObject *object, UtObject *item);

/// Adds an [item] to the end of [object] and takes the reference.
/// The is useful when [item] is being created only for this list:
/// ```c
/// ut_list_append_take(list, ut_string_new("Hello"));
/// ```
/// !take-ref item
void ut_list_append_take(UtObject *object, UtObject *item);

/// Adds the contents of [list] to the end of [object].
/// The data is copied.
void ut_list_append_list(UtObject *object, UtObject *list);

/// Adds an [item] to the start of [object].
void ut_list_prepend(UtObject *object, UtObject *item);

/// Adds an [item] to the start of [object] and takes the reference.
/// The is useful when [item] is being created only for this list:
/// ```c
/// ut_list_prepend_take(list, ut_string_new("Hello"));
/// ```
/// !take-ref item
void ut_list_prepend_take(UtObject *object, UtObject *item);

/// Adds the contents of [list] to the start of [object].
/// The data is copied.
void ut_list_prepend_list(UtObject *object, UtObject *list);

/// Adds an [item] in position [index] of [object].
/// Existing items will be moved to make space for this item so the list will
/// always grow in size by 1.
void ut_list_insert(UtObject *object, size_t index, UtObject *item);

/// Adds an [item] in position [index] of [object].
/// Existing items will be moved to make space for this item.
/// The is useful when [item] is being created only for this list:
/// ```c
/// ut_list_insert_take(list, 3, ut_string_new("Hello"));
/// ```
/// !take-ref item
void ut_list_insert_take(UtObject *object, size_t index, UtObject *item);

/// Insert items from [list] into [object] at position [index].
/// Existing items will be moved to make space for this item so the list will
/// always grow in size by the size of [list].
void ut_list_insert_list(UtObject *object, size_t index, UtObject *list);

/// Remove [count] elements from [object] starting at the position [index].
/// Elements after this range will be moved to use the space where the
/// removed elements were so the list will always reduce in size by [count].
/// Passing a range that is not valid will trigger assertions.
void ut_list_remove(UtObject *object, size_t index, size_t count);

/// Remove all elements in [object].
void ut_list_clear(UtObject *object);

/// Change the size of [object] to [length].
/// If [length] is smaller than the current size the excess elements will be
/// removed. If [length] is smaller than the current size new elements will be
/// added with empty contents (dependant on the list implementation).
void ut_list_resize(UtObject *object, size_t length);

/// Returns [true] if [object] implements [UtList].
bool ut_object_implements_list(UtObject *object);
