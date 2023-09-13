#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-x11-client.h"

#pragma once

typedef void (*UtX11ShmQueryVersionCallback)(
    UtObject *object, uint16_t major_version, uint16_t minor_version,
    uint16_t uid, uint16_t gid, uint8_t pixmap_format, bool shared_pixmaps,
    UtObject *error);

/// Creates a new X11 shared memory extension.
///
/// !arg-type client UtX11Client
/// !return-ref
/// !return-type UtX11ShmExtension
UtObject *ut_x11_shm_extension_new(UtObject *client, uint8_t major_opcode,
                                   uint8_t first_event, uint8_t first_error);

void ut_x11_shm_extension_query_version(UtObject *object,
                                        UtObject *callback_object,
                                        UtX11ShmQueryVersionCallback callback);

uint32_t ut_x11_shm_extension_attach(UtObject *object, uint32_t shmid,
                                     bool read_only);

void ut_x11_shm_extension_detach(UtObject *object, uint32_t segment);

uint32_t ut_x11_shm_extension_create_pixmap(UtObject *object, uint32_t drawable,
                                            uint16_t width, uint16_t height,
                                            uint8_t depth, uint32_t segment,
                                            uint32_t offset);

uint32_t ut_x11_shm_extension_attach_fd(UtObject *object, UtObject *fd,
                                        bool read_only);

uint32_t
ut_x11_shm_extension_create_segment(UtObject *object, uint32_t size,
                                    bool read_only, UtObject *callback_object,
                                    UtX11ShmCreateSegmentCallback callback);

/// Returns [true] if [object] is a [UtX11ShmExtension].
bool ut_object_is_x11_shm_extension(UtObject *object);
