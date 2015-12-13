/*******************************************************************\
* A small, native C test suite for libweather                       *
* Copyright (C) 2013-2014, Sam Stuewe                               *
*                                                                   *
* This program is free software; you can redistribute it and/or     *
* modify it under the terms of the GNU General Public License       *
* as published by the Free Software Foundation; either version 2    *
* of the License, or (at your option) any later version.            *
*                                                                   *
* This program is distributed in the hope that it will be useful,   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of    *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the      *
* GNU General Public License for more details.                      *
*                                                                   *
* You should have received a copy of the GNU General Public License *
* along with this program; if not, write to the Free Software       *
* Foundation, Inc., 51 Franklin Street, Fifth Floor,                *
* Boston, MA  02110-1301, USA.                                      *
\*******************************************************************/

// Libraries //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include "../src/weather.h"

// Forward Declarations //

/**
 * If you fork this code, please DO NOT use this key.
 * Getting an API key with OWM is free, but this key is exclusively
 * for shaman
 */
#define OWMAPIKEY "83a3a133bc7541a6608536d490f7a11d"

typedef bool (* test_p) (void);

struct test {
    char * desc;
    test_p func;
};

static struct json_write_result * json;

#define APPROX_EQUALITY_EPSILON 1e-7

/* Test Utilities */
bool
run_test (char * test_name, test_p function);

char *
test_result_text (bool passed);

bool
approx_equal (double value1, double value2);

/* Test Functions */
bool
test_strfweather (void);

bool
test_owm_local_fetch (void);

bool
test_owm_local_parse (void);

bool
test_owm_remote_fetch (void);

bool
test_owm_cache (void);

bool
test_owm_remote_parse (void);

bool
test_shaman_owm (void);

// Run Suite //
int
main (void) {

    struct test test_list [] = {
        { "Formatted Weather",   (test_p )test_strfweather      },
        { "OWM Local Fetching",  (test_p )test_owm_local_fetch  },
        { "OWM Local Parsing",   (test_p )test_owm_local_parse  },
        { "OWM Remote Fetching", (test_p )test_owm_remote_fetch },
        { "OWM Caching\t",       (test_p )test_owm_cache        },
        { "OWM Remote Parsing",  (test_p )test_owm_remote_parse },
        { "Shaman with OWM\t",   (test_p )test_shaman_owm       }
    }; unsigned test_count = sizeof test_list / sizeof test_list[0];

    unsigned failure_count = 0;
    for ( size_t i = 0; i < test_count; i ++ ) {
        bool passed = run_test(test_list[i].desc, test_list[i].func);
        failure_count += !passed;
    }

    bool suite_passed = (failure_count == 0);
    printf("\t\t\t\t\t--------\nPassed %u/%u tests\t\t\t[ %s \x1b[0m]\n",
           test_count - failure_count, test_count,
           test_result_text(suite_passed));
    return !suite_passed;
}

bool
run_test (char * test_nm, test_p test) {

    printf("Testing %s\t\t[ PEND ]\r", test_nm);
    bool passed = test();
    printf("Testing %s\t\t[ %s \x1b[0m]\n", test_nm, test_result_text(passed));
    return passed;
}

char *
test_result_text(bool passed) {

    return (passed ? "\x1b[32mPASS" : "\x1b[31mFAIL");
}

bool
test_strfweather (void) {

    struct weather wthr = {
        .dt = 1402513288,
        .pressure = 1011,
        .condition = "Sky is Clear",
        .weather_code = 800,
        .clouds = 18,
        .temp_min = 305.15,
        .temp_max = 307.59,
        .id = 4693342,
        .name = "Georgetown",
        .country = "US",
        .latitude = 30.63,
        .longitude = -97.68,
        .precipitation_3h = 18,
        .humidity = 62,
        .sunrise = 1402486044,
        .sunset = 1402536816,
        .temperature = 306.35,
        .wind_speed = 1.54,
        .wind_gust = 5.14,
        .wind_direction = 214
    };

    char * comp_str = "14025132881011Sky is Clear80018305.15307.594693342George"
                      "townUS30.63-97.68186214024860441402536816306.351.545.14"
                      "214";

    char * dest_str = malloc(115);
    strfweather(dest_str, 115, "%a%b%c%C%d%h%H%i%I%j%l%L%p%P%s%S%t%w%W%x%X",
                &wthr);

    int test_result = strncmp(comp_str, dest_str, 114);

    if ( dest_str ) { free(dest_str); };

    return (test_result == 0);
}

bool
test_owm_local_fetch (void) {

    json = owm_fetch_local("test.json");
    return (*json->data);
}

bool
test_owm_local_parse (void) {

    struct weather * w = owm_read(json);
    int failed_test_counter = 0;

    if ( strcmp(w->country, "US") != 0 ) { failed_test_counter ++; };
    if ( strcmp(w->name, "Georgetown") != 0 ) { failed_test_counter ++; };
    if ( strcmp(w->condition, "Sky is Clear") != 0 ) {
        failed_test_counter ++;
    }

    if ( !approx_equal(w->latitude, 30.63) ) { failed_test_counter ++; };
    if ( !approx_equal(w->longitude, -97.68) ) { failed_test_counter ++; };
    if ( w->sunrise != 1402486044 ) { failed_test_counter ++; };
    if ( w->sunset != 1402536816 ) { failed_test_counter ++; };
    if ( w->weather_code != 800 ) { failed_test_counter ++; };
    if ( !approx_equal(w->temperature, 306.35) ) { failed_test_counter ++; };
    if ( !approx_equal(w->pressure, 1011) ) { failed_test_counter ++; };
    if ( !approx_equal(w->temp_min, 305.15) ) { failed_test_counter ++; };
    if ( !approx_equal(w->temp_max, 307.59) ) { failed_test_counter ++; };
    if ( !approx_equal(w->humidity, 62) ) { failed_test_counter ++; };
    if ( !approx_equal(w->wind_speed, 1.54) ) { failed_test_counter ++; };
    if ( !approx_equal(w->wind_gust, 5.14) ) { failed_test_counter ++; };
    if ( !approx_equal(w->wind_direction, 214) ) { failed_test_counter ++; };
    if ( !approx_equal(w->precipitation_3h, 18) ) { failed_test_counter ++; };
    if ( !approx_equal(w->clouds, 18) ) { failed_test_counter ++; };
    if ( w->dt != 1402513288 ) { failed_test_counter ++; };
    if ( w->id != 4693342 ) { failed_test_counter ++; };

    return (failed_test_counter == 0);
}

bool
test_owm_remote_fetch (void) {

    json = owm_fetch_remote('q', "Saint Paul,US", 'i', NULL, OWMAPIKEY);
    return ( *json->data );
}

bool
test_owm_cache (void) {

    char * test_path = ".cache_test.json";
    json = owm_fetch_remote('q', "Saint Paul,US", 'i', test_path, OWMAPIKEY);

    if ( json->data ) {
        json = owm_fetch_local(test_path);
        unlink(test_path);

        return ( *json->data );
    } return false;
}

bool
test_owm_remote_parse (void) {

    struct weather * w = owm_read(json);

    if ( !w ) {
        return 1;
    }

    int failed_test_counter = 0;

    if ( strcmp(w->country, "US") != 0 ) { failed_test_counter ++; };
    if ( strcmp(w->name, "Saint Paul") != 0 ) { failed_test_counter ++; };
    if ( w->id != 5045360 ) { failed_test_counter ++; };

    return (failed_test_counter == 0);
}

bool
test_shaman_owm (void) {

    return 1;
}

bool
approx_equal (double value1, double value2) {

    return fabs(value2 - value1) < APPROX_EQUALITY_EPSILON;
}

// vim: set ts=4 sw=4 et:
