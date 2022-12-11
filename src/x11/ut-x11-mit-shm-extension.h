#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtX11MitShmQueryVersionCallback)(
    void *user_data, uint16_t major_version, uint16_t minor_version,
    uint16_t uid, uint16_t gid, uint8_t pixmap_format, bool shared_pixmaps,
    UtObject *error);
typedef void (*UtX11MitShmCreateSegmentCallback)(void *user_data, UtObject *fd,
                                                 UtObject *error);

UtObject *ut_x11_mit_shm_extension_new(UtObject *client, uint8_t major_opcode,
                                       uint8_t first_event,
                                       uint8_t first_error);

void ut_x11_mit_shm_extension_query_version(
    UtObject *object, UtX11MitShmQueryVersionCallback callback, void *user_data,
    UtObject *cancel);

uint32_t ut_x11_mit_shm_extension_attach(UtObject *object, uint32_t shmid,
                                         bool read_only);

void ut_x11_mit_shm_extension_detach(UtObject *object, uint32_t segment);

uint32_t ut_x11_mit_shm_extension_create_pixmap(UtObject *object,
                                                uint32_t drawable,
                                                uint16_t width, uint16_t height,
                                                uint8_t depth, uint32_t segment,
                                                uint32_t offset);

uint32_t ut_x11_mit_shm_extension_attach_fd(UtObject *object, UtObject *fd,
                                            bool read_only);

uint32_t ut_x11_mit_shm_extension_create_segment(
    UtObject *object, uint32_t size, bool read_only,
    UtX11MitShmCreateSegmentCallback callback, void *user_data,
    UtObject *cancel);

bool ut_object_is_x11_mit_shm_extension(UtObject *object);
