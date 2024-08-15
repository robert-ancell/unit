#include <assert.h>

#include "ut-jpeg-arithmetic-decoder.h"
#include "ut.h"

// Arithmetic Qe values as defined in ISO/IEC 10918-1 Table D.3
static uint16_t qe_values[113] = {
    0x5A1D, 0x2586, 0x1114, 0x080B, 0x03D8, 0x01DA, 0x00E5, 0x006F, 0x0036,
    0x001A, 0x000D, 0x0006, 0x0003, 0x0001, 0x5A7F, 0x3F25, 0x2CF2, 0x207C,
    0x17B9, 0x1182, 0x0CEF, 0x09A1, 0x072F, 0x055C, 0x0406, 0x0303, 0x0240,
    0x01B1, 0x0144, 0x00F5, 0x00B7, 0x008A, 0x0068, 0x004E, 0x003B, 0x002C,
    0x5AE1, 0x484C, 0x3A0D, 0x2EF1, 0x261F, 0x1F33, 0x19A8, 0x1518, 0x1177,
    0x0E74, 0x0BFB, 0x09F8, 0x0861, 0x0706, 0x05CD, 0x04DE, 0x040F, 0x0363,
    0x02D4, 0x025C, 0x01F8, 0x01A4, 0x0160, 0x0125, 0x00F6, 0x00CB, 0x00AB,
    0x008F, 0x5B12, 0x4D04, 0x412C, 0x37D8, 0x2FE8, 0x293C, 0x2379, 0x1EDF,
    0x1AA9, 0x174E, 0x1424, 0x119C, 0x0F6B, 0x0D51, 0x0BB6, 0x0A40, 0x5832,
    0x4D1C, 0x438E, 0x3BDD, 0x34EE, 0x2EAE, 0x299A, 0x2516, 0x5570, 0x4CA9,
    0x44D9, 0x3E22, 0x3824, 0x32B4, 0x2E17, 0x56A8, 0x4F46, 0x47E5, 0x41CF,
    0x3C3D, 0x375E, 0x5231, 0x4C0F, 0x4639, 0x415E, 0x5627, 0x50E7, 0x4B85,
    0x5597, 0x504F, 0x5A10, 0x5522, 0x59EB};

static uint8_t lps_next_index[113] = {
    1,   14,  16,  18,  20,  23,  25,  28,  30,  33,  35, 9,   10,  12,  15,
    36,  38,  39,  40,  42,  43,  45,  46,  48,  49,  51, 52,  54,  56,  57,
    59,  60,  62,  63,  32,  33,  37,  64,  65,  67,  68, 69,  70,  72,  73,
    74,  75,  77,  78,  79,  48,  50,  50,  51,  52,  53, 54,  55,  56,  57,
    58,  59,  61,  61,  65,  80,  81,  82,  83,  84,  86, 87,  87,  72,  72,
    74,  74,  75,  77,  77,  80,  88,  89,  90,  91,  92, 93,  86,  88,  95,
    96,  97,  99,  99,  93,  95,  101, 102, 103, 104, 99, 105, 106, 107, 103,
    105, 108, 109, 110, 111, 110, 112, 112};

static uint8_t mps_next_index[113] = {
    1,   2,   3,   4,   5,   6,   7,   8,  9,  10,  11, 12,  13,  13,  15,
    16,  17,  18,  19,  20,  21,  22,  23, 24, 25,  26, 27,  28,  29,  30,
    31,  32,  33,  34,  35,  9,   37,  38, 39, 40,  41, 42,  43,  44,  45,
    46,  47,  48,  49,  50,  51,  52,  53, 54, 55,  56, 57,  58,  59,  60,
    61,  62,  63,  32,  65,  66,  67,  68, 69, 70,  71, 72,  73,  74,  75,
    76,  77,  78,  79,  48,  81,  82,  83, 84, 85,  86, 87,  71,  89,  90,
    91,  92,  93,  94,  86,  96,  97,  98, 99, 100, 93, 102, 103, 104, 99,
    106, 107, 103, 109, 107, 111, 109, 111};

static uint8_t switch_mps[113] = {
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1};

typedef struct {
  UtObject object;

  // State machine
  uint8_t s;

  // Last read byte.
  uint8_t data;

  // Number of unused bits in [data].
  size_t ct;

  uint16_t a;

  uint16_t c;

  // The current bit value of the most probable symbol.
  uint8_t mps;
} UtJpegArithmeticDecoder;

