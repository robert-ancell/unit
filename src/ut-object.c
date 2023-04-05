#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ut.h"

typedef struct _WeakReference WeakReference;
struct _WeakReference {
  UtObject **object_ref;
  WeakReference *next;
};

UtObject *ut_object_new(size_t object_size, UtObjectInterface *interface) {
  UtObject *object = malloc(object_size);
  memset(object, 0, object_size);
  object->interface = interface;
  object->ref_count = 1;
  object->weak_references = NULL;

  if (interface->init != NULL) {
    interface->init(object);
  }

  return object;
}

bool ut_object_is_type(UtObject *object, UtObjectInterface *interface) {
  return object->interface == interface;
}

const char *ut_object_get_type_name(UtObject *object) {
  return object->interface->type_name;
}

char *ut_object_to_string(UtObject *object) {
  if (object == NULL) {
    return ut_cstring_new("NULL");
  }

  if (object->interface->to_string != NULL) {
    return object->interface->to_string(object);
  }

  UtObjectRef string = ut_string_new("<");
  ut_string_append(string, ut_object_get_type_name(object));
  ut_string_append(string, ">");

  return ut_string_take_text(string);
}

bool ut_object_equal(UtObject *object, UtObject *other) {
  // Default equality is comparing an object against itself.
  if (object->interface->equal == NULL) {
    return object == other;
  }

  return object->interface->equal(object, other);
}

int ut_object_get_hash(UtObject *object) {
  // Default has is based off the memory address of the object.
  if (object->interface->hash == NULL) {
    return (intptr_t)object;
  }

  return object->interface->hash(object);
}

UtObject *ut_object_ref(UtObject *object) {
  if (object == NULL) {
    return NULL;
  }

  assert(object->ref_count > 0);

  object->ref_count++;
  return object;
}

void ut_object_unref(UtObject *object) {
  if (object == NULL) {
    return;
  }

  assert(object->ref_count > 0);

  object->ref_count--;
  if (object->ref_count > 0) {
    return;
  }

  WeakReference *ref = object->weak_references;
  while (ref != NULL) {
    *ref->object_ref = NULL;
    WeakReference *next_ref = ref->next;
    free(ref);
    ref = next_ref;
  }
  object->weak_references = NULL;
  if (object->interface->cleanup != NULL) {
    object->interface->cleanup(object);
  }
  free(object);
}

void ut_object_weak_ref(UtObject *object, UtObject **object_ref) {
  if (object == NULL) {
    *object_ref = NULL;
    return;
  }

  WeakReference *ref = malloc(sizeof(WeakReference));
  ref->object_ref = object_ref;
  ref->next = object->weak_references;
  object->weak_references = ref;
  *object_ref = object;
}

void ut_object_weak_unref(UtObject **object_ref) {
  UtObject *object = *object_ref;
  if (object == NULL) {
    return;
  }

  WeakReference *prev_ref = NULL;
  WeakReference *ref = object->weak_references;
  while (ref != NULL) {
    if (ref->object_ref == object_ref) {
      if (prev_ref == NULL) {
        object->weak_references = ref->next;
      } else {
        prev_ref->next = ref->next;
      }
      free(ref);
      return;
    }
    prev_ref = ref;
    ref = ref->next;
  }
}

void *ut_object_get_interface(UtObject *object, void *interface_id) {
  for (int i = 0; object->interface->interfaces[i].interface_id != NULL; i++) {
    if (object->interface->interfaces[i].interface_id == interface_id) {
      return object->interface->interfaces[i].interface;
    }
  }

  return NULL;
}
