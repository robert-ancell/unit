#include <stdio.h>

#include "ut-cstring.h"

char *ut_cstring_new_printf(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  char *result = ut_cstring_new_vprintf(format, ap);
  va_end(ap);

  return result;
}

char *ut_cstring_new_vprintf(const char *format, va_list ap) {
  char empty = '\0';
  int description_length = vsnprintf(&empty, 1, format, ap);
  char *result = malloc(sizeof(char) * (description_length + 1));
  vsnprintf(result, description_length + 1, format, ap);
  return result;
}
