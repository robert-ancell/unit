#include <assert.h>

#include "ut.h"

// https://www.cipa.jp/std/documents/e/DC-008-2012_E.pdf

typedef struct {
  UtObject object;

  char *exif_version;
  char *flashpix_version;
  char *image_unique_id;
  char *camera_owner_name;
  char *body_serial_number;
  char *lens_make;
  char *lens_model;
  char *lens_serial_number;
} UtExifMetadata;

static bool validate_tag(UtObject *tag) {
  uint16_t id = ut_tiff_tag_get_id(tag);
  uint16_t type = ut_tiff_tag_get_type(tag);
  size_t count = ut_list_get_length(ut_tiff_tag_get_data(tag));
  printf("%04x %d %zi\n", id, type, count);
  switch (id) {
  case UT_EXIF_TAG_EXIF_VERSION:
    return type == UT_TIFF_TAG_TYPE_UNDEFINED && count == 4;
  case UT_EXIF_TAG_FLASHPIX_VERSION:
    return type == UT_TIFF_TAG_TYPE_UNDEFINED && count == 4;
  case UT_EXIF_TAG_COLOR_SPACE:
    return type == UT_TIFF_TAG_TYPE_SHORT && count == 1;
  case UT_EXIF_TAG_GAMMA:
    return type == UT_TIFF_TAG_TYPE_RATIONAL && count == 1;
  case UT_EXIF_TAG_COMPONENTS_CONFIGURATION:
    return false;
  case UT_EXIF_TAG_COMPRESSED_BITS_PER_PIXEL:
    return false;
  case UT_EXIF_TAG_PIXEL_X_DIMENSIONS:
    return false;
  case UT_EXIF_TAG_PIXEL_Y_DIMENSIONS:
    return false;
  case UT_EXIF_TAG_MAKER_NOTE:
    return type == UT_TIFF_TAG_TYPE_UNDEFINED;
  case UT_EXIF_TAG_USER_COMMENT:
    return type == UT_TIFF_TAG_TYPE_UNDEFINED;
  case UT_EXIF_TAG_RELATED_SOUND_FILE:
    return type == UT_TIFF_TAG_TYPE_ASCII && count == 13;
  case UT_EXIF_TAG_DATE_TIME_ORIGINAL:
    return type == UT_TIFF_TAG_TYPE_ASCII && count == 20;
  case UT_EXIF_TAG_DATE_TIME_DIGITIZED:
    return type == UT_TIFF_TAG_TYPE_ASCII && count == 20;
  case UT_EXIF_TAG_SUB_SEC_TIME:
    return type == UT_TIFF_TAG_TYPE_ASCII;
  case UT_EXIF_TAG_SUB_SEC_TIME_ORIGINAL:
    return type == UT_TIFF_TAG_TYPE_ASCII;
  case UT_EXIF_TAG_SUB_SEC_TIME_DIGITIZED:
    return type == UT_TIFF_TAG_TYPE_ASCII;
  case UT_EXIF_TAG_IMAGE_UNIQUE_ID:
    return type == UT_TIFF_TAG_TYPE_ASCII && count == 33;
  case UT_EXIF_TAG_CAMERA_OWNER_NAME:
    return type == UT_TIFF_TAG_TYPE_ASCII;
  case UT_EXIF_TAG_BODY_SERIAL_NUMBER:
    return type == UT_TIFF_TAG_TYPE_ASCII;
  case UT_EXIF_TAG_LENS_SPECIFICATION:
    return false;
  case UT_EXIF_TAG_LENS_MAKE:
    return type == UT_TIFF_TAG_TYPE_ASCII;
  case UT_EXIF_TAG_LENS_MODEL:
    return type == UT_TIFF_TAG_TYPE_ASCII;
  case UT_EXIF_TAG_LENS_SERIAL_NUMBER:
    return type == UT_TIFF_TAG_TYPE_ASCII;
  case UT_EXIF_TAG_EXPOSURE_TIME:
    return type == UT_TIFF_TAG_TYPE_RATIONAL && count == 1;
  case UT_EXIF_TAG_F_NUMBER:
    return type == UT_TIFF_TAG_TYPE_RATIONAL && count == 1;
  case UT_EXIF_TAG_EXPOSURE_PROGRAM:
    return type == UT_TIFF_TAG_TYPE_SHORT && count == 1;
  case UT_EXIF_TAG_SPECTRAL_SENSITIVITY:
    return false;
  case UT_EXIF_TAG_PHOTOGRAPHIC_SENSITIVITY:
    return false;
  case UT_EXIF_TAG_OECF:
    return false;
  case UT_EXIF_TAG_SENSITIVITY_TYPE:
    return false;
  case UT_EXIF_TAG_STANDARD_OUTPUT_SENSITIVITY:
    return false;
  case UT_EXIF_TAG_RECOMMENDED_EXPOSURE_INDEX:
    return false;
  case UT_EXIF_TAG_ISO_SPEED:
    return false;
  case UT_EXIF_TAG_ISO_SPEED_LATITUDE_YYY:
    return false;
  case UT_EXIF_TAG_ISO_SPEED_LATITUDE_ZZZ:
    return false;
  case UT_EXIF_TAG_SHUTTER_SPEED_VALUE:
    return false;
  case UT_EXIF_TAG_APERTURE_VALUE:
    return false;
  case UT_EXIF_TAG_BRIGHTNESS_VALUE:
    return false;
  case UT_EXIF_TAG_EXPOSURE_BIAS_VALUE:
    return false;
  case UT_EXIF_TAG_MAX_APERTURE_VALUE:
    return false;
  case UT_EXIF_TAG_SUBJECT_DISTANCE:
    return false;
  case UT_EXIF_TAG_METERING_MODE:
    return false;
  case UT_EXIF_TAG_LIGHT_SOURCE:
    return false;
  case UT_EXIF_TAG_FLASH:
    return false;
  case UT_EXIF_TAG_FOCAL_LENGTH:
    return false;
  case UT_EXIF_TAG_SUBJECT_AREA:
    return false;
  case UT_EXIF_TAG_FLASH_ENERGY:
    return false;
  case UT_EXIF_TAG_SPATIAL_FREQUENCY_RESPONSE:
    return false;
  case UT_EXIF_TAG_FOCAL_PLANE_X_RESOLUTION:
    return false;
  case UT_EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION:
    return false;
  case UT_EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT:
    return false;
  case UT_EXIF_TAG_SUBJECT_LOCATION:
    return false;
  case UT_EXIF_TAG_EXPOSURE_INDEX:
    return false;
  case UT_EXIF_TAG_SENSING_METHOD:
    return false;
  case UT_EXIF_TAG_FILE_SOURCE:
    return false;
  case UT_EXIF_TAG_SCENE_TYPE:
    return false;
  case UT_EXIF_TAG_CFA_PATTERN:
    return false;
  case UT_EXIF_TAG_CUSTOM_RENDERED:
    return false;
  case UT_EXIF_TAG_EXPOSURE_MODE:
    return false;
  case UT_EXIF_TAG_WHITE_BALANCE:
    return false;
  case UT_EXIF_TAG_DIGITAL_ZOOM_RATIO:
    return type == UT_TIFF_TAG_TYPE_RATIONAL && count == 1;
  case UT_EXIF_TAG_FOCAL_LENGTH_IN_35_MM_FILM:
    return type == UT_TIFF_TAG_TYPE_SHORT && count == 1;
  case UT_EXIF_TAG_SCENE_CAPTURE_TYPE:
    return type == UT_TIFF_TAG_TYPE_SHORT && count == 1;
  case UT_EXIF_TAG_GAIN_CONTROL:
    return type == UT_TIFF_TAG_TYPE_RATIONAL && count == 1;
  case UT_EXIF_TAG_CONTRAST:
    return type == UT_TIFF_TAG_TYPE_SHORT && count == 1;
  case UT_EXIF_TAG_SATURATION:
    return type == UT_TIFF_TAG_TYPE_SHORT && count == 1;
  case UT_EXIF_TAG_SHARPNESS:
    return type == UT_TIFF_TAG_TYPE_SHORT && count == 1;
  case UT_EXIF_TAG_DEVICE_SETTING_DESCRIPTION:
    return type == UT_TIFF_TAG_TYPE_UNDEFINED;
  case UT_EXIF_TAG_SUBJECT_DISTANCE_RANGE:
    return type == UT_TIFF_TAG_TYPE_SHORT && count == 1;
  default:
    return true;
  }
}

