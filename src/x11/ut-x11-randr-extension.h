#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-x11-client.h"

#pragma once

typedef void (*UtX11ClientRandrQueryVersionCallback)(void *user_data,
                                                     uint32_t major_version,
                                                     uint32_t minor_version,
                                                     UtObject *error);
typedef void (*UtX11ClientRandrSetScreenConfigCallback)(void *user_data);
typedef void (*UtX11ClientRandrGetScreenInfoCallback)(void *user_data);
typedef void (*UtX11ClientRandrGetScreenSizeRangeCallback)(void *user_data);
typedef void (*UtX11ClientRandrGetScreenResourcesCallback)(void *user_data);
typedef void (*UtX11ClientRandrGetOutputInfoCallback)(void *user_data);
typedef void (*UtX11ClientRandrListOutputPropertiesCallback)(void *user_data);
typedef void (*UtX11ClientRandrQueryOutputPropertyCallback)(void *user_data);
typedef void (*UtX11ClientRandrGetOutputPropertyCallback)(void *user_data);
typedef void (*UtX11ClientRandrCreateModeCallback)(void *user_data);
typedef void (*UtX11ClientRandrGetCrtcInfoCallback)(void *user_data);
typedef void (*UtX11ClientRandrSetCrtcConfigCallback)(void *user_data);
typedef void (*UtX11ClientRandrGetCrtcGammaSizeCallback)(void *user_data);
typedef void (*UtX11ClientRandrGetCrtcGammaCallback)(void *user_data);
typedef void (*UtX11ClientRandrGetMonitorsCallback)(void *user_data,
                                                    UtObject *monitor_info);

UtObject *ut_x11_randr_extension_new(UtObject *client, uint8_t major_opcode,
                                     uint8_t first_event, uint8_t first_error,
                                     const UtX11EventCallbacks *event_callbacks,
                                     void *user_data, UtObject *cancel);

void ut_x11_randr_extension_query_version(
    UtObject *object, UtX11ClientRandrQueryVersionCallback callback,
    void *user_data, UtObject *cancel);

void ut_x11_randr_extension_set_screen_config(
    UtObject *object, uint32_t window, uint32_t timestamp,
    uint32_t config_timestamp, int16_t size_index, uint16_t rotation,
    UtX11ClientRandrSetScreenConfigCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_randr_extension_select_input(UtObject *object, uint32_t window,
                                         uint16_t enable);

void ut_x11_randr_extension_get_screen_info(
    UtObject *object, uint32_t window,
    UtX11ClientRandrGetScreenInfoCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_randr_extension_get_screen_size_range(
    UtObject *object, uint32_t window,
    UtX11ClientRandrGetScreenSizeRangeCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_randr_extension_set_screen_size(UtObject *object, uint32_t window,
                                            uint16_t width, uint16_t height,
                                            uint32_t width_in_millimeters,
                                            uint32_t height_in_millimeters);

void ut_x11_randr_extension_get_screen_resources(
    UtObject *object, uint32_t window,
    UtX11ClientRandrGetScreenResourcesCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_randr_extension_get_output_info(
    UtObject *object, uint32_t output, uint32_t config_timestamp,
    UtX11ClientRandrGetOutputInfoCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_randr_extension_list_output_properties(
    UtObject *object, uint32_t output,
    UtX11ClientRandrListOutputPropertiesCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_randr_extension_query_output_property(
    UtObject *object, uint32_t output, uint32_t property,
    UtX11ClientRandrQueryOutputPropertyCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_randr_extension_configure_output_property(UtObject *object,
                                                      uint32_t output,
                                                      uint32_t property,
                                                      bool pending, bool range,
                                                      UtObject *valid_values);

void ut_x11_randr_extension_change_output_property(UtObject *object,
                                                   uint32_t output,
                                                   uint32_t property,
                                                   uint32_t format, int mode);

void ut_x11_randr_extension_delete_output_property(UtObject *object,
                                                   uint32_t output,
                                                   uint32_t property);

void ut_x11_randr_extension_get_output_property(
    UtObject *object, uint32_t output, uint32_t property, uint32_t type,
    UtX11ClientRandrGetOutputPropertyCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_randr_extension_get_create_mode(
    UtObject *object, uint32_t window, int mode, const char *mode_name,
    UtX11ClientRandrCreateModeCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_randr_extension_destroy_mode(UtObject *object, uint32_t mode);

void ut_x11_randr_extension_add_output_mode(UtObject *object, uint32_t output,
                                            uint32_t mode);

void ut_x11_randr_extension_delete_output_mode(UtObject *object,
                                               uint32_t output, uint32_t mode);

void ut_x11_randr_extension_get_crtc_info(
    UtObject *object, uint32_t crtc, uint32_t config_timestamp,
    UtX11ClientRandrGetCrtcInfoCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_randr_extension_set_crtc_config(
    UtObject *object, uint32_t crtc, uint32_t timestamp,
    uint32_t config_timestamp, int16_t x, int16_t y, uint32_t mode,
    UtX11ClientRandrSetCrtcConfigCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_randr_extension_get_crtc_gamma_size(
    UtObject *object, uint32_t crtc,
    UtX11ClientRandrGetCrtcGammaSizeCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_randr_extension_get_crtc_gamma(
    UtObject *object, uint32_t crtc,
    UtX11ClientRandrGetCrtcGammaCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_randr_extension_set_crtc_gamma(UtObject *object, uint32_t crtc,
                                           int red, int green, int blue);

void ut_x11_randr_extension_get_monitors(
    UtObject *object, uint32_t window,
    UtX11ClientRandrGetMonitorsCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_randr_extension_set_monitor(UtObject *object, uint32_t window,
                                        UtObject *monitor_info);

void ut_x11_randr_extension_delete_monitor(UtObject *object, uint32_t window,
                                           uint32_t name);

bool ut_object_is_x11_randr_extension(UtObject *object);
