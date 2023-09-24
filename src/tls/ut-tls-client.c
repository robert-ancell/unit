#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>

#include "ut.h"

// https://datatracker.ietf.org/doc/html/rfc8446

#define SSL_VERSION_3_0 0x0300
#define TLS_VERSION_1_0 0x0301
#define TLS_VERSION_1_2 0x0303
#define TLS_VERSION_1_3 0x0304

typedef enum {
  STATE_START,
  STATE_WAIT_SERVER_HELLO,
  STATE_WAIT_ENCRYPTED_EXTENSIONS,
  STATE_WAIT_CERTIFICATE_REQUEST,
  STATE_WAIT_CERTIFICATE,
  STATE_WAIT_CERTIFICATE_VERIFY,
  STATE_WAIT_FINISHED,
  STATE_CONNECTED
} State;

typedef enum {
  CONTENT_TYPE_CHANGE_CIPHER_SPEC = 20,
  CONTENT_TYPE_ALERT = 21,
  CONTENT_TYPE_HANDSHAKE = 22,
  CONTENT_TYPE_APPLICATION_DATA = 23
} ContentType;

typedef enum {
  HANDSHAKE_TYPE_CLIENT_HELLO = 1,
  HANDSHAKE_TYPE_SERVER_HELLO = 2,
  HANDSHAKE_TYPE_NEW_SESSION_TICKET = 4,
  HANDSHAKE_TYPE_END_OF_EARLY_DATA = 5,
  HANDSHAKE_TYPE_ENCRYPTED_EXTENSIONS = 8,
  HANDSHAKE_TYPE_CERTIFICATE = 11,
  HANDSHAKE_TYPE_CERTIFICATE_REQUEST = 13,
  HANDSHAKE_TYPE_CERTIFICATE_VERIFY = 15,
  HANDSHAKE_TYPE_FINISHED = 20,
  HANDSHAKE_TYPE_KEY_UPDATE = 24,
  HANDSHAKE_TYPE_MESSAGE_HASH = 254
} HandshakeType;

typedef struct {
  UtObject object;
  UtObject *input_stream;
  UtObject *output_stream;

  State state;
} UtTlsClient;

static void ut_tls_client_init(UtObject *object) {
  UtTlsClient *self = (UtTlsClient *)object;
}

static void ut_tls_client_cleanup(UtObject *object) {
  UtTlsClient *self = (UtTlsClient *)object;
  ut_object_unref(self->input_stream);
  ut_object_unref(self->output_stream);
}

static UtObject *encode_client_hello(UtTlsClient *self, uint16_t version,
                                     UtObject *random, UtObject *session_id,
                                     UtObject *cipher_suites,
                                     UtObject *compression_data,
                                     UtObject *extensions) {
  UtObjectRef message = ut_uint8_list_new();
  ut_uint8_list_append_uint16_be(message, version);
  assert(ut_list_get_length(random) == 32);
  ut_list_append_list(message, random);
  assert(ut_list_get_length(session_id) <= 0xff);
  ut_uint8_list_append(message, ut_list_get_length(session_id));
  ut_list_append_list(message, session_id);
  assert(ut_list_get_length(cipher_suites) <= 0xffff);
  ut_uint8_list_append_uint16_be(message, ut_list_get_length(cipher_suites));
  ut_list_append_list(message, cipher_suites);
  assert(ut_list_get_length(compression_data) <= 0xff);
  ut_uint8_list_append(message, ut_list_get_length(compression_data));
  ut_list_append_list(message, compression_data);
  assert(ut_list_get_length(extensions) <= 0xffff);
  ut_uint8_list_append_uint16_be(message, ut_list_get_length(extensions));
  ut_list_append_list(message, extensions);

  return ut_object_ref(message);
}

static UtObject *encode_handshake(HandshakeType type, UtObject *message) {
  UtObjectRef handshake = ut_uint8_list_new();
  ut_uint8_list_append(handshake, type);
  assert(ut_list_get_length(message) <= 0xffffff);
  append_uint24_be(handshake, ut_list_get_length(message));
  ut_list_append_list(handshake, message);

  return ut_object_ref(handshake);
}

static UtObject *encode_plaintext(ContentType type, uint16_t version,
                                  UtObject *fragment) {
  UtObjectRef plaintext = ut_uint8_list_new();
  ut_uint8_list_append(plaintext, type);
  ut_uint8_list_append_uint16_be(plaintext, version);
  assert(ut_list_get_length(fragment) <= 0xffff);
  ut_uint8_list_append_uint16_be(plaintext, ut_list_get_length(fragment));
  ut_list_append_list(plaintext, fragment);

  return ut_object_ref(plaintext);
}

static void send_client_hello() {}

static void send_end_of_early_data() {}

static void send_certificate() {}

static void send_certificate_verify() {}

static void send_finished() {}

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  size_t data_length = ut_list_get_length(data);

  return data_length;
}

static UtObjectInterface object_interface = {.type_name = "UtTlsClient",
                                             .init = ut_tls_client_init,
                                             .cleanup = ut_tls_client_cleanup};

UtObject *ut_tls_client_new(UtObject *input_stream, UtObject *output_stream) {
  UtObject *object = ut_object_new(sizeof(UtTlsClient), &object_interface);
  UtTlsClient *self = (UtTlsClient *)object;
  self->input_stream = ut_object_ref(input_stream);
  self->output_stream = ut_object_ref(output_stream);
  return object;
}

UtObject *ut_tls_client_new_tcp(UtObject *address, uint16_t port) {
  // FIXME
  return ut_object_new(sizeof(UtTlsClient), &object_interface);
}

void ut_tls_client_connect(UtObject *object) {
  UtTlsClient *self = (UtTlsClient *)object;
}

bool ut_object_is_tls_client(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
