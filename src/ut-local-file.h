#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

/// Creates a new local file that accesses [path].
///
/// !return-ref
/// !return-type UtLocalFile
UtObject *ut_local_file_new(const char *path);

/// Returns the file descriptor for this file or [NULL] if not yet opened.
///
/// !return-type UtFileDescriptor NULL
UtObject *ut_local_file_get_fd(UtObject *object);

/// Returns [true] if [object] is a [UtLocalFile].
bool ut_object_is_local_file(UtObject *object);
