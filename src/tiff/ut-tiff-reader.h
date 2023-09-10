#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_tiff_reader_new(UtObject *data);

UtObject *ut_tiff_reader_get_tags(UtObject *object);

UtObject *ut_tiff_reader_get_tag(UtObject *object, uint16_t id);

uint32_t ut_tiff_reader_get_image_width(UtObject *object);

uint32_t ut_tiff_reader_get_image_length(UtObject *object);

uint16_t ut_tiff_reader_get_bits_per_sample(UtObject *object);

uint16_t ut_tiff_reader_get_compression(UtObject *object);

uint16_t ut_tiff_reader_get_photometric_interpretation(UtObject *object);

uint16_t ut_tiff_reader_get_thresholding(UtObject *object);

uint16_t ut_tiff_reader_get_cell_width(UtObject *object);

uint16_t ut_tiff_reader_get_cell_length(UtObject *object);

uint16_t ut_tiff_reader_get_fill_order(UtObject *object);

char *ut_tiff_reader_get_document_name(UtObject *object);

char *ut_tiff_reader_get_image_description(UtObject *object);

char *ut_tiff_reader_get_make(UtObject *object);

char *ut_tiff_reader_get_model(UtObject *object);

uint16_t ut_tiff_reader_get_orientation(UtObject *object);

uint16_t ut_tiff_reader_get_samples_per_pixel(UtObject *object);

uint32_t ut_tiff_reader_get_rows_per_strip(UtObject *object);

uint16_t ut_tiff_reader_get_planar_configuration(UtObject *object);

char *ut_tiff_reader_get_page_name(UtObject *object);

uint16_t ut_tiff_reader_get_gray_response_unit(UtObject *object);

uint16_t ut_tiff_reader_get_resolution_unit(UtObject *object);

uint16_t ut_tiff_reader_get_page_number(UtObject *object,
                                        uint16_t *total_pages);

char *ut_tiff_reader_get_software(UtObject *object);

char *ut_tiff_reader_get_date_time(UtObject *object);

char *ut_tiff_reader_get_artist(UtObject *object);

char *ut_tiff_reader_get_host_computer(UtObject *object);

uint32_t ut_tiff_reader_get_tile_width(UtObject *object);

uint32_t ut_tiff_reader_get_tile_length(UtObject *object);

uint16_t ut_tiff_reader_get_ink_set(UtObject *object);

char *ut_tiff_reader_get_ink_names(UtObject *object);

char *ut_tiff_reader_get_target_printer(UtObject *object);

char *ut_tiff_reader_get_copyright(UtObject *object);

UtObject *ut_tiff_reader_get_subfile(UtObject *object, uint32_t offset);

UtObject *ut_tiff_reader_get_error(UtObject *object);

/// Returns [true] if [object] is a [UtTiffReader].
bool ut_object_is_tiff_reader(UtObject *object);
