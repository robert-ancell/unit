#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  uint8_t s;
} ArithmeticStateMachine;

typedef struct {
  ArithmeticStateMachine state_machine;

  uint32_t a;
  uint32_t c;

  size_t ct;

  uint8_t mps;

  uint8_t st;

  uint8_t output[32];
  size_t output_length;
} ArithmeticEncoder;

typedef struct {
  ArithmeticStateMachine state_machine;

  uint16_t a;
  uint16_t c;

  uint8_t data;

  size_t ct;

  uint8_t mps;

  const uint8_t *input;
  size_t input_length;
  size_t input_offset;
} ArithmeticDecoder;

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

static void init_state_machine(ArithmeticStateMachine *self) { self->s = 0; }

static uint16_t get_qe(ArithmeticStateMachine *self) {
  return qe_values[self->s];
}

static bool get_switch_mps(ArithmeticStateMachine *self) {
  return switch_mps[self->s] != 0;
}

static void next_lps(ArithmeticStateMachine *self) {
  self->s = lps_next_index[self->s];
}

static void next_mps(ArithmeticStateMachine *self) {
  self->s = mps_next_index[self->s];
}

static void byte_out(ArithmeticEncoder *self) {
  uint32_t t = self->c >> 19;
  if (t > 0xff) {
    self->output[self->output_length - 1]++;

    // Stuff zero
    if (self->output[self->output_length - 1] == 0xff) {
      self->output_length++;
      self->output[self->output_length] = 0x00;
    }

    // Output stacked zeros
    while (self->st > 0) {
      self->output[self->output_length] = 0x00;
      self->output_length++;
      self->st--;
    }

    self->output[self->output_length] = t & 0xff;
    self->output_length++;
  } else if (t == 0xff) {
    self->st++;
  } else {
    // Output stacked ffs
    while (self->st > 0) {
      self->output[self->output_length] = 0xff;
      self->output_length++;
      self->output[self->output_length] = 0x00;
      self->output_length++;
      self->st--;
    }

    self->output[self->output_length] = t;
    self->output_length++;
  }
  self->c &= 0x7ffff;
}

static void renorm_e(ArithmeticEncoder *self) {
  do {
    self->a <<= 1;
    self->c <<= 1;
    self->ct--;

    if (self->ct == 0) {
      byte_out(self);
      self->ct = 8;
    }
  } while (self->a < 0x8000);
}

// Code the least possible symbol.
static void encode_lps(ArithmeticEncoder *self) {
  uint16_t qe = get_qe(&self->state_machine);
  self->a -= qe;
  if (self->a >= qe) {
    self->c += self->a;
    self->a = qe;
  }

  // Estimate Qe
  if (get_switch_mps(&self->state_machine)) {
    self->mps ^= 0x1;
  }
  next_lps(&self->state_machine);

  renorm_e(self);
}

// Code the most possible symbol.
static void encode_mps(ArithmeticEncoder *self) {
  uint16_t qe = get_qe(&self->state_machine);
  self->a -= qe;
  if (self->a >= 0x8000) {
    return;
  }

  if (self->a < qe) {
    self->c += self->a;
    self->a = qe;
  }

  // Estimate Qe
  next_mps(&self->state_machine);

  renorm_e(self);
}

static void encode_bit(ArithmeticEncoder *self, uint8_t bit) {
  if (bit == self->mps) {
    encode_mps(self);
  } else {
    encode_lps(self);
  }
}

static void flush(ArithmeticEncoder *self) {
  // Clear final bits.
  uint32_t t = (self->c + self->a - 1) & 0xffff0000;
  if (t < self->c) {
    t += 0x8000;
  }
  self->c = t;

  self->c <<= self->ct;
  byte_out(self);

  self->c <<= 8;
  byte_out(self);

  // Discard final zeros FIXME
  while (self->output_length > 0 &&
         self->output[self->output_length - 1] == 0) {
    self->output_length--;
  }
  if (self->output_length > 0 &&
      self->output[self->output_length - 1] == 0xff) {
    self->output_length++;
  }
}

void jpeg_encode_arithmetic(const uint8_t *data, size_t data_length) {
  ArithmeticEncoder self;
  init_state_machine(&self.state_machine);
  self.a = 0x10000;
  self.c = 0;
  self.ct = 11;
  self.mps = 0;
  self.st = 0;
  self.output_length = 0;
  self.output[0] = 0x00;

  // printf(" EC   D MPS    Qe     A         C  CT ST Bx B\n");
  // size_t last_output_length = 0;
  for (size_t i = 0; i < data_length * 8; i++) {
    uint8_t d = data[i / 8] >> (7 - (i % 8)) & 0x1;
    // printf("%3zi   %d   %d  %04X  %04X  %08X  %2zi %2d  ?", i + 1, d,
    // self.mps,
    //        qe_values[self.s], self.a & 0xffff, self.c, self.ct, self.st);
    // for (size_t i = last_output_length; i < self.output_length; i++) {
    //   printf(" %02x", self.output[i]);
    // }
    // last_output_length = self.output_length;
    // printf("\n");
    encode_bit(&self, d);
  }

  // uint32_t c = self.c;
  flush(&self);
  // printf("Fsh                %04X  %08X  %2zi %2d  ?", self.a & 0xffff, c,
  //        self.ct, self.st);
  // for (size_t i = last_output_length; i < self.output_length; i++) {
  //   printf(" %02x", self.output[i]);
  // }
  // last_output_length = self.output_length;
  // printf("\n");

  // Add marker
  self.output[self.output_length] = 0xff;
  self.output_length++;
  self.output[self.output_length] = 0xd9;
  self.output_length++;

  for (size_t i = 0; i < self.output_length; i++) {
    printf("%02x", self.output[i]);
  }
  printf("\n");
}

static void byte_in(ArithmeticDecoder *self) {
  if (self->input_offset >= self->input_length) {
    return;
  }

  uint8_t byte = self->input[self->input_offset++];
  if (byte == 0xff && self->input_offset < self->input_length) {
    if (self->input[self->input_offset++] != 0x00) {
      printf("!!\n");
      return;
    }
  }

  self->data = byte;
  self->ct += 8;

  return;
}

static bool renorm_d(ArithmeticDecoder *self) {
  do {
    if (self->ct == 16) {
      byte_in(self);
    }
    self->a <<= 1;
    self->c <<= 1;
    self->c |= self->data >> 7;
    self->data <<= 1;
    if (self->ct == 0) {
      return false;
    }
    self->ct--;
  } while (self->a < 0x8000);

  return true;
}

static uint8_t cond_mps_exchange(ArithmeticDecoder *self) {
  uint8_t d;
  if (self->a < get_qe(&self->state_machine)) {
    d = self->mps ^ 0x1;
    if (get_switch_mps(&self->state_machine)) {
      self->mps ^= 0x1;
    }
    next_lps(&self->state_machine);
  } else {
    d = self->mps;
    next_mps(&self->state_machine);
  }

  return d;
}

static uint8_t cond_lps_exchange(ArithmeticDecoder *self) {
  self->c -= self->a;

  uint16_t qe = get_qe(&self->state_machine);
  uint8_t d;
  if (self->a < qe) {
    d = self->mps;
    next_mps(&self->state_machine);
  } else {
    d = self->mps ^ 0x1;
    if (get_switch_mps(&self->state_machine)) {
      self->mps ^= 0x1;
    }
    next_lps(&self->state_machine);
  }

  self->a = qe;

  return d;
}

static bool decode_bit(ArithmeticDecoder *self, uint8_t *d) {
  self->a -= get_qe(&self->state_machine);
  if (self->c < self->a) {
    if (self->a < 0x8000) {
      *d = cond_mps_exchange(self);
      return renorm_d(self);
    } else {
      *d = self->mps;
      return true;
    }
  } else {
    *d = cond_lps_exchange(self);
    return renorm_d(self);
  }
}

void jpeg_decode_arithmetic(const uint8_t *data, size_t data_length) {
  ArithmeticDecoder self;

  init_state_machine(&self.state_machine);
  self.a = 0;
  self.c = 0;
  self.ct = 0;
  self.mps = 0;

  self.input = data;
  self.input_length = data_length;
  self.input_offset = 0;

  byte_in(&self);
  self.c = self.data << 8;
  byte_in(&self);
  self.c |= self.data;
  self.data = 0;

  uint8_t d;
  size_t shift = 7;
  uint8_t byte = 0x00;
  while (decode_bit(&self, &d)) {
    byte |= d << shift;
    if (shift == 0) {
      printf("%02x", byte);
      byte = 0x00;
      shift = 7;
    } else {
      shift--;
    }
  }
  if (shift != 7) {
    printf("%02x", byte);
  }
  printf("\n");
}

int main(int argc, char **argv) {
  const uint8_t test_data[32] = {
      0x00, 0x02, 0x00, 0x51, 0x00, 0x00, 0x00, 0xC0, 0x03, 0x52, 0x87,
      0x2A, 0xAA, 0xAA, 0xAA, 0xAA, 0x82, 0xC0, 0x20, 0x00, 0xFC, 0xD7,
      0x9E, 0xF6, 0x74, 0xEA, 0xAB, 0xF7, 0x69, 0x7E, 0xE7, 0x4C};
  for (size_t i = 0; i < 32; i++) {
    printf("%02x", test_data[i]);
  }
  printf("\n");
  jpeg_encode_arithmetic(test_data, 32);

  const uint8_t expected_data[31] = {
      0x65, 0x5B, 0x51, 0x44, 0xF7, 0x96, 0x9D, 0x51, 0x78, 0x55, 0xBF,
      0xFF, 0x00, 0xFC, 0x51, 0x84, 0xC7, 0xCE, 0xF9, 0x39, 0x00, 0x28,
      0x7D, 0x46, 0x70, 0x8E, 0xCB, 0xC0, 0xF6, 0xFF, 0xD9};
  for (size_t i = 0; i < 31; i++) {
    printf("%02x", expected_data[i]);
  }
  printf("\n");

  jpeg_decode_arithmetic(expected_data, 29);
}
