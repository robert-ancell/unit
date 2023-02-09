#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef enum {
  UT_ZLIB_COMPRESSION_LEVEL_FASTEST = 0,
  UT_ZLIB_COMPRESSION_LEVEL_FAST = 1,
  UT_ZLIB_COMPRESSION_LEVEL_DEFAULT = 2,
  UT_ZLIB_COMPRESSION_LEVEL_MAXIMUM = 3
} UtZlibCompressionLevel;
