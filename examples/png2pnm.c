#include <stdio.h>

#include "ut.h"

static void decode_cb(void *user_data) {
  UtObject *decoder = user_data;

  UtObject *image = ut_png_decoder_get_image(decoder);
  if (image == NULL) {
    printf("Failed to decode: %s\n",
           ut_error_get_description(ut_png_decoder_get_error(decoder)));
    UtObjectRef return_code = ut_int32_new(1);
    ut_event_loop_return(return_code);
    return;
  }

  size_t width = ut_png_image_get_width(image);
  size_t height = ut_png_image_get_height(image);
  UtPngColorType color_type = ut_png_image_get_color_type(image);
  size_t n_channels = ut_png_image_get_n_channels(image);
  size_t bit_depth = ut_png_image_get_bit_depth(image);
  size_t row_stride = ut_png_image_get_row_stride(image);
  const uint8_t *image_data =
      ut_uint8_list_get_data(ut_png_image_get_data(image));
  UtObject *palette = ut_png_image_get_palette(image);
  const uint8_t *palette_data =
      palette != NULL ? ut_uint8_list_get_data(palette) : NULL;

  // Drop alpha channel.
  size_t n_used_channels = n_channels;
  if (color_type == UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA ||
      color_type == UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA) {
    n_used_channels--;
  }

  switch (color_type) {
  case UT_PNG_COLOR_TYPE_GREYSCALE:
  case UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA:
    printf("P2\n");
    break;
  case UT_PNG_COLOR_TYPE_INDEXED_COLOR:
  case UT_PNG_COLOR_TYPE_TRUECOLOR:
  case UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA:
    printf("P3\n");
    break;
  }
  printf("%zi %zi\n", width, height);
  switch (color_type) {
  case UT_PNG_COLOR_TYPE_GREYSCALE:
  case UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA:
  case UT_PNG_COLOR_TYPE_TRUECOLOR:
  case UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA:
    printf("%d\n", (1 << bit_depth) - 1);
    break;
  case UT_PNG_COLOR_TYPE_INDEXED_COLOR:
    printf("255\n");
    break;
  }
  for (size_t y = 0; y < height; y++) {
    size_t row_start = y * row_stride;
    for (size_t x = 0; x < width; x++) {
      if (x != 0) {
        printf(" ");
      }
      for (size_t c = 0; c < n_used_channels; c++) {
        uint16_t sample = 0;
        size_t s = (x * n_channels) + c;
        switch (bit_depth) {
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
        if (color_type == UT_PNG_COLOR_TYPE_INDEXED_COLOR) {
          uint8_t r = palette_data[(sample * 3) + 0];
          uint8_t g = palette_data[(sample * 3) + 1];
          uint8_t b = palette_data[(sample * 3) + 2];
          printf("%d %d %d", r, g, b);
        } else {
          printf("%d", sample);
        }
      }
    }
    printf("\n");
  }

  UtObjectRef return_code = ut_int32_new(0);
  ut_event_loop_return(return_code);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: png2pnm [file.png]\n");
    return 1;
  }

  UtObjectRef file = ut_local_file_new(argv[1]);
  ut_file_open_read(file);
  UtObjectRef decoder = ut_png_decoder_new(file);
  ut_png_decoder_decode(decoder, decode_cb, decoder, NULL);

  UtObjectRef return_code = ut_event_loop_run();
  return ut_int32_get_value(return_code);
}
