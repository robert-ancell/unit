#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>

#include "ut.h"

typedef enum {
  DECODER_STATE_STATUS_LINE,
  DECODER_STATE_HEADER,
  DECODER_STATE_CONTENT,
  DECODER_STATE_ERROR,
  DECODER_STATE_DONE
} DecoderState;

// FIXME: Make a real object
typedef struct {
  UtObject *tcp_socket;
  UtObject *cancel;
  DecoderState state;
  UtObject *content_stream;
  char *host;
  uint16_t port;
  char *method;
  char *path;
  UtHttpResponseCallback callback;
  void *callback_user_data;
  UtObject *callback_cancel;
  unsigned int response_status_code;
  char *reason_phrase;
  UtObject *response_headers;
  UtObject *error;
} HttpRequest;

HttpRequest *http_request_new(const char *host, uint16_t port,
                              const char *method, const char *path,
                              UtHttpResponseCallback callback,
                              void *callback_user_data,
                              UtObject *callback_cancel) {
  HttpRequest *request = malloc(sizeof(HttpRequest));
  request->host = ut_cstring_new(host);
  request->port = port;
  request->method = ut_cstring_new(method);
  request->path = ut_cstring_new(path);
  request->cancel = ut_cancel_new();
  request->callback = callback;
  request->callback_user_data = callback_user_data;
  request->callback_cancel = ut_object_ref(callback_cancel);
  request->response_status_code = 0;
  request->reason_phrase = NULL;
  request->response_headers = ut_object_array_new();
  return request;
}

void http_request_free(HttpRequest *request) {
  ut_object_unref(request->tcp_socket);
  ut_object_unref(request->cancel);
  ut_object_unref(request->content_stream);
  free(request->host);
  free(request->method);
  free(request->path);
  ut_object_unref(request->callback_cancel);
  free(request->reason_phrase);
  ut_object_unref(request->response_headers);
  ut_object_unref(request->error);
  free(request);
}

typedef struct {
  UtObject object;
  UtObject *ip_address_resolver;
} UtHttpClient;

static void ut_http_client_init(UtObject *object) {
  UtHttpClient *self = (UtHttpClient *)object;
  self->ip_address_resolver = ut_ip_address_resolver_new();
}

static void ut_http_client_cleanup(UtObject *object) {
  UtHttpClient *self = (UtHttpClient *)object;
  ut_object_unref(self->ip_address_resolver);
}

static void set_error(HttpRequest *request, const char *description) {
  if (request->state == DECODER_STATE_ERROR) {
    return;
  }

  request->error = ut_png_error_new(description);
  request->state = DECODER_STATE_ERROR;
}

static bool parse_uri(const char *uri, char **scheme, char **user_info,
                      char **host, uint16_t *port, char **path, char **query,
                      char **fragment) {
  const char *scheme_start = uri, *scheme_end = uri;
  while (*scheme_end != '\0' && *scheme_end != ':') {
    scheme_end++;
  }
  if (*scheme_end != ':') {
    return false;
  }
  if (scheme != NULL) {
    *scheme = ut_cstring_new_sized(scheme_start, scheme_end - scheme_start);
  }

  const char *hier_part_start = scheme_end + 1, *hier_part_end;
  if (ut_cstring_starts_with(hier_part_start, "//")) {
    const char *authority_start = hier_part_start + 2,
               *authority_end = authority_start;
    while (*authority_end != '\0' && *authority_end != '/' &&
           *authority_end != '?' && *authority_end != '#') {
      authority_end++;
    }

    const char *userinfo_start = authority_start,
               *userinfo_end = userinfo_start;
    while (userinfo_end != authority_end && *userinfo_end != '@') {
      userinfo_end++;
    }
    const char *host_start = userinfo_end;
    if (userinfo_end != authority_end) {
      if (user_info != NULL) {
        *user_info =
            ut_cstring_new_sized(userinfo_start, userinfo_end - userinfo_start);
      }
      host_start = userinfo_end + 1;
    } else {
      if (user_info != NULL) {
        *user_info = NULL;
      }
      host_start = authority_start;
    }

    const char *host_end = host_start;
    while (host_end != authority_end && *host_end != ':') {
      host_end++;
    }
    if (host != NULL) {
      *host = ut_cstring_new_sized(userinfo_start, host_end - userinfo_start);
    }

    if (*host_end == ':') {
      const char *port_start = host_end + 1, *port_end = authority_end;
      if (port != NULL) {
        ut_cstring_ref port_string =
            ut_cstring_new_sized(port_start, port_end - port_start);
        *port = atoi(port_string);
      }
    } else {
      if (port != NULL) {
        *port = 0;
      }
    }
    hier_part_end = authority_end;
  } else {
    if (user_info != NULL) {
      *user_info = NULL;
    }
    if (host != NULL) {
      *host = NULL;
    }
    if (port != NULL) {
      *port = 0;
    }
    hier_part_end = hier_part_start;
  }

  const char *path_start = hier_part_end, *path_end = path_start;
  while (*path_end != '\0' && *path_end != '?' && *path_end != '#') {
    path_end++;
  }
  if (path != NULL) {
    *path = ut_cstring_new_sized(path_start, path_end - path_start);
  }

  if (*path_end == '?') {
    const char *query_start = path_end + 1, *query_end;
    while (*query_end != '\0' && *query_end != '#') {
      query_end++;
    }
    if (query != NULL) {
      *query = ut_cstring_new_sized(query_start, query_end - query_start);
    }
    path_start = query_end;
  } else {
    if (query != NULL) {
      *query = NULL;
    }
  }

  if (*path_end == '#') {
    const char *fragment_start = path_end + 1, *fragment_end;
    while (*fragment_end != '\0' && *fragment_end != '#') {
      fragment_end++;
    }
    if (fragment != NULL) {
      *fragment =
          ut_cstring_new_sized(fragment_start, fragment_end - fragment_start);
    }
    path_start = fragment_end;
  } else {
    if (fragment != NULL) {
      *fragment = NULL;
    }
  }

  return true;
}

