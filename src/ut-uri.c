#include <assert.h>

#include "ut.h"

// https://www.ietf.org/rfc/rfc3986

// FIXME: Validate characters
// FIXME: Encoded characters
// FIXME: Validate IP literal

typedef struct {
  UtObject object;

  char *scheme;
  char *user_info;
  char *host;
  uint16_t port;
  char *path;
  char *query;
  char *fragment;
} UtUri;

static char *ut_uri_to_string(UtObject *object) {
  UtUri *self = (UtUri *)object;

  UtObjectRef string = ut_string_new(self->scheme);
  ut_string_append(string, ":");
  if (self->host != NULL) {
    ut_string_append(string, "//");
    if (self->user_info != NULL) {
      ut_string_append(string, self->user_info);
      ut_string_append(string, "@");
    }
    ut_string_append(string, self->host);
  }
  if (self->port != 0) {
    ut_string_append(string, ":");
    ut_string_append_printf(string, "%d", self->port);
  }
  if (self->path != NULL) {
    ut_string_append(string, self->path);
  }
  if (self->query != NULL) {
    ut_string_append(string, "?");
    ut_string_append(string, self->query);
  }
  if (self->fragment != NULL) {
    ut_string_append(string, "#");
    ut_string_append(string, self->fragment);
  }

  return ut_string_take_text(string);
}

static void ut_uri_cleanup(UtObject *object) {
  UtUri *self = (UtUri *)object;
  free(self->scheme);
  free(self->user_info);
  free(self->host);
  free(self->path);
  free(self->query);
  free(self->fragment);
}

static UtObjectInterface object_interface = {.type_name = "UtUri",
                                             .to_string = ut_uri_to_string,
                                             .cleanup = ut_uri_cleanup};

UtObject *ut_uri_new(const char *scheme, const char *user_info,
                     const char *host, uint16_t port, const char *path,
                     const char *query, const char *fragment) {
  UtObject *object = ut_object_new(sizeof(UtUri), &object_interface);
  UtUri *self = (UtUri *)object;

  assert(scheme != NULL);
  assert(path != NULL);

  self->scheme = ut_cstring_new(scheme);
  self->user_info = user_info != NULL ? ut_cstring_new(user_info) : NULL;
  self->host = host != NULL ? ut_cstring_new(host) : NULL;
  self->port = port;
  self->path = ut_cstring_new(path);
  self->query = query != NULL ? ut_cstring_new(query) : NULL;
  self->fragment = fragment != NULL ? ut_cstring_new(fragment) : NULL;

  return object;
}

UtObject *ut_uri_new_from_string(const char *uri) {
  UtObject *object = ut_object_new(sizeof(UtUri), &object_interface);
  UtUri *self = (UtUri *)object;

  const char *scheme_start = uri, *scheme_end = uri;
  while (*scheme_end != '\0' && *scheme_end != ':') {
    scheme_end++;
  }
  if (*scheme_end != ':') {
    return ut_error_new("Missing scheme terminator");
  }
  self->scheme = ut_cstring_new_sized(scheme_start, scheme_end - scheme_start);

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
      self->user_info =
          ut_cstring_new_sized(userinfo_start, userinfo_end - userinfo_start);
      host_start = userinfo_end + 1;
    } else {
      host_start = authority_start;
    }

    const char *host_end = host_start;
    if (*host_start == '[') {
      while (*host_end != '\0' && *host_end != ']') {
        host_end++;
      }
      if (*host_end != ']') {
        return ut_error_new("Unterminated literal IP address");
      }
      host_end++;
    } else {
      while (host_end != authority_end && *host_end != ':') {
        host_end++;
      }
    }
    self->host =
        ut_cstring_new_sized(userinfo_start, host_end - userinfo_start);

    if (*host_end == ':') {
      const char *port_start = host_end + 1, *port_end = authority_end;
      ut_cstring_ref port_string =
          ut_cstring_new_sized(port_start, port_end - port_start);
      self->port = atoi(port_string);
    }
    hier_part_end = authority_end;
  } else {
    hier_part_end = hier_part_start;
  }

  const char *path_start = hier_part_end, *path_end = path_start;
  while (*path_end != '\0' && *path_end != '?' && *path_end != '#') {
    path_end++;
  }
  self->path = ut_cstring_new_sized(path_start, path_end - path_start);

  if (*path_end == '?') {
    const char *query_start = path_end + 1, *query_end = query_start;
    while (*query_end != '\0' && *query_end != '#') {
      query_end++;
    }
    self->query = ut_cstring_new_sized(query_start, query_end - query_start);
    path_end = query_end;
  }

  if (*path_end == '#') {
    const char *fragment_start = path_end + 1, *fragment_end = fragment_start;
    while (*fragment_end != '\0' && *fragment_end != '#') {
      fragment_end++;
    }
    self->fragment =
        ut_cstring_new_sized(fragment_start, fragment_end - fragment_start);
  }

  return object;
}

const char *ut_uri_get_scheme(UtObject *object) {
  assert(ut_object_is_uri(object));
  UtUri *self = (UtUri *)object;
  return self->scheme;
}

const char *ut_uri_get_user_info(UtObject *object) {
  assert(ut_object_is_uri(object));
  UtUri *self = (UtUri *)object;
  return self->user_info;
}

const char *ut_uri_get_host(UtObject *object) {
  assert(ut_object_is_uri(object));
  UtUri *self = (UtUri *)object;
  return self->host;
}

uint16_t ut_uri_get_port(UtObject *object) {
  assert(ut_object_is_uri(object));
  UtUri *self = (UtUri *)object;
  return self->port;
}

const char *ut_uri_get_path(UtObject *object) {
  assert(ut_object_is_uri(object));
  UtUri *self = (UtUri *)object;
  return self->path;
}

const char *ut_uri_get_query(UtObject *object) {
  assert(ut_object_is_uri(object));
  UtUri *self = (UtUri *)object;
  return self->query;
}

const char *ut_uri_get_fragment(UtObject *object) {
  assert(ut_object_is_uri(object));
  UtUri *self = (UtUri *)object;
  return self->fragment;
}

bool ut_object_is_uri(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
