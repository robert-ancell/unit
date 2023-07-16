#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ut-fd-output-stream.h"
#include "ut.h"

typedef struct _WriteBlock WriteBlock;

struct _WriteBlock {
  UtObject *data;
  size_t n_written;
  UtObject *callback_object;
  UtOutputStreamCallback callback;
  WriteBlock *next;
};

typedef struct {
  UtObject object;
  UtObject *fd;
  UtObject *watch;
  WriteBlock *blocks;
  WriteBlock *last_block;
} UtFdOutputStream;

static void add_block(UtFdOutputStream *self, UtObject *data,
                      UtObject *callback_object,
                      UtOutputStreamCallback callback) {
  WriteBlock *block;

  block = malloc(sizeof(WriteBlock));
  block->data = ut_object_ref(data);
  block->n_written = 0;
  ut_object_weak_ref(callback_object, &block->callback_object);
  block->callback = callback;
  block->next = NULL;

  if (self->last_block != NULL) {
    self->last_block->next = block;
    self->last_block = block;
  } else {
    self->blocks = self->last_block = block;
  }
}

static void free_block(WriteBlock *block) {
  ut_object_unref(block->data);
  ut_object_weak_unref(&block->callback_object);
  free(block);
}

static void write_cb(UtObject *object) {
  UtFdOutputStream *self = (UtFdOutputStream *)object;

  WriteBlock *block = self->blocks;
  assert(block != NULL);

  // Write remaining data.
  size_t block_length = ut_list_get_length(block->data);
  size_t n_to_write = block_length - block->n_written;
  const uint8_t *buffer;
  uint8_t *allocated_buffer = NULL;
  buffer = ut_uint8_list_get_data(block->data);
  if (buffer == NULL) {
    allocated_buffer = ut_uint8_list_copy_data(block->data);
    buffer = allocated_buffer;
  }
  ssize_t n_written = write(ut_file_descriptor_get_fd(self->fd),
                            buffer + block->n_written, n_to_write);
  free(allocated_buffer);
  assert(n_written >= 0);
  block->n_written += n_written;

  if (block->n_written == block_length) {
    self->blocks = block->next;
    if (self->blocks == NULL) {
      self->last_block = NULL;
    }

    if (block->callback_object != NULL && block->callback != NULL) {
      block->callback(block->callback_object, NULL);
    }

    free_block(block);
  }

  // Stop listening for write events when done.
  if (self->blocks == NULL) {
    ut_event_loop_cancel_watch(self->watch);
    ut_object_clear(&self->watch);
  }
}

static void ut_fd_output_stream_cleanup(UtObject *object) {
  UtFdOutputStream *self = (UtFdOutputStream *)object;
  ut_object_unref(self->fd);
  ut_object_unref(self->watch);
  WriteBlock *next_block;
  for (WriteBlock *b = self->blocks; b != NULL; b = next_block) {
    next_block = b->next;
    free_block(b);
  }
  self->blocks = NULL;
}

static void ut_fd_output_stream_write(UtObject *object, UtObject *data,
                                      UtObject *callback_object,
                                      UtOutputStreamCallback callback) {
  UtFdOutputStream *self = (UtFdOutputStream *)object;

  add_block(self, data, callback_object, callback);

  if (self->watch == NULL) {
    self->watch = ut_event_loop_add_write_watch(self->fd, object, write_cb);
  }
}

static UtOutputStreamInterface output_stream_interface = {
    .write = ut_fd_output_stream_write};

static UtObjectInterface object_interface = {
    .type_name = "UtFdOutputStream",
    .cleanup = ut_fd_output_stream_cleanup,
    .interfaces = {{&ut_output_stream_id, &output_stream_interface},
                   {NULL, NULL}}};

UtObject *ut_fd_output_stream_new(UtObject *fd) {
  UtObject *object = ut_object_new(sizeof(UtFdOutputStream), &object_interface);
  UtFdOutputStream *self = (UtFdOutputStream *)object;
  self->fd = ut_object_ref(fd);
  return object;
}

bool ut_object_is_fd_output_stream(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
