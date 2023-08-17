#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef uri = ut_uri_new("http", NULL, "www.ietf.org", 0,
                               "/rfc/rfc2396.txt", NULL, NULL);
  ut_cstring_ref uri_string = ut_object_to_string(uri);
  ut_assert_cstring_equal(uri_string, "http://www.ietf.org/rfc/rfc2396.txt");

  // Examples from RFC 3986 Section 1.1.2

  UtObjectRef ftp_uri =
      ut_uri_new_from_string("ftp://ftp.is.co.za/rfc/rfc1808.txt");
  ut_assert_is_not_error(ftp_uri);
  ut_assert_cstring_equal(ut_uri_get_scheme(ftp_uri), "ftp");
  ut_assert_null_cstring(ut_uri_get_user_info(ftp_uri));
  ut_assert_cstring_equal(ut_uri_get_host(ftp_uri), "ftp.is.co.za");
  ut_assert_int_equal(ut_uri_get_port(ftp_uri), 0);
  ut_assert_cstring_equal(ut_uri_get_path(ftp_uri), "/rfc/rfc1808.txt");
  ut_assert_null_cstring(ut_uri_get_query(ftp_uri));
  ut_assert_null_cstring(ut_uri_get_fragment(ftp_uri));

  UtObjectRef http_uri =
      ut_uri_new_from_string("http://www.ietf.org/rfc/rfc2396.txt");
  ut_assert_is_not_error(http_uri);
  ut_assert_cstring_equal(ut_uri_get_scheme(http_uri), "http");
  ut_assert_null_cstring(ut_uri_get_user_info(http_uri));
  ut_assert_cstring_equal(ut_uri_get_host(http_uri), "www.ietf.org");
  ut_assert_int_equal(ut_uri_get_port(http_uri), 0);
  ut_assert_cstring_equal(ut_uri_get_path(http_uri), "/rfc/rfc2396.txt");
  ut_assert_null_cstring(ut_uri_get_query(http_uri));
  ut_assert_null_cstring(ut_uri_get_fragment(http_uri));

  UtObjectRef ldap_uri =
      ut_uri_new_from_string("ldap://[2001:db8::7]/c=GB?objectClass?one");
  ut_assert_is_not_error(ldap_uri);
  ut_assert_cstring_equal(ut_uri_get_scheme(ldap_uri), "ldap");
  ut_assert_null_cstring(ut_uri_get_user_info(ldap_uri));
  ut_assert_cstring_equal(ut_uri_get_host(ldap_uri), "[2001:db8::7]");
  ut_assert_int_equal(ut_uri_get_port(ldap_uri), 0);
  ut_assert_cstring_equal(ut_uri_get_path(ldap_uri), "/c=GB");
  ut_assert_cstring_equal(ut_uri_get_query(ldap_uri), "objectClass?one");
  ut_assert_null_cstring(ut_uri_get_fragment(ldap_uri));

  UtObjectRef mailto_uri =
      ut_uri_new_from_string("mailto:John.Doe@example.com");
  ut_assert_is_not_error(mailto_uri);
  ut_assert_cstring_equal(ut_uri_get_scheme(mailto_uri), "mailto");
  ut_assert_null_cstring(ut_uri_get_user_info(mailto_uri));
  ut_assert_null_cstring(ut_uri_get_host(mailto_uri));
  ut_assert_int_equal(ut_uri_get_port(mailto_uri), 0);
  ut_assert_cstring_equal(ut_uri_get_path(mailto_uri), "John.Doe@example.com");
  ut_assert_null_cstring(ut_uri_get_query(mailto_uri));
  ut_assert_null_cstring(ut_uri_get_fragment(mailto_uri));

  UtObjectRef news_uri =
      ut_uri_new_from_string("news:comp.infosystems.www.servers.unix");
  ut_assert_is_not_error(news_uri);
  ut_assert_cstring_equal(ut_uri_get_scheme(news_uri), "news");
  ut_assert_null_cstring(ut_uri_get_user_info(news_uri));
  ut_assert_null_cstring(ut_uri_get_host(news_uri));
  ut_assert_int_equal(ut_uri_get_port(news_uri), 0);
  ut_assert_cstring_equal(ut_uri_get_path(news_uri),
                          "comp.infosystems.www.servers.unix");
  ut_assert_null_cstring(ut_uri_get_query(news_uri));
  ut_assert_null_cstring(ut_uri_get_fragment(news_uri));

  UtObjectRef tel_uri = ut_uri_new_from_string("tel:+1-816-555-1212");
  ut_assert_is_not_error(tel_uri);
  ut_assert_cstring_equal(ut_uri_get_scheme(tel_uri), "tel");
  ut_assert_null_cstring(ut_uri_get_user_info(tel_uri));
  ut_assert_null_cstring(ut_uri_get_host(tel_uri));
  ut_assert_int_equal(ut_uri_get_port(tel_uri), 0);
  ut_assert_cstring_equal(ut_uri_get_path(tel_uri), "+1-816-555-1212");
  ut_assert_null_cstring(ut_uri_get_query(tel_uri));
  ut_assert_null_cstring(ut_uri_get_fragment(tel_uri));

  UtObjectRef telnet_uri = ut_uri_new_from_string("telnet://192.0.2.16:80/");
  ut_assert_is_not_error(telnet_uri);
  ut_assert_cstring_equal(ut_uri_get_scheme(telnet_uri), "telnet");
  ut_assert_null_cstring(ut_uri_get_user_info(telnet_uri));
  ut_assert_cstring_equal(ut_uri_get_host(telnet_uri), "192.0.2.16");
  ut_assert_int_equal(ut_uri_get_port(telnet_uri), 80);
  ut_assert_cstring_equal(ut_uri_get_path(telnet_uri), "/");
  ut_assert_null_cstring(ut_uri_get_query(telnet_uri));
  ut_assert_null_cstring(ut_uri_get_fragment(telnet_uri));

  UtObjectRef urn_uri = ut_uri_new_from_string(
      "urn:oasis:names:specification:docbook:dtd:xml:4.1.2");
  ut_assert_is_not_error(urn_uri);
  ut_assert_cstring_equal(ut_uri_get_scheme(urn_uri), "urn");
  ut_assert_null_cstring(ut_uri_get_user_info(urn_uri));
  ut_assert_null_cstring(ut_uri_get_host(urn_uri));
  ut_assert_int_equal(ut_uri_get_port(urn_uri), 0);
  ut_assert_cstring_equal(ut_uri_get_path(urn_uri),
                          "oasis:names:specification:docbook:dtd:xml:4.1.2");
  ut_assert_null_cstring(ut_uri_get_query(urn_uri));
  ut_assert_null_cstring(ut_uri_get_fragment(urn_uri));

  // Example from RFC 3986 section 3.
  UtObjectRef rfc_uri = ut_uri_new_from_string(
      "foo://example.com:8042/over/there?name=ferret#nose");
  ut_assert_is_not_error(rfc_uri);
  ut_assert_cstring_equal(ut_uri_get_scheme(rfc_uri), "foo");
  ut_assert_null_cstring(ut_uri_get_user_info(rfc_uri));
  ut_assert_cstring_equal(ut_uri_get_host(rfc_uri), "example.com");
  ut_assert_int_equal(ut_uri_get_port(rfc_uri), 8042);
  ut_assert_cstring_equal(ut_uri_get_path(rfc_uri), "/over/there");
  ut_assert_cstring_equal(ut_uri_get_query(rfc_uri), "name=ferret");
  ut_assert_cstring_equal(ut_uri_get_fragment(rfc_uri), "nose");

  return 0;
}
