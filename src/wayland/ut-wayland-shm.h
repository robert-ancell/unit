#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Format of SHM images:
/// - [UT_WAYLAND_SHM_FORMAT_argb8888] - argb8888.
/// - [UT_WAYLAND_SHM_FORMAT_xrgb8888] - xrgb8888.
/// - [UT_WAYLAND_SHM_FORMAT_c8] - c8.
/// - [UT_WAYLAND_SHM_FORMAT_rgb332] - rgb332.
/// - [UT_WAYLAND_SHM_FORMAT_bgr233] - bgr233.
/// - [UT_WAYLAND_SHM_FORMAT_xrgb4444] - xrgb4444.
/// - [UT_WAYLAND_SHM_FORMAT_xbgr4444] - xbgr4444.
/// - [UT_WAYLAND_SHM_FORMAT_rgbx4444] - rgbx4444.
/// - [UT_WAYLAND_SHM_FORMAT_bgrx4444] - bgrx4444.
/// - [UT_WAYLAND_SHM_FORMAT_argb4444] - argb4444.
/// - [UT_WAYLAND_SHM_FORMAT_abgr4444] - abgr4444.
/// - [UT_WAYLAND_SHM_FORMAT_rgba4444] - rgba4444.
/// - [UT_WAYLAND_SHM_FORMAT_bgra4444] - bgra4444.
/// - [UT_WAYLAND_SHM_FORMAT_xrgb1555] - xrgb1555.
/// - [UT_WAYLAND_SHM_FORMAT_xbgr1555] - xbgr1555.
/// - [UT_WAYLAND_SHM_FORMAT_rgbx5551] - rgbx5551.
/// - [UT_WAYLAND_SHM_FORMAT_bgrx5551] - bgrx5551.
/// - [UT_WAYLAND_SHM_FORMAT_argb1555] - argb1555.
/// - [UT_WAYLAND_SHM_FORMAT_abgr1555] - abgr1555.
/// - [UT_WAYLAND_SHM_FORMAT_rgba5551] - rgba5551.
/// - [UT_WAYLAND_SHM_FORMAT_bgra5551] - bgra5551.
/// - [UT_WAYLAND_SHM_FORMAT_rgb565] - rgb565.
/// - [UT_WAYLAND_SHM_FORMAT_bgr565] - bgr565.
/// - [UT_WAYLAND_SHM_FORMAT_rgb888] - rgb888.
/// - [UT_WAYLAND_SHM_FORMAT_bgr888] - bgr888.
/// - [UT_WAYLAND_SHM_FORMAT_xbgr8888] - xbgr8888.
/// - [UT_WAYLAND_SHM_FORMAT_rgbx8888] - rgbx8888.
/// - [UT_WAYLAND_SHM_FORMAT_bgrx8888] - bgrx8888.
/// - [UT_WAYLAND_SHM_FORMAT_abgr8888] - abgr8888.
/// - [UT_WAYLAND_SHM_FORMAT_rgba8888] - rgba8888.
/// - [UT_WAYLAND_SHM_FORMAT_bgra8888] - bgra8888.
/// - [UT_WAYLAND_SHM_FORMAT_xrgb2101010] - xrgb2101010.
/// - [UT_WAYLAND_SHM_FORMAT_xbgr2101010] - xbgr2101010.
/// - [UT_WAYLAND_SHM_FORMAT_rgbx1010102] - rgbx1010102.
/// - [UT_WAYLAND_SHM_FORMAT_bgrx1010102] - bgrx1010102.
/// - [UT_WAYLAND_SHM_FORMAT_argb2101010] - argb2101010.
/// - [UT_WAYLAND_SHM_FORMAT_abgr2101010] - abgr2101010.
/// - [UT_WAYLAND_SHM_FORMAT_rgba1010102] - rgba1010102.
/// - [UT_WAYLAND_SHM_FORMAT_bgra1010102] - bgra1010102.
/// - [UT_WAYLAND_SHM_FORMAT_yuyv] - yuyv.
/// - [UT_WAYLAND_SHM_FORMAT_yvyu] - yvyu.
/// - [UT_WAYLAND_SHM_FORMAT_uyvy] - uyvy.
/// - [UT_WAYLAND_SHM_FORMAT_vyuy] - vyuy.
/// - [UT_WAYLAND_SHM_FORMAT_ayuv] - ayuv.
/// - [UT_WAYLAND_SHM_FORMAT_nv12] - nv12.
/// - [UT_WAYLAND_SHM_FORMAT_nv21] - nv21.
/// - [UT_WAYLAND_SHM_FORMAT_nv16] - nv16.
/// - [UT_WAYLAND_SHM_FORMAT_nv61] - nv61.
/// - [UT_WAYLAND_SHM_FORMAT_yuv410] - yuv410.
/// - [UT_WAYLAND_SHM_FORMAT_yvu410] - yvu410.
/// - [UT_WAYLAND_SHM_FORMAT_yuv411] - yuv411.
/// - [UT_WAYLAND_SHM_FORMAT_yvu411] - yvu411.
/// - [UT_WAYLAND_SHM_FORMAT_yuv420] - yuv420.
/// - [UT_WAYLAND_SHM_FORMAT_yvu420] - yvu420.
/// - [UT_WAYLAND_SHM_FORMAT_yuv422] - yuv422.
/// - [UT_WAYLAND_SHM_FORMAT_yvu422] - yvu422.
/// - [UT_WAYLAND_SHM_FORMAT_yuv444] - yuv444.
/// - [UT_WAYLAND_SHM_FORMAT_yvu444] - yvu444.
/// - [UT_WAYLAND_SHM_FORMAT_r8] - r8.
/// - [UT_WAYLAND_SHM_FORMAT_r16] - r16.
/// - [UT_WAYLAND_SHM_FORMAT_rg88] - rg88.
/// - [UT_WAYLAND_SHM_FORMAT_gr88] - gr88.
/// - [UT_WAYLAND_SHM_FORMAT_rg1616] - rg1616.
/// - [UT_WAYLAND_SHM_FORMAT_gr1616] - gr1616.
/// - [UT_WAYLAND_SHM_FORMAT_xrgb16161616f] - xrgb16161616f.
/// - [UT_WAYLAND_SHM_FORMAT_xbgr16161616f] - xbgr16161616f.
/// - [UT_WAYLAND_SHM_FORMAT_argb16161616f] - argb16161616f.
/// - [UT_WAYLAND_SHM_FORMAT_abgr16161616f] - abgr16161616f.
/// - [UT_WAYLAND_SHM_FORMAT_xyuv8888] - xyuv8888.
/// - [UT_WAYLAND_SHM_FORMAT_vuy888] - vuy888.
/// - [UT_WAYLAND_SHM_FORMAT_vuy101010] - vuy101010.
/// - [UT_WAYLAND_SHM_FORMAT_y210] - y210.
/// - [UT_WAYLAND_SHM_FORMAT_y212] - y212.
/// - [UT_WAYLAND_SHM_FORMAT_y216] - y216.
/// - [UT_WAYLAND_SHM_FORMAT_y410] - y410.
/// - [UT_WAYLAND_SHM_FORMAT_y412] - y412.
/// - [UT_WAYLAND_SHM_FORMAT_y416] - y416.
/// - [UT_WAYLAND_SHM_FORMAT_xvyu2101010] - xvyu2101010.
/// - [UT_WAYLAND_SHM_FORMAT_xvyu12_16161616] - xvyu12_16161616.
/// - [UT_WAYLAND_SHM_FORMAT_xvyu16161616] - xvyu16161616.
/// - [UT_WAYLAND_SHM_FORMAT_y0l0] - y0l0.
/// - [UT_WAYLAND_SHM_FORMAT_x0l0] - x0l0.
/// - [UT_WAYLAND_SHM_FORMAT_y0l2] - y0l2.
/// - [UT_WAYLAND_SHM_FORMAT_x0l2] - x0l2.
/// - [UT_WAYLAND_SHM_FORMAT_yuv420_8bit] - yuv420_8bit.
/// - [UT_WAYLAND_SHM_FORMAT_yuv420_10bit] - yuv420_10bit.
/// - [UT_WAYLAND_SHM_FORMAT_xrgb8888_a8] - xrgb8888_a8.
/// - [UT_WAYLAND_SHM_FORMAT_xbgr8888_a8] - xbgr8888_a8.
/// - [UT_WAYLAND_SHM_FORMAT_rgbx8888_a8] - rgbx8888_a8.
/// - [UT_WAYLAND_SHM_FORMAT_bgrx8888_a8] - bgrx8888_a8.
/// - [UT_WAYLAND_SHM_FORMAT_rgb888_a8] - rgb888_a8.
/// - [UT_WAYLAND_SHM_FORMAT_bgr888_a8] - bgr888_a8.
/// - [UT_WAYLAND_SHM_FORMAT_rgb565_a8] - rgb565_a8.
/// - [UT_WAYLAND_SHM_FORMAT_bgr565_a8] - bgr565_a8.
/// - [UT_WAYLAND_SHM_FORMAT_nv24] - nv24.
/// - [UT_WAYLAND_SHM_FORMAT_nv42] - nv42.
/// - [UT_WAYLAND_SHM_FORMAT_p210] - p210.
/// - [UT_WAYLAND_SHM_FORMAT_p010] - p010.
/// - [UT_WAYLAND_SHM_FORMAT_p012] - p012.
/// - [UT_WAYLAND_SHM_FORMAT_p016] - p016.
/// - [UT_WAYLAND_SHM_FORMAT_axbxgxrx106106106106] - axbxgxrx106106106106.
/// - [UT_WAYLAND_SHM_FORMAT_nv15] - nv15.
/// - [UT_WAYLAND_SHM_FORMAT_q410] - q410.
/// - [UT_WAYLAND_SHM_FORMAT_q401] - q401.
/// - [UT_WAYLAND_SHM_FORMAT_xrgb16161616] - xrgb16161616.
/// - [UT_WAYLAND_SHM_FORMAT_xbgr16161616] - xbgr16161616.
/// - [UT_WAYLAND_SHM_FORMAT_argb16161616] - argb16161616.
/// - [UT_WAYLAND_SHM_FORMAT_abgr16161616] - abgr16161616.
typedef enum {
  UT_WAYLAND_SHM_FORMAT_argb8888 = 0,
  UT_WAYLAND_SHM_FORMAT_xrgb8888 = 1,
  UT_WAYLAND_SHM_FORMAT_c8 = 0x20203843,
  UT_WAYLAND_SHM_FORMAT_rgb332 = 0x38424752,
  UT_WAYLAND_SHM_FORMAT_bgr233 = 0x38524742,
  UT_WAYLAND_SHM_FORMAT_xrgb4444 = 0x32315258,
  UT_WAYLAND_SHM_FORMAT_xbgr4444 = 0x32314258,
  UT_WAYLAND_SHM_FORMAT_rgbx4444 = 0x32315852,
  UT_WAYLAND_SHM_FORMAT_bgrx4444 = 0x32315842,
  UT_WAYLAND_SHM_FORMAT_argb4444 = 0x32315241,
  UT_WAYLAND_SHM_FORMAT_abgr4444 = 0x32314241,
  UT_WAYLAND_SHM_FORMAT_rgba4444 = 0x32314152,
  UT_WAYLAND_SHM_FORMAT_bgra4444 = 0x32314142,
  UT_WAYLAND_SHM_FORMAT_xrgb1555 = 0x35315258,
  UT_WAYLAND_SHM_FORMAT_xbgr1555 = 0x35314258,
  UT_WAYLAND_SHM_FORMAT_rgbx5551 = 0x35315852,
  UT_WAYLAND_SHM_FORMAT_bgrx5551 = 0x35315842,
  UT_WAYLAND_SHM_FORMAT_argb1555 = 0x35315241,
  UT_WAYLAND_SHM_FORMAT_abgr1555 = 0x35314241,
  UT_WAYLAND_SHM_FORMAT_rgba5551 = 0x35314152,
  UT_WAYLAND_SHM_FORMAT_bgra5551 = 0x35314142,
  UT_WAYLAND_SHM_FORMAT_rgb565 = 0x36314752,
  UT_WAYLAND_SHM_FORMAT_bgr565 = 0x36314742,
  UT_WAYLAND_SHM_FORMAT_rgb888 = 0x34324752,
  UT_WAYLAND_SHM_FORMAT_bgr888 = 0x34324742,
  UT_WAYLAND_SHM_FORMAT_xbgr8888 = 0x34324258,
  UT_WAYLAND_SHM_FORMAT_rgbx8888 = 0x34325852,
  UT_WAYLAND_SHM_FORMAT_bgrx8888 = 0x34325842,
  UT_WAYLAND_SHM_FORMAT_abgr8888 = 0x34324241,
  UT_WAYLAND_SHM_FORMAT_rgba8888 = 0x34324152,
  UT_WAYLAND_SHM_FORMAT_bgra8888 = 0x34324142,
  UT_WAYLAND_SHM_FORMAT_xrgb2101010 = 0x30335258,
  UT_WAYLAND_SHM_FORMAT_xbgr2101010 = 0x30334258,
  UT_WAYLAND_SHM_FORMAT_rgbx1010102 = 0x30335852,
  UT_WAYLAND_SHM_FORMAT_bgrx1010102 = 0x30335842,
  UT_WAYLAND_SHM_FORMAT_argb2101010 = 0x30335241,
  UT_WAYLAND_SHM_FORMAT_abgr2101010 = 0x30334241,
  UT_WAYLAND_SHM_FORMAT_rgba1010102 = 0x30334152,
  UT_WAYLAND_SHM_FORMAT_bgra1010102 = 0x30334142,
  UT_WAYLAND_SHM_FORMAT_yuyv = 0x56595559,
  UT_WAYLAND_SHM_FORMAT_yvyu = 0x55595659,
  UT_WAYLAND_SHM_FORMAT_uyvy = 0x59565955,
  UT_WAYLAND_SHM_FORMAT_vyuy = 0x59555956,
  UT_WAYLAND_SHM_FORMAT_ayuv = 0x56555941,
  UT_WAYLAND_SHM_FORMAT_nv12 = 0x3231564e,
  UT_WAYLAND_SHM_FORMAT_nv21 = 0x3132564e,
  UT_WAYLAND_SHM_FORMAT_nv16 = 0x3631564e,
  UT_WAYLAND_SHM_FORMAT_nv61 = 0x3136564e,
  UT_WAYLAND_SHM_FORMAT_yuv410 = 0x39565559,
  UT_WAYLAND_SHM_FORMAT_yvu410 = 0x39555659,
  UT_WAYLAND_SHM_FORMAT_yuv411 = 0x31315559,
  UT_WAYLAND_SHM_FORMAT_yvu411 = 0x31315659,
  UT_WAYLAND_SHM_FORMAT_yuv420 = 0x32315559,
  UT_WAYLAND_SHM_FORMAT_yvu420 = 0x32315659,
  UT_WAYLAND_SHM_FORMAT_yuv422 = 0x36315559,
  UT_WAYLAND_SHM_FORMAT_yvu422 = 0x36315659,
  UT_WAYLAND_SHM_FORMAT_yuv444 = 0x34325559,
  UT_WAYLAND_SHM_FORMAT_yvu444 = 0x34325659,
  UT_WAYLAND_SHM_FORMAT_r8 = 0x20203852,
  UT_WAYLAND_SHM_FORMAT_r16 = 0x20363152,
  UT_WAYLAND_SHM_FORMAT_rg88 = 0x38384752,
  UT_WAYLAND_SHM_FORMAT_gr88 = 0x38385247,
  UT_WAYLAND_SHM_FORMAT_rg1616 = 0x32334752,
  UT_WAYLAND_SHM_FORMAT_gr1616 = 0x32335247,
  UT_WAYLAND_SHM_FORMAT_xrgb16161616f = 0x48345258,
  UT_WAYLAND_SHM_FORMAT_xbgr16161616f = 0x48344258,
  UT_WAYLAND_SHM_FORMAT_argb16161616f = 0x48345241,
  UT_WAYLAND_SHM_FORMAT_abgr16161616f = 0x48344241,
  UT_WAYLAND_SHM_FORMAT_xyuv8888 = 0x56555958,
  UT_WAYLAND_SHM_FORMAT_vuy888 = 0x34325556,
  UT_WAYLAND_SHM_FORMAT_vuy101010 = 0x30335556,
  UT_WAYLAND_SHM_FORMAT_y210 = 0x30313259,
  UT_WAYLAND_SHM_FORMAT_y212 = 0x32313259,
  UT_WAYLAND_SHM_FORMAT_y216 = 0x36313259,
  UT_WAYLAND_SHM_FORMAT_y410 = 0x30313459,
  UT_WAYLAND_SHM_FORMAT_y412 = 0x32313459,
  UT_WAYLAND_SHM_FORMAT_y416 = 0x36313459,
  UT_WAYLAND_SHM_FORMAT_xvyu2101010 = 0x30335658,
  UT_WAYLAND_SHM_FORMAT_xvyu12_16161616 = 0x36335658,
  UT_WAYLAND_SHM_FORMAT_xvyu16161616 = 0x38345658,
  UT_WAYLAND_SHM_FORMAT_y0l0 = 0x304c3059,
  UT_WAYLAND_SHM_FORMAT_x0l0 = 0x304c3058,
  UT_WAYLAND_SHM_FORMAT_y0l2 = 0x324c3059,
  UT_WAYLAND_SHM_FORMAT_x0l2 = 0x324c3058,
  UT_WAYLAND_SHM_FORMAT_yuv420_8bit = 0x38305559,
  UT_WAYLAND_SHM_FORMAT_yuv420_10bit = 0x30315559,
  UT_WAYLAND_SHM_FORMAT_xrgb8888_a8 = 0x38415258,
  UT_WAYLAND_SHM_FORMAT_xbgr8888_a8 = 0x38414258,
  UT_WAYLAND_SHM_FORMAT_rgbx8888_a8 = 0x38415852,
  UT_WAYLAND_SHM_FORMAT_bgrx8888_a8 = 0x38415842,
  UT_WAYLAND_SHM_FORMAT_rgb888_a8 = 0x38413852,
  UT_WAYLAND_SHM_FORMAT_bgr888_a8 = 0x38413842,
  UT_WAYLAND_SHM_FORMAT_rgb565_a8 = 0x38413552,
  UT_WAYLAND_SHM_FORMAT_bgr565_a8 = 0x38413542,
  UT_WAYLAND_SHM_FORMAT_nv24 = 0x3432564e,
  UT_WAYLAND_SHM_FORMAT_nv42 = 0x3234564e,
  UT_WAYLAND_SHM_FORMAT_p210 = 0x30313250,
  UT_WAYLAND_SHM_FORMAT_p010 = 0x30313050,
  UT_WAYLAND_SHM_FORMAT_p012 = 0x32313050,
  UT_WAYLAND_SHM_FORMAT_p016 = 0x36313050,
  UT_WAYLAND_SHM_FORMAT_axbxgxrx106106106106 = 0x30314241,
  UT_WAYLAND_SHM_FORMAT_nv15 = 0x3531564e,
  UT_WAYLAND_SHM_FORMAT_q410 = 0x30313451,
  UT_WAYLAND_SHM_FORMAT_q401 = 0x31303451,
  UT_WAYLAND_SHM_FORMAT_xrgb16161616 = 0x38345258,
  UT_WAYLAND_SHM_FORMAT_xbgr16161616 = 0x38344258,
  UT_WAYLAND_SHM_FORMAT_argb16161616 = 0x38345241,
  UT_WAYLAND_SHM_FORMAT_abgr16161616 = 0x38344241
} UtWaylandShmFormat;

/// Creates a new Wayland shared memory object with [id] on [client].
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandShm
UtObject *ut_wayland_shm_new(UtObject *client, uint32_t id);

/// Creates a new Wayland shared memory object with well known [name] from
/// [registry].
///
/// !arg-type registry UtWaylandRegistry
/// !return-ref
/// !return-type UtWaylandShm
UtObject *ut_wayland_shm_new_from_registry(UtObject *registry, uint32_t name);

/// Returns the supported image formats.
///
/// !return-type UtUint32List
UtObject *ut_wayland_shm_get_formats(UtObject *object);

/// Returns true if image [format] is supported.
bool ut_wayland_shm_has_format(UtObject *object, uint32_t format);

/// Creates a shared memory pool using [fd] of [size] bytes.
///
/// !arg-type fd UtFileDescriptor
/// !return-ref
/// !return-type UtWaylandShmPool
UtObject *ut_wayland_shm_create_pool(UtObject *object, UtObject *fd,
                                     int32_t size);

/// Returns [true] if [object] is a [UtWaylandShm].
bool ut_object_is_wayland_shm(UtObject *object);
