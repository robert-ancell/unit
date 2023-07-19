#include <assert.h>
#include <stdio.h>

#include "ut.h"

typedef struct {
  UtObject object;
  unsigned int year;
  unsigned int month;
  unsigned int day;
  unsigned int hour;
  unsigned int minutes;
  unsigned int seconds;
} UtDateTime;

static char *ut_date_time_to_string(UtObject *object) {
  UtDateTime *self = (UtDateTime *)object;
  return ut_cstring_new_printf("<UtDateTime>(%u, %u, %u, %u, %u, %u)",
                               self->year, self->month, self->day, self->hour,
                               self->minutes, self->seconds);
}

static bool ut_date_time_equal(UtObject *object, UtObject *other) {
  UtDateTime *self = (UtDateTime *)object;
  if (!ut_object_is_date_time(other)) {
    return false;
  }
  UtDateTime *other_self = (UtDateTime *)other;
  return self->year == other_self->year && self->month == other_self->month &&
         self->day == other_self->day && self->hour == other_self->hour &&
         self->minutes == other_self->minutes &&
         self->seconds == other_self->seconds;
}

static UtObjectInterface object_interface = {.type_name = "UtDateTime",
                                             .to_string =
                                                 ut_date_time_to_string,
                                             .equal = ut_date_time_equal};

UtObject *ut_date_time_new(unsigned int year, unsigned int month,
                           unsigned int day, unsigned int hour,
                           unsigned int minutes, unsigned int seconds) {
  UtObject *object = ut_object_new(sizeof(UtDateTime), &object_interface);
  UtDateTime *self = (UtDateTime *)object;
  assert(month >= 1 && month <= 12);
  assert(day >= 1 && month <= 31);
  self->year = year;
  self->month = month;
  self->day = day;
  self->hour = hour;
  self->minutes = minutes;
  self->seconds = seconds;
  return object;
}

unsigned int ut_date_time_get_year(UtObject *object) {
  assert(ut_object_is_date_time(object));
  UtDateTime *self = (UtDateTime *)object;
  return self->year;
}

unsigned int ut_date_time_get_month(UtObject *object) {
  assert(ut_object_is_date_time(object));
  UtDateTime *self = (UtDateTime *)object;
  return self->month;
}

unsigned int ut_date_time_get_day(UtObject *object) {
  assert(ut_object_is_date_time(object));
  UtDateTime *self = (UtDateTime *)object;
  return self->day;
}

unsigned int ut_date_time_get_hour(UtObject *object) {
  assert(ut_object_is_date_time(object));
  UtDateTime *self = (UtDateTime *)object;
  return self->hour;
}

unsigned int ut_date_time_get_minutes(UtObject *object) {
  assert(ut_object_is_date_time(object));
  UtDateTime *self = (UtDateTime *)object;
  return self->minutes;
}

unsigned int ut_date_time_get_seconds(UtObject *object) {
  assert(ut_object_is_date_time(object));
  UtDateTime *self = (UtDateTime *)object;
  return self->seconds;
}

bool ut_object_is_date_time(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
