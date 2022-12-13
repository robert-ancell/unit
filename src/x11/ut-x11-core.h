#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-x11-client.h"

#pragma once

UtObject *ut_x11_core_new(UtObject *client,
                          const UtX11EventCallbacks *event_callbacks,
                          void *user_data, UtObject *cancel);

uint32_t ut_x11_core_create_window(UtObject *object, uint32_t parent, int16_t x,
                                   int16_t y, uint16_t width, uint16_t height,
                                   uint8_t depth, uint32_t visual,
                                   uint32_t event_mask);

void ut_x11_core_destroy_window(UtObject *object, uint32_t window);

void ut_x11_core_map_window(UtObject *object, uint32_t window);

void ut_x11_core_unmap_window(UtObject *object, uint32_t window);

void ut_x11_core_configure_window(UtObject *object, uint32_t window, int16_t x,
                                  int16_t y, uint16_t width, uint16_t height);

void ut_x11_core_intern_atom(UtObject *object, const char *name,
                             bool only_if_exists,
                             UtX11InternAtomCallback callback, void *user_data,
                             UtObject *cancel);

void ut_x11_core_get_atom_name(UtObject *object, uint32_t atom,
                               UtX11GetAtomNameCallback callback,
                               void *user_data, UtObject *cancel);

void ut_x11_core_change_property_uint8(UtObject *object, uint32_t window,
                                       uint32_t property, uint32_t type,
                                       UtObject *value);

void ut_x11_core_change_property_uint16(UtObject *object, uint32_t window,
                                        uint32_t property, uint32_t type,
                                        UtObject *value);

void ut_x11_core_change_property_uint32(UtObject *object, uint32_t window,
                                        uint32_t property, uint32_t type,
                                        UtObject *value);

void ut_x11_core_change_property_string(UtObject *object, uint32_t window,
                                        uint32_t property, const char *value);

void ut_x11_core_delete_property(UtObject *object, uint32_t window,
                                 uint32_t property);

void ut_x11_core_get_property(UtObject *object, uint32_t window,
                              uint32_t property, uint32_t type,
                              uint32_t long_offset, uint32_t long_length,
                              bool delete, UtX11GetPropertyCallback callback,
                              void *user_data, UtObject *cancel);

void ut_x11_core_list_properties(UtObject *object, uint32_t window,
                                 UtX11ListPropertiesCallback callback,
                                 void *user_data, UtObject *cancel);

uint32_t ut_x11_core_create_pixmap(UtObject *object, uint32_t drawable,
                                   uint16_t width, uint16_t height,
                                   uint8_t depth);

void ut_x11_core_free_pixmap(UtObject *object, uint32_t pixmap);

uint32_t ut_x11_core_create_gc(UtObject *object, uint32_t drawable);

void ut_x11_core_free_gc(UtObject *object, uint32_t gc);

void ut_x11_core_clear_area(UtObject *object, uint32_t window, int16_t x,
                            int16_t y, uint16_t width, uint16_t height,
                            bool exposures);

void ut_x11_core_copy_area(UtObject *object, uint32_t src_drawable,
                           uint32_t dst_drawable, uint32_t gc, int16_t src_x,
                           int16_t src_y, int16_t dst_x, int16_t dst_y,
                           uint16_t width, uint16_t height);

void ut_x11_core_put_image(UtObject *object, uint32_t drawable, uint32_t gc,
                           UtX11ImageFormat format, uint16_t width,
                           uint16_t height, uint8_t depth, int16_t dst_x,
                           int16_t dst_y, uint8_t *data, size_t data_length);

void ut_x11_core_query_extension(UtObject *object, const char *name,
                                 UtX11QueryExtensionCallback callback,
                                 void *user_data, UtObject *cancel);

void ut_x11_core_list_extensions(UtObject *object,
                                 UtX11ListExtensionsCallback callback,
                                 void *user_data, UtObject *cancel);

void ut_x11_core_bell(UtObject *object);

void ut_x11_core_kill_client(UtObject *object, uint32_t resource);

bool ut_object_is_x11_core(UtObject *object);