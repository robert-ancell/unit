#include <stdbool.h>
#include <stddef.h>

#pragma once

typedef struct _UtObject UtObject;

typedef struct {
  const char *type_name;
  void (*init)(UtObject *object);
  char *(*to_string)(UtObject *object);
  bool (*equal)(UtObject *object, UtObject *other);
  int (*hash)(UtObject *object);
  void (*cleanup)(UtObject *object);

  struct {
    void *interface_id;
    void *interface;
  } interfaces[];
} UtObjectInterface;

struct _UtObject {
  UtObjectInterface *interface;
  int ref_count;
  void *weak_references;
};

/// Creates a new object with [object_size] and [functions].
/// This function is only required when creating new object types.
///
/// !return-ref
/// !return-type UtObject
UtObject *ut_object_new(size_t object_size, UtObjectInterface *functions);

/// Gets the interface functions that matches [interface_id] or NULL.
/// This function is only required when creating new interface types.
void *ut_object_get_interface(UtObject *object, void *interface_id);

/// Returns the name of this type.
const char *ut_object_get_type_name(UtObject *object);

/// Generates a human readable string summary of this object.
char *ut_object_to_string(UtObject *object);

/// Returns true if this object is the same as [other].
///
/// !arg-type other UtObject
bool ut_object_equal(UtObject *object, UtObject *other);

/// Returns a hash value for this object.
int ut_object_get_hash(UtObject *object);

/// Increase the reference count on this object.
/// Returns [NULL] if [object] is [NULL].
///
/// !return-ref
/// !return-type UtObject NULL
UtObject *ut_object_ref(UtObject *object);

/// Decrease the reference count on this object.
/// The object will be descroyed when the last reference is removed.
/// Does nothing if [object] is [NULL].
void ut_object_unref(UtObject *object);

/// Add a weak reference on this object and set [object_ref] to be this object.
/// When this object is dereferenced, [object_ref] will be set to [NULL].
void ut_object_weak_ref(UtObject *object, UtObject **object_ref);

/// Removes a weak reference from [object_ref].
void ut_object_weak_unref(UtObject **object_ref);

/// Sets [object] to be a reference to [value]. If [object] already contained a
/// value, it is unreferenced.
///
/// !arg-type value UtObject
static inline void ut_object_set(UtObject **object, UtObject *value) {
  ut_object_unref(*object);
  *object = ut_object_ref(value);
}

/// Clears [object] to NULL. If [object] already contained a value, it is
/// unreferenced.
static inline void ut_object_clear(UtObject **object) {
  ut_object_unref(*object);
  *object = NULL;
}

#define UtObjectRef UtObject *__attribute__((__cleanup__(ut_object_clear)))

/// Returns [true] if [object] is the type defined in [functions].
/// This is used when creating new object types.
bool ut_object_is_type(UtObject *object, UtObjectInterface *functions);
