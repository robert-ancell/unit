#include <assert.h>

#include "ut-ip-address.h"

int ut_ip_address_id = 0;

char *ut_ip_address_to_string(UtObject *object) {
  UtIPAddressInterface *ip_address_interface =
      ut_object_get_interface(object, &ut_ip_address_id);
  assert(ip_address_interface != NULL);
  return ip_address_interface->to_string(object);
}

bool ut_object_implements_ip_address(UtObject *object) {
  return ut_object_get_interface(object, &ut_ip_address_id) != NULL;
}