static ssize_t find_line_end(UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  for (size_t i = 0; i < data_length - 1; i++) {
    if (ut_uint8_list_get_element(data, i) == '\r' &&
        ut_uint8_list_get_element(data, i + 1) == '\n') {
      return i;
    }
  }

  return -1;
}

static UtObject *get_line(UtObject *data, size_t *offset) {
  ssize_t line_end = find_line_end(data);
  if (line_end < 0) {
    return NULL;
  }

  *offset = line_end + 2;
  return ut_list_get_sublist(data, 0, line_end);
}

static ssize_t find_character(UtObject *data, size_t offset, char character) {
  size_t data_length = ut_list_get_length(data);
  for (size_t i = offset; i < data_length; i++) {
    if (ut_uint8_list_get_element(data, i) == character) {
      return i;
    }
  }

  return -1;
}

static char *get_string(UtObject *data, size_t start, size_t end) {
  while (start < end && ut_uint8_list_get_element(data, start) == ' ') {
    start++;
  }
  while (end > start && ut_uint8_list_get_element(data, end - 1) == ' ') {
    end--;
  }
  UtObjectRef string_data = ut_list_get_sublist(data, start, end - start);
  UtObjectRef string = ut_string_new_from_iso_8859_1(string_data);
  return ut_string_take_text(string);
}

static bool parse_status_line(HttpRequest *request, UtObject *data) {
  size_t protocol_version_start = 0;
  ssize_t protocol_version_end =
      find_character(data, protocol_version_start, ' ');
  if (protocol_version_end < 0) {
    set_error(request, "Invalid HTTP status line");
    return false;
  }

  size_t response_status_code_start = protocol_version_end + 1;
  ssize_t response_status_code_end =
      find_character(data, response_status_code_start, ' ');
  if (response_status_code_end < 0) {
    set_error(request, "Invalid HTTP status line");
    return false;
  }

  size_t reason_phrase_start = response_status_code_end + 1;
  size_t reason_phrase_end = ut_list_get_length(data);

  ut_cstring_ref response_status_code =
      get_string(data, response_status_code_start, response_status_code_end);
  request->response_status_code = atoi(response_status_code);
  request->reason_phrase =
      get_string(data, reason_phrase_start, reason_phrase_end);

  return true;
}

static bool parse_header(HttpRequest *request, UtObject *data) {
  size_t name_start = 0;
  ssize_t name_end = find_character(data, name_start, ':');
  if (name_end < 0) {
    return false;
  }

  size_t value_start = name_end + 1;
  size_t value_end = ut_list_get_length(data);

  ut_cstring_ref name = get_string(data, name_start, name_end);
  ut_cstring_ref value = get_string(data, value_start, value_end);
  UtObjectRef header = ut_http_header_new(name, value);

  ut_list_append(request->response_headers, header);

  return true;
}

