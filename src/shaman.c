/*******************************************************************\
* A small, native C utility for fetching weather                    *
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
#include <getopt.h>
#include "weather.h"

// Main Function //
int main (int argc, char ** argv) {
//    struct json_write_result * test = fetch_data_file("./test/test.json");
    struct json_write_result * test = fetch_data_owm('q', "Saint%20Paul,US", 'i', NULL);
    struct weather * weather = read_weather(test);
	char test_string [1024] = {'\0'};

	strfweather(test_string, 1024, "%a (%t°F)", weather);

	printf("%s\n", test_string);

    return 0;
}

// vim: set ts=4 sw=4 et:
