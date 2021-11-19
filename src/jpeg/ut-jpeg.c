#include <math.h>
#include <stddef.h>
#include <stdint.h>

void jpeg_build_data_unit_order(uint8_t *order) {
  // Data units are encoded in zig-zag order.
  enum { E, SW, S, NE } dir = E;
  size_t x = 0;
  size_t y = 0;
  for (size_t i = 0; i < 64; i++) {
    order[i] = (y * 8) + x;

    // Move to next location and change direction if hit an edge.
    switch (dir) {
    case E:
      x++;
      dir = y == 0 ? SW : NE;
      break;
    case SW:
      x--;
      y++;
      if (x == 0 || y == 7) {
        dir = y == 7 ? E : S;
      }
      break;
    case S:
      y++;
      dir = x == 0 ? NE : SW;
      break;
    case NE:
      x++;
      y--;
      if (x == 7 || y == 0) {
        dir = x == 7 ? S : E;
      }
      break;
    }
  }
}

void jpeg_build_dct_values(float *dct_alpha, float *dct_cos) {
  // 1/√2
  dct_alpha[0] = 0.70710678118654752440;
  for (size_t i = 1; i < 8; i++) {
    dct_alpha[i] = 1.0;
  }

  float pi = 3.14159265358979323846;
  for (size_t u = 0; u < 8; u++) {
    for (size_t v = 0; v < 8; v++) {
      dct_cos[(v * 8) + u] = cos(((2 * u) + 1) * v * pi / 16.0);
    }
  }
}

void jpeg_dct(float *dct_alpha, float *dct_cos, const int16_t *data_unit,
              float *encoded_data_unit) {
  for (size_t u = 0; u < 8; u++) {
    for (size_t v = 0; v < 8; v++) {
      float sum = 0.0;
      for (size_t x = 0; x < 8; x++) {
        for (size_t y = 0; y < 8; y++) {
          sum += data_unit[(y * 8) + x] * dct_cos[(u * 8) + x] *
                 dct_cos[(v * 8) + y];
        }
      }
      encoded_data_unit[(v * 8) + u] = 0.25 * dct_alpha[u] * dct_alpha[v] * sum;
    }
  }
}

void jpeg_inverse_dct(float *dct_alpha, float *dct_cos,
                      const int16_t *encoded_data_unit, int16_t *data_unit) {
  for (size_t x = 0; x < 8; x++) {
    for (size_t y = 0; y < 8; y++) {
      float sum = 0.0;
      for (size_t u = 0; u < 8; u++) {
        for (size_t v = 0; v < 8; v++) {
          sum += dct_alpha[u] * dct_alpha[v] * encoded_data_unit[(v * 8) + u] *
                 dct_cos[(u * 8) + x] * dct_cos[(v * 8) + y];
        }
      }
      data_unit[(y * 8) + x] = round(0.25 * sum);
    }
  }
}
