#include <stdio.h>

#include "ut-gif-test-data.h"
#include "ut.h"

static UtObject *create_gif(uint32_t width, uint32_t height,
                            const char *hex_color_table,
                            const char *hex_image_data) {
  UtObjectRef image_data = ut_uint8_list_new_from_hex_string(hex_image_data);
  UtObjectRef color_table = ut_uint8_list_new_from_hex_string(hex_color_table);
  return ut_gif_image_new(0, 0, width, height, color_table, image_data);
}

// Check the GIF image can be encoded and matches the expected data.
static void check_gif_encode(UtObject *image, const char *hex_data) {
  UtObjectRef data = ut_uint8_array_new();
  UtObjectRef encoder = ut_gif_encoder_new_single(image, data);
  ut_gif_encoder_encode(encoder);
  ut_assert_uint8_list_equal_hex(data, hex_data);
}

int main(int argc, char **argv) {
  UtObjectRef depth1_image =
      create_gif(2, 1, depth1_color_table_data, depth1_image_data);
  check_gif_encode(depth1_image, depth1_data);

  UtObjectRef depth2_image =
      create_gif(2, 2, depth2_color_table_data, depth2_image_data);
  check_gif_encode(depth2_image, depth2_data);

  UtObjectRef depth3_image =
      create_gif(4, 2, depth3_color_table_data, depth3_image_data);
  check_gif_encode(depth3_image, depth3_data);

  UtObjectRef depth4_image =
      create_gif(4, 4, depth4_color_table_data, depth4_image_data);
  check_gif_encode(depth4_image, depth4_data);

  UtObjectRef depth5_image =
      create_gif(8, 4, depth5_color_table_data, depth5_image_data);
  check_gif_encode(depth5_image, depth5_data);

  UtObjectRef depth6_image =
      create_gif(8, 8, depth6_color_table_data, depth6_image_data);
  check_gif_encode(depth6_image, depth6_data);

  UtObjectRef depth7_image =
      create_gif(16, 8, depth7_color_table_data, depth7_image_data);
  check_gif_encode(depth7_image, depth7_data);

  UtObjectRef depth8_image =
      create_gif(16, 16, depth8_color_table_data, depth8_image_data);
  check_gif_encode(depth8_image, depth8_data);

  UtObjectRef comment_image = create_gif(1, 1, "000000ffffff", "01");
  UtObjectRef comment_data = ut_uint8_array_new();
  UtObjectRef comment_encoder =
      ut_gif_encoder_new_single(comment_image, comment_data);
  ut_gif_encoder_add_comment(comment_encoder, "Lorem Ipsum!");
  ut_gif_encoder_encode(comment_encoder);
  ut_assert_uint8_list_equal_hex(
      comment_data, "47494638396101000100f00000000000ffffff21fe0c4c6f72656d2049"
                    "7073756d21002c000000000100010000010136003b");

  UtObjectRef long_comment_image = create_gif(1, 1, "000000ffffff", "01");
  UtObjectRef long_comment_data = ut_uint8_array_new();
  UtObjectRef long_comment_encoder =
      ut_gif_encoder_new_single(long_comment_image, long_comment_data);
  ut_gif_encoder_add_comment(
      long_comment_encoder,
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod "
      "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim "
      "veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea "
      "commodo consequat. Duis aute irure dolor in reprehenderit in voluptate "
      "velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint "
      "occaecat cupidatat non proident, sunt in culpa qui officia deserunt "
      "mollit anim id est laborum.");
  ut_gif_encoder_encode(long_comment_encoder);
  ut_assert_uint8_list_equal_hex(
      long_comment_data,
      "47494638396101000100f00000000000ffffff21feff4c6f72656d20697073756d20646f"
      "6c6f722073697420616d65742c20636f6e73656374657475722061646970697363696e67"
      "20656c69742c2073656420646f20656975736d6f642074656d706f7220696e6369646964"
      "756e74207574206c61626f726520657420646f6c6f7265206d61676e6120616c69717561"
      "2e20557420656e696d206164206d696e696d2076656e69616d2c2071756973206e6f7374"
      "72756420657865726369746174696f6e20756c6c616d636f206c61626f726973206e6973"
      "6920757420616c697175697020657820656120636f6d6d6f646f20636f6e736571756174"
      "2e2044756973206175746520697275726520646f6c6f722069be6e20726570726568656e"
      "646572697420696e20766f6c7570746174652076656c697420657373652063696c6c756d"
      "20646f6c6f726520657520667567696174206e756c6c612070617269617475722e204578"
      "636570746575722073696e74206f6363616563617420637570696461746174206e6f6e20"
      "70726f6964656e742c2073756e7420696e2063756c706120717569206f66666963696120"
      "6465736572756e74206d6f6c6c697420616e696d20696420657374206c61626f72756d2e"
      "002c000000000100010000010136003b");

  return 0;
}
