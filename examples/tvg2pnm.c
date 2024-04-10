#include <stdio.h>

#include "ut.h"

static void decode_cb(UtObject *object) {
  UtObject *decoder = object;

  if (ut_tvg_decoder_get_error(decoder) != NULL) {
    printf("Failed to decode: %s\n",
           ut_error_get_description(ut_tvg_decoder_get_error(decoder)));
    UtObjectRef return_code = ut_int32_new(1);
    ut_event_loop_return(return_code);
    return;
  }

  UtObject *image = ut_tvg_decoder_get_image(decoder);
  uint16_t width = ut_tvg_image_get_width(image);
  uint16_t height = ut_tvg_image_get_height(image);

  UtObjectRef renderer = ut_tvg_renderer_new(image);
  UtObjectRef rgba = ut_tvg_renderer_render(renderer);
  const uint8_t *pixels = ut_uint8_list_get_data(rgba);

  printf("P3\n");
  printf("%d %d\n", width, height);
  printf("255\n");
  size_t offset = 0;
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      if (x != 0) {
        printf(" ");
      }
      const uint8_t *pixel = pixels + offset;
      printf("%d %d %d", pixel[0], pixel[1], pixel[2]);
      offset += 3;
    }
    printf("\n");
  }

  UtObjectRef return_code = ut_int32_new(0);
  ut_event_loop_return(return_code);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: tvg2pnm [file.tvg]\n");
    return 1;
  }

  UtObjectRef file = ut_local_file_new(argv[1]);
  ut_file_open_read(file);
  UtObjectRef decoder = ut_tvg_decoder_new(file);
  ut_tvg_decoder_decode(decoder, decoder, decode_cb);

  UtObjectRef return_code = ut_event_loop_run();
  return ut_int32_get_value(return_code);
}
