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
#define APIKEY "83a3a133bc7541a6608536d490f7a11d"

// Forward Declarations //
char * locate_cache (void);
int check_if_stale (const char * cache_path);

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

            c = getopt_long(argc, argv, "himrc:f:l:", options, &option_index);
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

                case 'r':
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

    if ( !cache_path ) { cache_path = locate_cache(); };

    if ( flag_refresh > 0 || check_if_stale(cache_path) ) {
        json = fetch_data_owm('q', location, flag_scale, cache_path, APIKEY);
    } else {
        json = fetch_data_file(cache_path);
        // TODO: Also freshen if requested location differs from cache location
        //if ( !strstr(json->data, location) ) {
        //    free(json->data);
        //    json = fetch_data_owm('q', location, flag_scale, cache_path, APIKEY);
        //}
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

char * locate_cache (void) {
    char * cache_path;
    char * xdg_config_home = getenv("XDG_CONFIG_HOME");

    if ( xdg_config_home ) {
        size_t xdg_conf_len = strlen(xdg_config_home);
        char * xdg_conf_shaman = malloc(xdg_conf_len + 9);

        snprintf(xdg_conf_shaman, xdg_conf_len + 8, "%s/shaman", xdg_config_home);
        int error = mkdir(xdg_conf_shaman, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); // mode == 0755

        if ( error && errno != EEXIST ) {
            fprintf(stderr, "Error checking cache in $XDG_CONFIG_HOME/.shaman: %s\n", strerror(errno));
            free(xdg_conf_shaman);
            exit(1);
        } else {
            size_t xdg_conf_shaman_len = strlen(xdg_conf_shaman);
            cache_path = malloc(xdg_conf_shaman_len + 13);
            snprintf(cache_path, xdg_conf_shaman_len + 12, "%s/cache.json", xdg_conf_shaman);
        }

        free(xdg_conf_shaman);

    } else {
        char * home = getenv("HOME");
        size_t home_len = strlen(home);
        char * home_shaman = malloc(home_len + 10);

        snprintf(home_shaman, home_len + 9, "%s/.shaman", home);
        int error = mkdir(home_shaman, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); // mode == 0755

        if ( error && errno != EEXIST ) {
            fprintf(stderr, "Error checking cache in $HOME/.shaman: %s\n", strerror(errno));
            free(home_shaman);
            exit(1);
        } else {
            size_t home_shaman_len = strlen(home_shaman);
            cache_path = malloc(home_shaman_len + 13);
            snprintf(cache_path, home_shaman_len + 12, "%s/cache.json", home_shaman);
        }

        free(home_shaman);
    }

    return cache_path;
}

int check_if_stale (const char * cache_path) {
    struct stat cache_stats;
    int status = stat(cache_path, &cache_stats);
    int errsv = errno;

    if ( status == 0 ) {
        return ((time(NULL) - cache_stats.st_mtim.tv_sec) >= 600);
    } else {
        fprintf(stderr, "Determining if cache should be freshened failed: %s\n", strerror(errsv));
        return -1;
    }
}

// vim: set ts=4 sw=4 et:
