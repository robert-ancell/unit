#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtDateTime] object with a timezone offset.
/// !return-ref
UtObject *ut_date_time_new(unsigned int year, unsigned int month,
                           unsigned int day, unsigned int hour,
                           unsigned int minutes, unsigned int seconds,
                           int utc_offset);

/// Creates a new [UtDateTime] object in UTC.
/// !return-ref
UtObject *ut_date_time_new_utc(unsigned int year, unsigned int month,
                               unsigned int day, unsigned int hour,
                               unsigned int minutes, unsigned int seconds);

/// Gets the year of [object].
unsigned int ut_date_time_get_year(UtObject *object);

/// Gets the month of [object].
unsigned int ut_date_time_get_month(UtObject *object);

/// Gets the day of [object].
unsigned int ut_date_time_get_day(UtObject *object);

/// Gets the hour of [object].
unsigned int ut_date_time_get_hour(UtObject *object);

/// Gets the minutes of [object].
unsigned int ut_date_time_get_minutes(UtObject *object);

/// Gets the seconds of [object].
unsigned int ut_date_time_get_seconds(UtObject *object);

/// Gets the offset from UTC in minutes of [object].
int ut_date_time_get_utc_offset(UtObject *object);

/// Returns [true] if [object] is a [UtDateTime].
bool ut_object_is_date_time(UtObject *object);
