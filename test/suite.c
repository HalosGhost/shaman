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
struct json_write_result * test_local_fetching (void);
int test_local_parsing (struct json_write_result * test);
struct json_write_result * test_remote_fetching (void);
int test_remote_parsing (struct json_write_result * test);

// Run Suite //
int main (void) {
    struct json_write_result * test1_result = test_local_fetching();
    if ( !test1_result->data ) { return 1; };

    int test2_result = test_local_parsing(test1_result);
    if ( test2_result ) { return test2_result; };

    struct json_write_result * test3_result = test_remote_fetching();
    if ( !test1_result->data ) { return 3; };

    int test4_result = test_remote_parsing(test3_result);
    if ( test4_result ) { return 4; };

    return 0;
}

struct json_write_result * test_local_fetching (void) {
    printf("Testing local JSON Fetching\t[ PEND ]\r");
    struct json_write_result * test = fetch_data_file("test.json");
    printf("Testing local JSON Fetching\t[ %s ]\n", (test->data ? "PASS" : "FAIL"));

    if ( test->data ) {
        return test;
    } else {
        return NULL;
    }
}

int test_local_parsing (struct json_write_result * test) {
    printf("Testing local JSON Parsing\t[ PEND ]\r");
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

    printf("Testing local JSON Parsing\t[ %s ]\n", (failed_test_counter == 0 ? "PASS" : "FAIL"));

    free(weather->name);
    free(weather->country);
    free(weather->condition);

    return failed_test_counter;
}

struct json_write_result * test_remote_fetching (void) {
    char * test_path = "./cache_test.json";
    printf("Testing remote JSON Fetching\t[ PEND ]\r");
    struct json_write_result * test = fetch_data_owm('q', "Saint Paul,US", 'i', test_path);
    printf("Testing remote JSON Fetching\t[ %s ]\n", (test->data ? "PASS" : "FAIL"));

    if ( test->data ) {
        printf("Testing JSON Caching\t\t[ PEND ]\r");
        struct json_write_result * test2 = fetch_data_file(test_path);
        printf("Testing JSON Caching\t\t[ %s ]\n", ( strcmp(test->data, test2->data) == 0 ? "PASS" : "FAIL"));
        free(test2->data);
        unlink(test_path);

        return test;
    } else {
        return NULL;
    }
}

int test_remote_parsing (struct json_write_result * test) {
    printf("Testing remote JSON Parsing\t[ PEND ]\r");
    struct weather * weather = read_weather(test);

    if ( !weather ) {
        free(weather->name);
        free(weather->country);
        free(weather->condition);

        return 1;
    }

    int failed_test_counter = 0;

    if ( strcmp(weather->country, "US") != 0 ) { failed_test_counter ++; };
    if ( strcmp(weather->name, "Saint Paul") != 0 ) { failed_test_counter ++; };
    if ( weather->id != 5045360 ) { failed_test_counter ++; };

    printf("Testing remote JSON Parsing\t[ %s ]\n", (failed_test_counter == 0 ? "PASS" : "FAIL"));

    free(weather->name);
    free(weather->country);
    free(weather->condition);

    return failed_test_counter;
}

// vim: set ts=4 sw=4 et:
