#include <stdio.h>

#include "ut.h"

static void resolve_cb(void *user_data, UtObject *addresses) {
  ut_cstring_ref text = ut_object_to_string(addresses);
  printf("%s\n", text);
  ut_event_loop_return(NULL);
}

int main(int argc, char **argv) {
  UtObjectRef resolver = ut_ip_address_resolver_new();
  ut_ip_address_resolver_lookup(resolver, "example.com", resolve_cb, NULL,
                                NULL);

  ut_event_loop_run();

  return 0;
}
