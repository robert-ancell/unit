#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef dt1 = ut_date_time_new_utc(2023, 7, 19, 20, 54, 46);
  UtObjectRef dt2 = ut_date_time_new_utc(2023, 7, 19, 20, 54, 46);
  UtObjectRef dt3 = ut_date_time_new_utc(1991, 8, 26, 6, 12, 8);

  ut_assert_int_equal(ut_date_time_get_year(dt1), 2023);
  ut_assert_int_equal(ut_date_time_get_month(dt1), 7);
  ut_assert_int_equal(ut_date_time_get_day(dt1), 19);
  ut_assert_int_equal(ut_date_time_get_hour(dt1), 20);
  ut_assert_int_equal(ut_date_time_get_minutes(dt1), 54);
  ut_assert_int_equal(ut_date_time_get_seconds(dt1), 46);
  ut_assert_int_equal(ut_date_time_get_utc_offset(dt1), 0);

  ut_assert_true(ut_object_equal(dt1, dt2));
  ut_assert_false(ut_object_equal(dt1, dt3));

  return 0;
}