static size_t decode_status_line(HttpRequest *request, UtObject *data) {
  size_t offset = 0;
  UtObjectRef line = get_line(data, &offset);
  if (line == NULL) {
    return 0;
  }

  if (!parse_status_line(request, line)) {
    return 0;
  }

  request->state = DECODER_STATE_HEADER;

  return offset;
}

static size_t decode_header(HttpRequest *request, UtObject *data) {
  size_t offset = 0;
  UtObjectRef line = get_line(data, &offset);
  if (line == NULL) {
    return 0;
  }

  // Ends on empty line.
  if (ut_list_get_length(line) == 0) {
    request->state = DECODER_STATE_CONTENT;
    UtObjectRef response = ut_http_response_new(
        request->response_status_code, request->reason_phrase,
        request->response_headers, request->content_stream);
    if (request->callback != NULL) {
      request->callback(request->callback_user_data, response);
    }

    return offset;
  }

  if (!parse_header(request, line)) {
    set_error(request, "Invalid HTTP header");
    return 0;
  }

  return offset;
}

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  HttpRequest *request = user_data;

  size_t data_length = ut_list_get_length(data);
  size_t offset = 0;
  while (true) {
    size_t n_used;
    UtObjectRef d = ut_list_get_sublist(data, offset, data_length - offset);
    DecoderState old_state = request->state;
    switch (request->state) {
    case DECODER_STATE_STATUS_LINE:
      n_used = decode_status_line(request, d);
      break;
    case DECODER_STATE_HEADER:
      n_used = decode_header(request, d);
      break;
    case DECODER_STATE_CONTENT:
      ut_buffered_input_stream_write(request->content_stream, d, complete);
      return offset;
    case DECODER_STATE_ERROR:
    case DECODER_STATE_DONE:
      return offset;
    default:
      assert(false);
    }

    if (request->state == old_state && n_used == 0) {
      if (complete && request->state != DECODER_STATE_CONTENT) {
        set_error(request, "HTTP connection closed before headers received");
      }
      return offset;
    }

    offset += n_used;
  }
}

static void connect_cb(void *user_data) {
  HttpRequest *request = user_data;

  UtObjectRef header = ut_string_new(request->method);
  ut_string_append(header, " ");
  ut_string_append(header, request->path);
  ut_string_append(header, " HTTP/1.1\r\n");
  ut_string_append(header, "Host: ");
  ut_string_append(header, request->host);
  ut_string_append(header, "\r\n");
  ut_string_append(header, "\r\n");
  UtObjectRef utf8 = ut_string_get_utf8(header);
  ut_output_stream_write(request->tcp_socket, utf8);
  request->state = DECODER_STATE_STATUS_LINE;
  ut_input_stream_read(request->tcp_socket, read_cb, request, request->cancel);
}

static void lookup_cb(void *user_data, UtObject *addresses) {
  HttpRequest *request = user_data;

  UtObjectRef address = ut_list_get_first(addresses);
  request->tcp_socket = ut_tcp_socket_new(address, request->port);
  request->content_stream = ut_buffered_input_stream_new();
  ut_tcp_socket_connect(request->tcp_socket, connect_cb, request,
                        request->callback_cancel);
}

static UtObjectInterface object_interface = {.type_name = "HttpClient",
                                             .init = ut_http_client_init,
                                             .cleanup = ut_http_client_cleanup};

UtObject *ut_http_client_new() {
  return ut_object_new(sizeof(UtHttpClient), &object_interface);
}

void ut_http_client_send_request(UtObject *object, const char *method,
                                 const char *uri,
                                 UtHttpResponseCallback callback,
                                 void *user_data, UtObject *cancel) {
  assert(ut_object_is_http_client(object));
  UtHttpClient *self = (UtHttpClient *)object;

  ut_cstring_ref scheme = NULL;
  ut_cstring_ref host = NULL;
  ut_cstring_ref path = NULL;
  uint16_t port;
  assert(parse_uri(uri, &scheme, NULL, &host, &port, &path, NULL, NULL));
  assert(ut_cstring_equal(scheme, "http"));
  assert(host != NULL);
  if (ut_cstring_equal(path, "")) {
    ut_cstring_set(&path, "/");
  }
  if (port == 0) {
    port = 80;
  }

  HttpRequest *request =
      http_request_new(host, port, method, path, callback, user_data, cancel);
  ut_ip_address_resolver_lookup(self->ip_address_resolver, host, lookup_cb,
                                request, cancel);
}

bool ut_object_is_http_client(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
