#pragma once

typedef enum {
  UT_PNG_CHUNK_TYPE_IMAGE_HEADER = 0x49484452,
  UT_PNG_CHUNK_TYPE_PALETTE = 0x504c5445,
  UT_PNG_CHUNK_TYPE_IMAGE_DATA = 0x49444154,
  UT_PNG_CHUNK_TYPE_IMAGE_END = 0x49454e44,
  UT_PNG_CHUNK_TYPE_CHROMATICITIES = 0x6348524d,
  UT_PNG_CHUNK_TYPE_GAMMA = 0x67414d41,
  UT_PNG_CHUNK_TYPE_ICC_PROFILE = 0x69434350,
  UT_PNG_CHUNK_TYPE_SIGNIFICANT_BITS = 0x73424954,
  UT_PNG_CHUNK_TYPE_STANDARD_RGB = 0x73524742,
  UT_PNG_CHUNK_TYPE_BACKGROUND = 0x624b4744,
  UT_PNG_CHUNK_TYPE_HISTOGRAM = 0x68495354,
  UT_PNG_CHUNK_TYPE_TRANSPARENCY = 0x74524e53,
  UT_PNG_CHUNK_TYPE_EXIF = 0x65584966,
  UT_PNG_CHUNK_TYPE_PHYSICAL_DIMENSIONS = 0x70485973,
  UT_PNG_CHUNK_TYPE_SUGGESTED_PALETTE = 0x73504c54,
  UT_PNG_CHUNK_TYPE_MODIFICATION_TIME = 0x74494d45,
  UT_PNG_CHUNK_TYPE_INTERNATIONAL_TEXT = 0x69545874,
  UT_PNG_CHUNK_TYPE_TEXT = 0x74455874,
  UT_PNG_CHUNK_TYPE_COMPRESSED_TEXT = 0x7a545874
} UtPngChunkType;

typedef enum {
  UT_PNG_INTERLACE_METHOD_NONE,
  UT_PNG_INTERLACE_METHOD_ADAM7
} UtPngInterlaceMethod;
