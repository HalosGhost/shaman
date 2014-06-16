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
#include "usage.h"

// Main Function //
int main (int argc, char ** argv) {
    char flag_scale = 'i';
    char flag_refresh = 0;
    char * format;
    char * location;
    char * cache_path = NULL;

    if ( argc <= 1 ) { 
        _usage(1); 
    } else {
        int c;

        while ( c != -1 ) {
            static struct option options [] = {
                /* Flags */
                { "help",     no_argument,         0, 'h'   },
                { "imperial", no_argument,         0, 'i'   },
                { "metric",   no_argument,         0, 'm'   },
                { "refresh",  no_argument,         0, 'r'   },
                /* Swtiches */
                { "cache",    required_argument,   0, 'c'   },
                { "format",   required_argument,   0, 'f'   },
                { "location", required_argument,   0, 'l'   },
                { 0,          0,                   0, 0     },
            };

            int option_index = 0;

            c = getopt_long(argc, argv, "himc:f:l:", options, &option_index);
            switch ( c ) {
                case 'h':
                    _usage(0);
                    break;

                case 'i':
                    flag_scale = 'i';
                    break;

                case 'm':
                    flag_scale = 'm';
                    break;

                case 'x':
                    flag_refresh ++;
                    break;

                case 'c': {
                    size_t cache_path_length = strlen(optarg) + 1;
                    cache_path = malloc(cache_path_length);
                    snprintf(cache_path, cache_path_length, "%s", optarg);
                    break;
                }

                case 'f': {
                    size_t format_length = strlen(optarg) + 1;
                    format = malloc(format_length);
                    strncpy(format, optarg, format_length);
                    break;
                }

                case 'l': {
                    // TODO: Correctly encode location
                    size_t location_length = strlen(optarg) + 1;
                    location = malloc(location_length);
                    snprintf(location, location_length, "%s", optarg);
                    break;
                }
            }
        }
    }

    struct json_write_result * json;

    if ( !cache_path ) {
        // TODO: Correctly check for default cache
        json = fetch_data_owm('q', location, flag_scale, cache_path);
    } else {
        // TODO: Handle cache freshening
        json = fetch_data_file(cache_path);
    }

    if ( cache_path ) { free(cache_path); };
    if ( location ) { free(location); };

    struct weather * weather = read_weather(json);
    char output_string [1024] = {'\0'};

    strfweather(output_string, 1024, format, weather);
    if ( format ) { free(format); };

    printf("%s\n", output_string);

    return 0;
}

// vim: set ts=4 sw=4 et:
