#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_color_new_rgba(double red, double green, double blue,
                            double alpha);

UtObject *ut_color_new_from_hex_string(const char *hex_string);

double ut_color_get_red(UtObject *object);

double ut_color_get_green(UtObject *object);

double ut_color_get_blue(UtObject *object);

double ut_color_get_alpha(UtObject *object);

bool ut_object_is_color(UtObject *object);