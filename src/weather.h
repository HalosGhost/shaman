#ifndef WEATHER_H
#define WEATHER_H

#ifdef __cplusplus
extern "C" {
#endif

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
#include <stddef.h>

// Forward Declarations //
#define BUFFER_SIZE 2048 // 2KiB

struct json_write_result {
    char * data;
    unsigned long position;
};

struct weather {
    long dt;                  // %a  time data received
    double pressure;          // %b  barometric pressure
    const char * condition;   // %c  weather conditions
    double clouds;            // %d  cloud coverage
    double temp_min;          // %h  low temperature
    double temp_max;          // %H  high temperature
    long long id;             // %i  city id number
    const char * name;        // %I  city name
    const char * country;     // %j  country code
    double latitude;          // %l  latitude
    double longitude;         // %L  longitude
    double precipitation_3h;  // %p  precipitation
    double humidity;          // %P  relative humidity
    long sunrise;             // %s  time of sunrise
    long sunset;              // %S  time of sunset
    double temperature;       // %t  current temperature
    //double au;                // %T  apparent temperature
    double wind_speed;        // %w  sustained wind speed
    double wind_gust;         // %W  gusting wind speed
    double wind_direction;    // %x  wind heading
    long long weather_code;   // %C  weather condition code
};

extern struct json_write_result *
owm_fetch_local (const char *);

extern struct json_write_result *
owm_fetch_remote (const char, const char *, const char, const char *,
                  const char *);

extern struct weather *
owm_read (struct json_write_result *);

extern struct weather *
owm_easy (const char, const char *, const char, const char *, const unsigned,
          const char *, const char);

extern size_t
strfweather (char *, size_t, const char *, const struct weather *);

#ifdef __cplusplus
}
#endif

#endif // WEATHER_H
// vim: set ts=4 sw=4 et:
