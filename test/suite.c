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
#include "weather.h"

// Forward Declarations //
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int test_strfweather (void);

/* OWM */
struct json_write_result * test_owm_local_fetch (void);
int test_owm_local_parse (struct json_write_result * test);
struct json_write_result * test_owm_remote_fetch (void);
int test_owm_remote_parse (struct json_write_result * test);

/* Shaman */


// Run Suite //
int main (void) {
    int strfweather_result = test_strfweather();
    if ( strfweather_result != 0 ) { return 1; };

    struct json_write_result * olf_result = test_owm_local_fetch();
    if ( !olf_result->data ) { return 2; };

    int olp_result = test_owm_local_parse(olf_result);
    if ( olp_result ) { return 3; };

    struct json_write_result * orf_result = test_owm_remote_fetch();
    if ( !orf_result->data ) { return 4; };

    int orp_result = test_owm_remote_parse(orf_result);
    if ( orp_result ) { return 5; };

    return 0;
}

int test_strfweather (void) {
    printf("Testing strfweather()\t\t[ PEND ]\r");
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

    char * comp_str = "14025132881011Sky is Clear80018305.15307.594693342GeorgetownUS30.63-97.68186214024860441402536816306.351.545.14214";
    char * dest_str = malloc(115);
    strfweather(dest_str, 115, "%a%b%c%C%d%h%H%i%I%j%l%L%p%P%s%S%t%w%W%x%X", &wthr);

    int test = strncmp(comp_str, dest_str, 114);

    if ( dest_str ) { free(dest_str); };

    printf("Testing strfweather()\t\t[ %s " ANSI_COLOR_RESET "]\n", (test == 0 ? ANSI_COLOR_GREEN "PASS" : ANSI_COLOR_RED "FAIL"));

    return test;
}

struct json_write_result * test_owm_local_fetch (void) {
    printf("Testing OWM Local Fetching\t[ PEND ]\r");
    struct json_write_result * test = fetch_data_file("test.json");
    printf("Testing OWM Local Fetching\t[ %s " ANSI_COLOR_RESET "]\n", (test->data ? ANSI_COLOR_GREEN "PASS" : ANSI_COLOR_RED "FAIL"));

    if ( test->data ) {
        return test;
    } else {
        return NULL;
    }
}

int test_owm_local_parse (struct json_write_result * test) {
    printf("Testing OWM Local Parsing\t[ PEND ]\r");
    struct weather * weather = read_weather(test);
    int failed_test_counter = 0;

    if ( strcmp(weather->country, "US") != 0 ) { failed_test_counter ++; };
    if ( strcmp(weather->name, "Georgetown") != 0 ) { failed_test_counter ++; };
    if ( strcmp(weather->condition, "Sky is Clear") != 0 ) { failed_test_counter ++; };
    if ( weather->latitude != 30.63 ) { failed_test_counter ++; };
    if ( weather->longitude != -97.68 ) { failed_test_counter ++; };
    if ( weather->sunrise != 1402486044 ) { failed_test_counter ++; };
    if ( weather->sunset != 1402536816 ) { failed_test_counter ++; };
    if ( weather->weather_code != 800 ) { failed_test_counter ++; };
    if ( weather->temperature != 306.35 ) { failed_test_counter ++; };
    if ( weather->pressure != 1011 ) { failed_test_counter ++; };
    if ( weather->temp_min != 305.15 ) { failed_test_counter ++; };
    if ( weather->temp_max != 307.59 ) { failed_test_counter ++; };
    if ( weather->humidity != 62 ) { failed_test_counter ++; };
    if ( weather->wind_speed != 1.54 ) { failed_test_counter ++; };
    if ( weather->wind_gust != 5.14 ) { failed_test_counter ++; };
    if ( weather->wind_direction != 214 ) { failed_test_counter ++; };
    if ( weather->precipitation_3h != 18 ) { failed_test_counter ++; };
    if ( weather->clouds != 18 ) { failed_test_counter ++; };
    if ( weather->dt != 1402513288 ) { failed_test_counter ++; };
    if ( weather->id != 4693342 ) { failed_test_counter ++; };

    printf("Testing OWM Local Parsing\t[ %s " ANSI_COLOR_RESET "]\n", (failed_test_counter == 0 ? ANSI_COLOR_GREEN "PASS" : ANSI_COLOR_RED "FAIL"));

    return failed_test_counter;
}

struct json_write_result * test_owm_remote_fetch (void) {
    char * test_path = "./cache_test.json";
    printf("Testing OWM Remote Fetching\t[ PEND ]\r");
    struct json_write_result * test = fetch_data_owm('q', "Saint Paul,US", 'i', test_path, NULL);
    printf("Testing OWM Remote Fetching\t[ %s " ANSI_COLOR_RESET "]\n", (test->data ? ANSI_COLOR_GREEN "PASS" : ANSI_COLOR_RED "FAIL"));

    if ( test->data ) {
        printf("Testing OWM Caching\t\t[ PEND ]\r");
        struct json_write_result * test2 = fetch_data_file(test_path);
        printf("Testing OWM Caching\t\t[ %s " ANSI_COLOR_RESET "]\n", ( strcmp(test->data, test2->data) == 0 ? ANSI_COLOR_GREEN "PASS" : ANSI_COLOR_RED "FAIL"));
        free(test2->data);
        unlink(test_path);

        return test;
    } else {
        return NULL;
    }
}

int test_owm_remote_parse (struct json_write_result * test) {
    printf("Testing OWM Remote Parsing\t[ PEND ]\r");
    struct weather * weather = read_weather(test);

    if ( !weather ) {
        return 1;
    }

    int failed_test_counter = 0;

    if ( strcmp(weather->country, "US") != 0 ) { failed_test_counter ++; };
    if ( strcmp(weather->name, "Saint Paul") != 0 ) { failed_test_counter ++; };
    if ( weather->id != 5045360 ) { failed_test_counter ++; };

    printf("Testing OWM Remote Parsing\t[ %s " ANSI_COLOR_RESET "]\n", (failed_test_counter == 0 ? ANSI_COLOR_GREEN "PASS" : ANSI_COLOR_RED "FAIL"));

    return failed_test_counter;
}

// vim: set ts=4 sw=4 et:
