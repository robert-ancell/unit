#include "ut.h"

int ut_tvg_command_id = 0;

bool ut_object_implements_tvg_command(UtObject *object) {
  return ut_object_get_interface(object, &ut_tvg_command_id) != NULL;
}
