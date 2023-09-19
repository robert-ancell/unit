#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Compression level used in ZLib encoding:
/// - [UT_ZLIB_COMPRESSION_LEVEL_FASTEST] - fastest compression.
/// - [UT_ZLIB_COMPRESSION_LEVEL_FAST] - fast compression.
/// - [UT_ZLIB_COMPRESSION_LEVEL_DEFAULT] - default compression.
/// - [UT_ZLIB_COMPRESSION_LEVEL_MAXIMUM] - maximum compression.
typedef enum {
  UT_ZLIB_COMPRESSION_LEVEL_FASTEST = 0,
  UT_ZLIB_COMPRESSION_LEVEL_FAST = 1,
  UT_ZLIB_COMPRESSION_LEVEL_DEFAULT = 2,
  UT_ZLIB_COMPRESSION_LEVEL_MAXIMUM = 3
} UtZlibCompressionLevel;
