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
#include <sys/stat.h>
#include <sys/types.h>
#include "weather.h"
#include "usage.h"

// Main Function //
int main (int argc, char ** argv) {
    char flag_scale = 'i';
    char flag_refresh = 0;
    char * format = NULL;
    char * location = NULL;
    char * cache_path = NULL;

    if ( argc <= 1 ) { 
        _usage(1); 
    } else {
        int c = 0;

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
                    cache_path = strdup(optarg);
                    break;
                }

                case 'f': {
                    format = strdup(optarg);
                    break;
                }

                case 'l': {
                    location = strdup(optarg);
                    break;
                }
            }
        }
    }
    
    if ( !format ) {
        format = malloc(11);
        snprintf(format, 11, "%%c (%%t°%c)", ( flag_scale == 'm' ? 'C' : 'F' ));
    }

    if ( !location ) { _usage(1); };

    struct json_write_result * json;

    if ( !cache_path ) {
        //char * xdg_config_home = getenv("XDG_CONFIG_HOME");
    
        //if ( xdg_config_home ) {
        //    size_t config_home_length = strlen(xdg_config_home);
        //    char * shaman_config_path;
    
        //    snprintf(shaman_config_path, config_home_length + 8, "%s/shaman", xdg_config_home);
        //    int error = mkdir(shaman_config_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // mode == 0644
        //    
        //    if ( !error ) {
        //        size_t shaman_config_length = strlen(shaman_config_path);
        //        snprintf(cache_path, shaman_config_length + 12, "%s/cache.json", shaman_config_path);
        //    } else {
        //        cache_path = NULL;
        //    }
        //} else if (1/* check if $HOME/.config exists */) {
        //    // use $HOME/.config/shaman/cache.json
        //} else {
        //    // use $HOME/.shaman/cache.json
        //}

        json = fetch_data_owm('q', location, flag_scale, cache_path);
    } else {
        // TODO: Handle cache freshening
        json = fetch_data_file(cache_path);
    }

    if ( cache_path ) { free(cache_path); };
    if ( location ) { free(location); };

    struct weather * weather = read_weather(json);
    char output_string [BUFFER_SIZE];

    strfweather(output_string, BUFFER_SIZE, format, weather);

    if ( format ) { free(format); };

    printf("%s\n", output_string);

    return 0;
}

// vim: set ts=4 sw=4 et:
