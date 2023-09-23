#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new date time for day [year], [month], [day] and time [hour],
/// [minutes], [seconds], offset [utc_offset_minutes] from UTC.
///
/// !return-ref
/// !return-type UtDateTime
UtObject *ut_date_time_new(unsigned int year, unsigned int month,
                           unsigned int day, unsigned int hour,
                           unsigned int minutes, unsigned int seconds,
                           int utc_offset_minutes);

/// Creates a new date time for day [year], [month], [day] and time [hour],
/// [minutes], [seconds] in UTC.
///
/// !return-ref
/// !return-type UtDateTime
UtObject *ut_date_time_new_utc(unsigned int year, unsigned int month,
                               unsigned int day, unsigned int hour,
                               unsigned int minutes, unsigned int seconds);

/// Gets the year of this date time.
unsigned int ut_date_time_get_year(UtObject *object);

/// Gets the month of this date time.
unsigned int ut_date_time_get_month(UtObject *object);

/// Gets the day of this date time.
unsigned int ut_date_time_get_day(UtObject *object);

/// Gets the hour of this date time.
unsigned int ut_date_time_get_hour(UtObject *object);

/// Gets the minutes of this date time.
unsigned int ut_date_time_get_minutes(UtObject *object);

/// Gets the seconds of this date time.
unsigned int ut_date_time_get_seconds(UtObject *object);

/// Gets the offset from UTC in minutes of this date time.
int ut_date_time_get_utc_offset_minutes(UtObject *object);

/// Returns [true] if [object] is a [UtDateTime].
bool ut_object_is_date_time(UtObject *object);