static uint16_t get_qe(UtJpegArithmeticDecoder *self) {
  return qe_values[self->s];
}

static bool get_switch_mps(UtJpegArithmeticDecoder *self) {
  return switch_mps[self->s] != 0;
}

static void next_lps(UtJpegArithmeticDecoder *self) {
  self->s = lps_next_index[self->s];
}

static void next_mps(UtJpegArithmeticDecoder *self) {
  self->s = mps_next_index[self->s];
}

// Read a the next scan byte from [data] and write it to [value].
static bool read_scan_byte(UtJpegArithmeticDecoder *self, UtObject *data,
                           size_t *offset) {
  size_t data_length = ut_list_get_length(data);

  size_t o = *offset;
  if (o >= data_length) {
    return false;
  }

  uint8_t byte1 = ut_uint8_list_get_element(data, o++);

  // Scan data terminates on a marker. If 0xff is in the scan data, 0x00 is
  // after it so it can't be a valid marker. The 0x00 is dropped.
  if (byte1 == 0xff) {
    if (o >= data_length) {
      return false;
    }
    uint8_t byte2 = ut_uint8_list_get_element(data, o++);
    if (byte2 != 0x00) {
      // self->state = DECODER_STATE_MARKER;
      return false;
    }
  }

  *offset = o;
  self->data = byte1;
  self->ct += 8;
  return true;
}

static bool renormalize(UtJpegArithmeticDecoder *self, UtObject *data,
                        size_t *offset) {
  do {
    if (self->ct == 16) {
      if (!read_scan_byte(self, data, offset)) {
        return false;
      }
    }
    self->a <<= 1;
    self->c <<= 1;
    self->c |= self->data >> 7;
    self->data <<= 1;
    if (self->ct == 0) {
      return true; // FIXME: End of data?
    }
    self->ct--;
  } while (self->a < 0x8000);

  return true;
}

/// Invert the MPS bit if the state machine is set to.
static void maybe_switch_mps(UtJpegArithmeticDecoder *self) {
  if (get_switch_mps(self)) {
    self->mps ^= 0x1;
  }
}

/// Returns the bit used by the least possible symbol.
static uint8_t get_lps(UtJpegArithmeticDecoder *self) {
  return self->mps ^ 0x1;
}

static uint8_t cond_mps_exchange(UtJpegArithmeticDecoder *self) {
  uint8_t d;
  if (self->a < get_qe(self)) {
    d = get_lps(self);
    maybe_switch_mps(self);
    next_lps(self);
  } else {
    d = self->mps;
    next_mps(self);
  }

  return d;
}

static uint8_t cond_lps_exchange(UtJpegArithmeticDecoder *self) {
  self->c -= self->a;

  uint16_t qe = get_qe(self);
  uint8_t d;
  if (self->a < qe) {
    d = self->mps;
    next_mps(self);
  } else {
    d = get_lps(self);
    maybe_switch_mps(self);
    next_lps(self);
  }

  self->a = qe;

  return d;
}

static void ut_jpeg_arithmetic_decoder_init(UtObject *object) {}

static void ut_jpeg_arithmetic_decoder_cleanup(UtObject *object) {}

static UtObjectInterface object_interface = {
    .type_name = "UtJpegArithmeticDecoder",
    .init = ut_jpeg_arithmetic_decoder_init,
    .cleanup = ut_jpeg_arithmetic_decoder_cleanup};

UtObject *ut_jpeg_arithmetic_decoder_new() {
  return ut_object_new(sizeof(UtJpegArithmeticDecoder), &object_interface);
}

bool ut_jpeg_arithmetic_decoder_read_bit(UtObject *object, UtObject *data,
                                         size_t *offset, uint8_t *bit) {
  assert(ut_object_is_jpeg_arithmetic_decoder(object));
  UtJpegArithmeticDecoder *self = (UtJpegArithmeticDecoder *)object;

  self->a -= get_qe(self);
  if (self->c < self->a) {
    if (self->a < 0x8000) {
      *bit = cond_mps_exchange(self);
      renormalize(self, data, offset);
      return true;
    } else {
      *bit = self->mps;
      return true;
    }
  } else {
    *bit = cond_lps_exchange(self);
    return renormalize(self, data, offset);
  }
}

bool ut_object_is_jpeg_arithmetic_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
