#include <stdint.h>

// Precaclulate order values are entered into a data unit.
void jpeg_build_data_unit_order(uint8_t *order);

// Precaculate values used in DCT calculations.
void jpeg_build_dct_values(float *dct_alpha, float *dct_cos);

// Perform discrete cosine transform on [data_unit] and write to
// [encoded_data_unit].
void jpeg_dct(float *dct_alpha, float *dct_cos, const int16_t *data_unit,
              float *encoded_data_unit);

// Perform inverse discrete cosine transform on [encoded_data_unit] and write to
// [data_unit].
void jpeg_inverse_dct(float *dct_alpha, float *dct_cos,
                      const int16_t *encoded_data_unit, int16_t *data_unit);
