/*******************************************************************\
* A small, native C library for fetching weather                    *
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
    if ( test2_result ) { return 2; };

    struct json_write_result * test3_result = test_remote_fetching();
    if ( !test1_result->data ) { return 3; };

    int test4_result = test_remote_parsing(test3_result);
    if ( test4_result ) { return 4; };

    return 0;
}

struct json_write_result * test_local_fetching (void) {
    printf("Testing local JSON Fetching\t[ PEND ]\r");
    struct json_write_result * test = fetch_data_file("/home/halosghost/Projects/shaman/src/test.json");
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

    printf("Testing local JSON Parsing\t[ %s ]\n", (failed_test_counter == 0 ? "PASS" : "FAIL"));

    free(weather->name);
    free(weather->country);

    return failed_test_counter;
}

struct json_write_result * test_remote_fetching (void) {
    printf("Testing remote JSON Fetching\t[ PEND ]\r");
    struct json_write_result * test = fetch_data_owm('q', "Saint%20Paul,us", 'i');
    printf("Testing remote JSON Fetching\t[ %s ]\n", (test->data ? "PASS" : "FAIL"));

    if ( test->data ) {
        return test;
    } else {
        return NULL;
    }
}

int test_remote_parsing (struct json_write_result * test) {
    printf("Testing remote JSON Parsing\t[ PEND ]\r");
    struct weather * weather = read_weather(test);
    int failed_test_counter = 0;

    printf("Testing remote JSON Parsing\t[ %s ]\n", (failed_test_counter == 0 ? "PASS" : "FAIL"));

    free(weather->name);
    free(weather->country);

    return failed_test_counter;
}

// vim: set ts=4 sw=4 et:
