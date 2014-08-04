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
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include "weather.h"

const char * c_dir [] = {
    "N", "NbE", "NNE", "NEbN",
    "NE", "NEbE", "ENE", "EbN",
    "E", "EbS", "ESE", "SEbE",
    "SE", "SEbS", "SSE", "SbE",
    "S", "SbW", "SSW", "SWbS",
    "SW", "SWbW", "WSW", "WbS",
    "W", "WbN", "WNW", "NWbW",
    "NW", "NWbN", "NNW", "NbW"
};

// Allow unicode representations of basic
const char * icons [] = {
	"\xe2\x98\x80", // Sun
	"\xe2\x98\xbd", // Moon
	"\xe2\x98\x81", // Clouds
	"\xe2\x98\x94", // Rain
	"\xe2\x9a\xa1", // Thunderstorm
};

// TODO: Add support for formatting time variables
// TODO: Add support for Apparent Temperature
size_t 
strfweather (char * dest_str, size_t n, const char * format, 
             const struct weather * w) {

    int cl = 0;

    for ( ; *format; ++format) {
        if ( *format == '%' ) {
            switch ( *++format ) {
                case '\0':
                    --format;
                    break;

                case 'a':
                    cl += snprintf(dest_str + cl, n - cl, "%ld", w->dt);
                    continue;

                case 'b':
                    cl += snprintf(dest_str + cl, n - cl, "%g", w->pressure);
                    continue;

                case 'c':
                    cl += snprintf(dest_str + cl, n - cl, "%s", w->condition);
                    continue;

                case 'C':
                    cl += snprintf(dest_str + cl, n - cl, "%d", 
                                   w->weather_code);
                    continue;

                case 'd':
                    cl += snprintf(dest_str + cl, n - cl, "%g", w->clouds);
                    continue;

                case 'h':
                    cl += snprintf(dest_str + cl, n - cl, "%g", w->temp_min);
                    continue;

                case 'H':
                    cl += snprintf(dest_str + cl, n - cl, "%g", w->temp_max);
                    continue;

                case 'i':
                    cl += snprintf(dest_str + cl, n - cl, "%ld", w->id);
                    continue;

                case 'I':
                    cl += snprintf(dest_str + cl, n - cl, "%s", w->name);
                    continue;

                case 'j':
                    cl += snprintf(dest_str + cl, n - cl, "%s", w->country);
                    continue;

                case 'l':
                    cl += snprintf(dest_str + cl, n - cl, "%g", w->latitude);
                    continue;

                case 'L':
                    cl += snprintf(dest_str + cl, n - cl, "%g", w->longitude);
                    continue;

                case 'p':
                    cl += snprintf(dest_str + cl, n - cl, "%g", 
                                   w->precipitation_3h);
                    continue;

                case 'P':
                    cl += snprintf(dest_str + cl, n - cl, "%g", w->humidity);
                    continue;

                case 's':
                    cl += snprintf(dest_str + cl, n - cl, "%ld", w->sunrise);
                    continue;

                case 'S':
                    cl += snprintf(dest_str + cl, n - cl, "%ld", w->sunset);
                    continue;

                case 't':
                    cl += snprintf(dest_str + cl, n - cl, "%g", w->temperature);
                    continue;

                //case 'T':
                    // cl += snprintf(dest_str + cl, n - cl, "%g", w->au);
                    //continue;

                case 'w':
                    cl += snprintf(dest_str + cl, n - cl, "%g", w->wind_speed);
                    continue;

                case 'W':
                    cl += snprintf(dest_str + cl, n - cl, "%g", w->wind_gust);
                    continue;

                case 'x':
                    cl += snprintf(dest_str + cl, n - cl, "%g", 
                                   w->wind_direction);
                    continue;

                case 'X': {
                    int dir = (int )round(w->wind_direction / (360 / 32)) % 32;
                    cl += snprintf(dest_str + cl, n - cl, "%s", c_dir[dir]);
                    continue;
                }

                case '%':
                    cl += snprintf(dest_str + cl, n - cl, "%c", *format);
                    continue;

                default:
                    break;
            }
        } else {
            cl += snprintf(dest_str + cl, n - cl, "%c", *format);
        }
    }
    
    return (cl == n ? 0 : cl);
}

// vim: set ts=4 sw=4 et:
