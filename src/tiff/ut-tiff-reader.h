#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TIFF reader to decode [data].
///
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtTiffReader
UtObject *ut_tiff_reader_new(UtObject *data);

/// Returns the tags read.
///
/// !return-type UtObjectList
UtObject *ut_tiff_reader_get_tags(UtObject *object);

/// Returns the first tag that matches [id] or [NULL] if none.
///
/// !return-type UtTiffTag NULL
UtObject *ut_tiff_reader_lookup_tag(UtObject *object, uint16_t id);

/// Returns the width of the image in pixels.
uint32_t ut_tiff_reader_get_image_width(UtObject *object);

/// Returns the length of the image in pixels.
uint32_t ut_tiff_reader_get_image_length(UtObject *object);

/// Returns the number of bits per pixel sample.
uint16_t ut_tiff_reader_get_bits_per_sample(UtObject *object);

/// Returns the compression used in the image.
/// See [UtTiffCompression] for standard values.
uint16_t ut_tiff_reader_get_compression(UtObject *object);

/// Returns the method used to convert the samples to color values.
/// See [UtTiffPhotometricInterpretation] for standard values.
uint16_t ut_tiff_reader_get_photometric_interpretation(UtObject *object);

/// The technique used to convert grey to black and white pixels.
uint16_t ut_tiff_reader_get_thresholding(UtObject *object);

/// Returns the width of a cell in the image.
uint16_t ut_tiff_reader_get_cell_width(UtObject *object);

/// Returns the length of a cell in the image.
uint16_t ut_tiff_reader_get_cell_length(UtObject *object);

/// Returns the logical order of bits within a byte.
uint16_t ut_tiff_reader_get_fill_order(UtObject *object);

/// Returns the name of the document.
char *ut_tiff_reader_get_document_name(UtObject *object);

/// Returns the description of the image.
char *ut_tiff_reader_get_image_description(UtObject *object);

/// Returns the make of the device that generated the image.
char *ut_tiff_reader_get_make(UtObject *object);

/// Returns the model of the device that generated the image.
char *ut_tiff_reader_get_model(UtObject *object);

/// Returns the orientation of the image.
uint16_t ut_tiff_reader_get_orientation(UtObject *object);

/// Returns the number of samples in each pixel.
uint16_t ut_tiff_reader_get_samples_per_pixel(UtObject *object);

/// Returns the number of rows per strip.
uint32_t ut_tiff_reader_get_rows_per_strip(UtObject *object);

/// Returns how the components of each pixel are stored.
uint16_t ut_tiff_reader_get_planar_configuration(UtObject *object);

/// Returns the name of the page.
char *ut_tiff_reader_get_page_name(UtObject *object);

/// Returns the units for X an Y resolution values.
uint16_t ut_tiff_reader_get_resolution_unit(UtObject *object);

/// The number of the page this image was scanned from.
/// [total_pages] is set to the total name of pages in the document this was
/// scanned from.
uint16_t ut_tiff_reader_get_page_number(UtObject *object,
                                        uint16_t *total_pages);

/// Returns the name of the software that generated the image.
char *ut_tiff_reader_get_software(UtObject *object);

/// Returns the date and time of image creation in the format "YYYY:MM:DD
/// HH:MM:SS".
char *ut_tiff_reader_get_date_time(UtObject *object);

/// Returns the name of the arists who created the image.
char *ut_tiff_reader_get_artist(UtObject *object);

/// Returns the name of host computer that generated the image.
char *ut_tiff_reader_get_host_computer(UtObject *object);

/// Returns the width of a tile in the image.
uint32_t ut_tiff_reader_get_tile_width(UtObject *object);

/// Returns the length of a tile in the image.
uint32_t ut_tiff_reader_get_tile_length(UtObject *object);

/// Returns the set of inks used in a
/// [UT_TIFF_PHOTOMETRIC_INTERPRETATION_SEPERATED] image.
uint16_t ut_tiff_reader_get_ink_set(UtObject *object);

/// Returns the names of each ink used in a
/// [UT_TIFF_PHOTOMETRIC_INTERPRETATION_SEPERATED] image.
///
/// !return-ref
/// !return-type UtStringList
UtObject *ut_tiff_reader_get_ink_names(UtObject *object);

/// Returns the number of inks.
uint16_t ut_tiff_reader_get_number_of_inks(UtObject *object);

/// Returns a description of the printing environment in a
/// [UT_TIFF_PHOTOMETRIC_INTERPRETATION_SEPERATED] image.
char *ut_tiff_reader_get_target_printer(UtObject *object);

/// Returns the copyright assigned to the image.
char *ut_tiff_reader_get_copyright(UtObject *object);

/// Returns a sub file at position [offset] in this file.
///
/// !return-type UtTiffReaer
UtObject *ut_tiff_reader_get_subfile(UtObject *object, uint32_t offset);

/// Returns the first error that occurred during decoding of [NULL] if no error.
///
/// !return-type UtTiffError NULL
UtObject *ut_tiff_reader_get_error(UtObject *object);

/// Returns [true] if [object] is a [UtTiffReader].
bool ut_object_is_tiff_reader(UtObject *object);
