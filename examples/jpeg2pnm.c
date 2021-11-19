#include <stdio.h>

#include "ut.h"

static void decode_cb(void *user_data) {
  UtObject *decoder = user_data;

  UtObject *image = ut_jpeg_decoder_get_image(decoder);
  if (image == NULL) {
    printf("Failed to decode: %s\n",
           ut_error_get_description(ut_jpeg_decoder_get_error(decoder)));
    UtObjectRef return_code = ut_int32_new(1);
    ut_event_loop_return(return_code);
    return;
  }

  size_t width = ut_jpeg_image_get_width(image);
  size_t height = ut_jpeg_image_get_height(image);
  size_t n_components = ut_jpeg_image_get_n_components(image);
  const uint8_t *data = ut_uint8_list_get_data(ut_jpeg_image_get_data(image));

  if (n_components == 1) {
    printf("P2\n");
  } else {
    printf("P3\n");
  }
  printf("%zi %zi\n", width, height);
  printf("255\n");
  size_t i = 0;
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      if (x != 0) {
        printf(" ");
      }
      for (size_t c = 0; c < n_components; c++) {
        if (c != 0) {
          printf(" ");
        }
        printf("%d", data[i++]);
      }
    }
    printf("\n");
  }

  UtObjectRef return_code = ut_int32_new(0);
  ut_event_loop_return(return_code);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: jpeg2pnm [file.jpg]\n");
    return 1;
  }

  UtObjectRef file = ut_local_file_new(argv[1]);
  ut_file_open_read(file);
  UtObjectRef decoder = ut_jpeg_decoder_new(file);
  ut_jpeg_decoder_decode(decoder, decode_cb, decoder, NULL);

  UtObjectRef return_code = ut_event_loop_run();
  return ut_int32_get_value(return_code);
}
