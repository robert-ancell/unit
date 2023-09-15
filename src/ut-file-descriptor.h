#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new file descriptor object from [fd].
/// The file descriptor will be closed when this object is destroyed.
///
/// !return-ref
/// !return-type UtFileDescriptor
UtObject *ut_file_descriptor_new(int fd);

/// Returns the file descriptor.
int ut_file_descriptor_get_fd(UtObject *object);

/// Returns the file descriptor and stops containing it in this object.
int ut_file_descriptor_take_fd(UtObject *object);

/// Closes the file descriptor.
void ut_file_descriptor_close(UtObject *object);

/// Returns [true] if [object] is a [UtFileDescriptor].
bool ut_object_is_file_descriptor(UtObject *object);
