#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef enum {
  UT_EXIF_TAG_EXIF_VERSION = 0x9000,
  UT_EXIF_TAG_FLASHPIX_VERSION = 0xa000,
  UT_EXIF_TAG_COLOR_SPACE = 0xa001,
  UT_EXIF_TAG_GAMMA = 0xa500,
  UT_EXIF_TAG_COMPONENTS_CONFIGURATION = 0x9101,
  UT_EXIF_TAG_COMPRESSED_BITS_PER_PIXEL = 0x9102,
  UT_EXIF_TAG_PIXEL_X_DIMENSIONS = 0xa002,
  UT_EXIF_TAG_PIXEL_Y_DIMENSIONS = 0xa003,
  UT_EXIF_TAG_MAKER_NOTE = 0x927c,
  UT_EXIF_TAG_USER_COMMENT = 0x9286,
  UT_EXIF_TAG_RELATED_SOUND_FILE = 0xa004,
  UT_EXIF_TAG_DATE_TIME_ORIGINAL = 0x9003,
  UT_EXIF_TAG_DATE_TIME_DIGITIZED = 0x9004,
  UT_EXIF_TAG_SUB_SEC_TIME = 0x9290,
  UT_EXIF_TAG_SUB_SEC_TIME_ORIGINAL = 0x9291,
  UT_EXIF_TAG_SUB_SEC_TIME_DIGITIZED = 0x9292,
  UT_EXIF_TAG_IMAGE_UNIQUE_ID = 0xa420,
  UT_EXIF_TAG_CAMERA_OWNER_NAME = 0xa430,
  UT_EXIF_TAG_BODY_SERIAL_NUMBER = 0xa431,
  UT_EXIF_TAG_LENS_SPECIFICATION = 0xa432,
  UT_EXIF_TAG_LENS_MAKE = 0xa433,
  UT_EXIF_TAG_LENS_MODEL = 0xa434,
  UT_EXIF_TAG_LENS_SERIAL_NUMBER = 0xa435,
  UT_EXIF_TAG_EXPOSURE_TIME = 0x829a,
  UT_EXIF_TAG_F_NUMBER = 0x829d,
  UT_EXIF_TAG_EXPOSURE_PROGRAM = 0x8822,
  UT_EXIF_TAG_SPECTRAL_SENSITIVITY = 0x8824,
  UT_EXIF_TAG_PHOTOGRAPHIC_SENSITIVITY = 0x8827,
  UT_EXIF_TAG_OECF = 0x8828,
  UT_EXIF_TAG_SENSITIVITY_TYPE = 0x8830,
  UT_EXIF_TAG_STANDARD_OUTPUT_SENSITIVITY = 0x8831,
  UT_EXIF_TAG_RECOMMENDED_EXPOSURE_INDEX = 0x8832,
  UT_EXIF_TAG_ISO_SPEED = 0x8833,
  UT_EXIF_TAG_ISO_SPEED_LATITUDE_YYY = 0x8834,
  UT_EXIF_TAG_ISO_SPEED_LATITUDE_ZZZ = 0x8835,
  UT_EXIF_TAG_SHUTTER_SPEED_VALUE = 0x9201,
  UT_EXIF_TAG_APERTURE_VALUE = 0x9202,
  UT_EXIF_TAG_BRIGHTNESS_VALUE = 0x9203,
  UT_EXIF_TAG_EXPOSURE_BIAS_VALUE = 0x9204,
  UT_EXIF_TAG_MAX_APERTURE_VALUE = 0x9205,
  UT_EXIF_TAG_SUBJECT_DISTANCE = 0x9206,
  UT_EXIF_TAG_METERING_MODE = 0x9207,
  UT_EXIF_TAG_LIGHT_SOURCE = 0x9208,
  UT_EXIF_TAG_FLASH = 0x9209,
  UT_EXIF_TAG_FOCAL_LENGTH = 0x920a,
  UT_EXIF_TAG_SUBJECT_AREA = 0x9214,
  UT_EXIF_TAG_FLASH_ENERGY = 0xa20b,
  UT_EXIF_TAG_SPATIAL_FREQUENCY_RESPONSE = 0xa20c,
  UT_EXIF_TAG_FOCAL_PLANE_X_RESOLUTION = 0xa20e,
  UT_EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION = 0xa20f,
  UT_EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT = 0xa210,
  UT_EXIF_TAG_SUBJECT_LOCATION = 0xa214,
  UT_EXIF_TAG_EXPOSURE_INDEX = 0xa215,
  UT_EXIF_TAG_SENSING_METHOD = 0xa217,
  UT_EXIF_TAG_FILE_SOURCE = 0xa300,
  UT_EXIF_TAG_SCENE_TYPE = 0xa301,
  UT_EXIF_TAG_CFA_PATTERN = 0xa302,
  UT_EXIF_TAG_CUSTOM_RENDERED = 0xa401,
  UT_EXIF_TAG_EXPOSURE_MODE = 0xa402,
  UT_EXIF_TAG_WHITE_BALANCE = 0xa403,
  UT_EXIF_TAG_DIGITAL_ZOOM_RATIO = 0xa404,
  UT_EXIF_TAG_FOCAL_LENGTH_IN_35_MM_FILM = 0xa405,
  UT_EXIF_TAG_SCENE_CAPTURE_TYPE = 0xa406,
  UT_EXIF_TAG_GAIN_CONTROL = 0xa407,
  UT_EXIF_TAG_CONTRAST = 0xa408,
  UT_EXIF_TAG_SATURATION = 0xa409,
  UT_EXIF_TAG_SHARPNESS = 0xa40a,
  UT_EXIF_TAG_DEVICE_SETTING_DESCRIPTION = 0xa40b,
  UT_EXIF_TAG_SUBJECT_DISTANCE_RANGE = 0xa40c
} UtExifTag;

UtObject *ut_exif_metadata_new();

UtObject *ut_exif_metadata_new_from_data(UtObject *data);

const char *ut_exif_metadata_get_exif_version(UtObject *object);

const char *ut_exif_metadata_get_flashpix_version(UtObject *object);

bool ut_object_is_exif_metadata(UtObject *object);
