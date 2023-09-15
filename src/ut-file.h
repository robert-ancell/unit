#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  void (*open_read)(UtObject *object);
  void (*open_write)(UtObject *object, bool create);
  void (*close)(UtObject *object);
} UtFileInterface;

extern int ut_file_id;

/// Open this file for reading.
void ut_file_open_read(UtObject *object);

/// Open this file for writing.
/// If [create] is true then the file will be created if it doesn't exist.
void ut_file_open_write(UtObject *object, bool create);

/// Close this file.
void ut_file_close(UtObject *object);

/// Returns [true] if [object] is a [UtFile].
bool ut_object_implements_file(UtObject *object);
