#include <stdio.h>

#include "ut.h"

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtObjectRef text = ut_string_new_from_utf8(data);
  printf("read - '%s'\n", ut_string_get_text(text));
  ut_event_loop_return(NULL);
  return ut_list_get_length(data);
}

int main(int argc, char **argv) {
  UtObjectRef readme = ut_local_file_new("README.md");
  ut_file_open_read(readme);
  UtObjectRef dummy_object = ut_null_new();
  ut_input_stream_read_all(readme, dummy_object, read_cb);

  UtObjectRef test_file = ut_local_file_new("TEST");
  ut_file_open_write(test_file, true);
  UtObjectRef test_data = ut_string_new_constant("Hello\n");
  UtObjectRef utf8 = ut_string_get_utf8(test_data);
  ut_output_stream_write(test_file, utf8);

  ut_event_loop_run();

  return 0;
}
