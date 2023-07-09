#include <stdio.h>

#include "ut.h"

static void done(int return_code) {
  UtObjectRef return_value = ut_int32_new(return_code);
  ut_event_loop_return(return_value);
}

static size_t decode_cb(UtObject *object, UtObject *data, bool complete) {
  size_t data_length = ut_list_get_length(data);

  UtObjectRef image = ut_tiff_image_new_from_data(data);
  if (ut_object_implements_error(image)) {
    printf("Failed to decode: %s\n", ut_error_get_description(image));
    done(1);
    return 0;
  }

  size_t width = ut_tiff_image_get_width(image);
  size_t length = ut_tiff_image_get_length(image);
  UtTiffPhotometricInterpretation photometric_interpretation =
      ut_tiff_image_get_photometric_interpretation(image);
  size_t samples_per_pixel = ut_tiff_image_get_samples_per_pixel(image);
  size_t bits_per_sample = ut_tiff_image_get_bits_per_sample(image);
  size_t row_stride = ut_tiff_image_get_row_stride(image);
  const uint8_t *image_data =
      ut_uint8_list_get_data(ut_tiff_image_get_data(image));

  size_t n_used_samples = samples_per_pixel;
  switch (photometric_interpretation) {
  case UT_TIFF_PHOTOMETRIC_INTERPRETATION_WHITE_IS_ZERO:
  case UT_TIFF_PHOTOMETRIC_INTERPRETATION_BLACK_IS_ZERO:
    n_used_samples = 1;
    printf("P2\n");
    break;
  case UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB:
    n_used_samples = 3;
    printf("P3\n");
    break;
  default:
    done(1);
    return data_length;
  }
  printf("%zi %zi\n", width, length);
  printf("%d\n", (1 << bits_per_sample) - 1);
  for (size_t y = 0; y < length; y++) {
    size_t row_start = y * row_stride;
    for (size_t x = 0; x < width; x++) {
      if (x != 0) {
        printf(" ");
      }
      for (size_t c = 0; c < n_used_samples; c++) {
        uint16_t sample = 0;
        size_t s = (x * samples_per_pixel) + c;
        switch (bits_per_sample) {
        case 1:
          sample = image_data[row_start + (s / 8)] >> (7 - (s % 8)) & 0x1;
          break;
        case 2:
          sample = image_data[row_start + (s / 4)] >> (6 - (s % 4) * 2) & 0x3;
          break;
        case 4:
          sample = image_data[row_start + (s / 2)] >> (4 - (s % 2) * 4) & 0xf;
          break;
        case 8:
          sample = image_data[row_start + s];
          break;
        case 16:
          sample = image_data[row_start + (s * 2)] << 8 |
                   image_data[row_start + (s * 2) + 1];
          break;
        }

        if (c != 0) {
          printf(" ");
        }
        printf("%d", sample);
      }
    }
    printf("\n");
  }

  done(0);

  return data_length;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: tiff2pnm [file.tiff]\n");
    return 1;
  }

  UtObjectRef file = ut_local_file_new(argv[1]);
  ut_file_open_read(file);
  UtObjectRef dummy_object = ut_null_new();
  ut_input_stream_read_all(file, dummy_object, decode_cb);

  UtObjectRef return_code = ut_event_loop_run();
  return ut_int32_get_value(return_code);
}
