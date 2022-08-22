#include <stdio.h>

#include "ut.h"

static void lookup_cb(void *user_data, UtObject *address) {
  printf("%s\n", ut_ip_address_to_string(address));

  // ut_event_loop_return(NULL);
}

int main(int argc, char **argv) {
  UtObjectRef address = ut_ipv4_address_new_from_quad(8, 8, 8, 8);
  UtObjectRef client = ut_dns_client_new(address);

  ut_dns_client_lookup_ipv4(client, "example.com", lookup_cb, NULL, NULL);
  ut_dns_client_lookup_ipv6(client, "example.com", lookup_cb, NULL, NULL);

  ut_event_loop_run();

  return 0;
}