static void ut_exif_metadata_init(UtObject *object) {
  UtExifMetadata *self = (UtExifMetadata *)object;

  self->exif_version = ut_cstring_new("0230");
  self->flashpix_version = ut_cstring_new("0100");
}

static void ut_exif_metadata_cleanup(UtObject *object) {
  UtExifMetadata *self = (UtExifMetadata *)object;

  free(self->exif_version);
  free(self->flashpix_version);
  free(self->image_unique_id);
  free(self->camera_owner_name);
  free(self->body_serial_number);
  free(self->lens_make);
  free(self->lens_model);
  free(self->lens_serial_number);
}

static char *ut_exif_metadata_to_string(UtObject *object) {
  return ut_cstring_new_printf("<UtExifMetadata>()");
}

static UtObjectInterface object_interface = {
    .type_name = "UtExifMetadata",
    .init = ut_exif_metadata_init,
    .cleanup = ut_exif_metadata_cleanup,
    .to_string = ut_exif_metadata_to_string,
    .interfaces = {{NULL, NULL}}};

UtObject *ut_exif_metadata_new() {
  UtObject *object = ut_object_new(sizeof(UtExifMetadata), &object_interface);
  return object;
}

UtObject *ut_exif_metadata_new_from_data(UtObject *data) {
  UtObjectRef object = ut_object_new(sizeof(UtExifMetadata), &object_interface);

  UtObjectRef reader = ut_tiff_reader_new(data);
  UtObject *tags = ut_tiff_reader_get_tags(reader);
  if (tags == NULL) {
    ut_cstring_ref description = ut_cstring_new_printf(
        "Invalid Exif data: %s",
        ut_error_get_description(ut_tiff_reader_get_error(reader)));
    return ut_exif_error_new(description);
  }

  UtObject *tag = ut_tiff_reader_get_tag(reader, UT_TIFF_TAG_EXIF_IFD);
  if (tag == NULL) {
    return ut_exif_error_new("Exif data missing IFD tag");
  }

  UtObjectRef exif_reader =
      ut_tiff_reader_get_subfile(reader, ut_tiff_tag_get_long(tag));
  UtObject *exif_tags = ut_tiff_reader_get_tags(exif_reader);
  if (exif_tags == NULL) {
    ut_cstring_ref description = ut_cstring_new_printf(
        "Invalid Exif data: %s",
        ut_error_get_description(ut_tiff_reader_get_error(reader)));
    return ut_exif_error_new(description);
  }
  printf("%s\n", ut_object_to_string(exif_tags));
  size_t exif_tags_length = ut_list_get_length(exif_tags);
  for (size_t i = 0; i < exif_tags_length; i++) {
    UtObjectRef tag = ut_list_get_element(exif_tags, i);
    if (!validate_tag(tag)) {
      return ut_exif_error_new("Invalid Exif tag");
    }
  }

  return ut_object_ref(object);
}

const char *ut_exif_metadata_get_exif_version(UtObject *object) {
  assert(ut_object_is_exif_metadata(object));
  UtExifMetadata *self = (UtExifMetadata *)object;
  return self->exif_version;
}

const char *ut_exif_metadata_get_flashpix_version(UtObject *object) {
  assert(ut_object_is_exif_metadata(object));
  UtExifMetadata *self = (UtExifMetadata *)object;
  return self->flashpix_version;
}

bool ut_object_is_exif_metadata(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
