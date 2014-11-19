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
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "weather.h"
#include "usage.h"

/**
 * If you fork this code, please DO NOT use this key.
 * Getting an API key with OWM is free, but this key is exclusively
 * for shaman
 */
#define OWMAPIKEY "83a3a133bc7541a6608536d490f7a11d"

// Forward Declarations //
char * 
locate_cache (char scale);

// Main Function //
int 
main (int argc, char ** argv) {
    char flag_scale = 'i';
    char flag_refresh = 0;
    char flag_verbose = 0;
    char flag_quiet = 0;
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
                { "verbose",  no_argument,         0, 'v'   },
                { "quiet",    no_argument,         0, 'q'   },
                /* Swtiches */
                { "cache",    required_argument,   0, 'c'   },
                { "format",   required_argument,   0, 'f'   },
                { "location", required_argument,   0, 'l'   },
                { 0,          0,                   0, 0     },
            };

            int option_index = 0;
            unsigned long optarg_len;

            c = getopt_long(argc, argv, "himrvqc:f:l:", options, &option_index);
            switch ( c ) {
                case 'h':
                    _usage(0);

                case 'i':
                    flag_scale = 'i';
                    break;

                case 'm':
                    flag_scale = 'm';
                    break;

                case 'r':
                    flag_refresh ++;
                    break;

                case 'v':
                    flag_verbose ++;
                    break;

                case 'q':
                    flag_quiet ++;
                    break;

                case 'c': 
                    optarg_len = sizeof(char) * strlen(optarg) + 1;
                    cache_path = malloc(optarg_len);
                    strncpy(cache_path, optarg, optarg_len);
                    break;

                case 'f': 
                    optarg_len = sizeof(char) * strlen(optarg) + 1;
                    format = malloc(optarg_len);
                    strncpy(format, optarg, optarg_len);
                    break;

                case 'l': 
                    optarg_len = sizeof(char) * strlen(optarg) + 1;
                    location = malloc(optarg_len);
                    strncpy(location, optarg, optarg_len);
                    break;
            }
        }
    }
    
    if ( !format ) {
        format = malloc(11);
        snprintf(format, 11, "%%c (%%t°%c)", (flag_scale == 'm' ? 'C' : 'F'));
    }

    if ( !location ) { _usage(1); };

    if ( !cache_path ) { cache_path = locate_cache(flag_scale); };

    struct weather * wthr = owm_easy('q', location, flag_scale, cache_path, 
                                     (flag_refresh > 0 ? 0 : 600), OWMAPIKEY, 
                                     flag_verbose);

    if ( cache_path ) { free(cache_path); };
    if ( location ) { free(location); };

    char output_string [BUFFER_SIZE];
    strfweather(output_string, BUFFER_SIZE, format, wthr);

    if ( format ) { free(format); };

    if ( !flag_quiet ) {
        printf("%s\n", output_string);
    } else {
        exit(wthr->weather_code / 100); // Report group of weather
    }

    return 0;
}

char * 
locate_cache (char scale) {

    char * cache_path;
    char * conf_prefix = getenv("XDG_CONFIG_HOME");

    char location = ( conf_prefix ? 'x' : 'h' );
    if ( location == 'h' ) { conf_prefix = getenv("HOME"); };

    size_t conf_prefix_len = strlen(conf_prefix);
    char * conf_dir = malloc(conf_prefix_len + 10);

    snprintf(conf_dir, conf_prefix_len + 9, "%s/%s", conf_prefix, 
             (location == 'x' ? "shaman" : ".shaman"));
    int error = mkdir(conf_dir, 
                      S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); 
                      // mode == 0755

    if ( error && errno != EEXIST ) {
        fprintf(stderr, "Error checking cache at %s: %s\n", conf_dir, 
                strerror(errno));

        free(conf_dir);
        exit(1);
    } else {
        size_t conf_dir_len = strlen(conf_dir);
        cache_path = malloc(conf_dir_len + 16);
        snprintf(cache_path, conf_dir_len + 15, 
                 (scale == 'i' ? "%s/imperial.json" : "%s/metric.json"), 
                 conf_dir);
    }

    free(conf_dir);
    return cache_path;
}

// vim: set ts=4 sw=4 et:
