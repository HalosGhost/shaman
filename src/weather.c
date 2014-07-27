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

// Function drawn from Petri Lehtinen's GitHub Jansson example
static
size_t
write_data_buffer (void * ptr, size_t size, size_t nmemb, void * stream) {

    struct json_write_result * result = (struct json_write_result * )stream;
    if ( result->position + size * nmemb >= BUFFER_SIZE - 1 ) {
        fprintf(stderr, "Write Buffer is too small\n");
        return 0;
    }

    memcpy(result->data + result->position, ptr, size * nmemb);
    result->position += size * nmemb;

    return size * nmemb;
}

// Fetch JSON from file
struct json_write_result *
owm_fetch_local (const char * json_file_path) {

    char * data = calloc(1, BUFFER_SIZE);
    static struct json_write_result written_result;
    written_result.data = data;
    written_result.position = 0;

    FILE * json_fp = fopen(json_file_path, "r");

    size_t bytes_read = fread(written_result.data, BUFFER_SIZE, 1, json_fp);

    if ( ferror(json_fp) ) {
        fclose(json_fp);
        fprintf(stderr, "Error reading file\n");
        exit(1);
    }

    fclose(json_fp);

    return &written_result;
}

// Fetch JSON from OpenWeatherMap
struct json_write_result *
owm_fetch_remote (const char method, const char * location, const char scale,
                  const char * file_cache_path, const char * api_key) {

    CURL * handle;
    CURLcode result;

    char * data = calloc(1, BUFFER_SIZE);
    static struct json_write_result written_result;
    written_result.data = data;
    written_result.position = 0;

    char url [256] = {'\0'};
    char * fetch_method;
    char * fetch_scale;

    switch ( method ) {
        case 'i':
            fetch_method = "id";
            break;

        default:
            fetch_method = "q";
            break;
    }

    switch ( scale ) {
        case 'i':
            fetch_scale = "imperial";
            break;

        default:
            fetch_scale = "metric";
            break;
    }

    curl_global_init(CURL_GLOBAL_ALL);
    handle = curl_easy_init();

    if ( handle ) {
        const char * provider =
            "http://api.openweathermap.org/data/2.5/weather";

        char * encoded_location = curl_easy_escape(handle, location, 0);

        snprintf(url, sizeof(url), "%s?%s=%s&units=%s&APPID=%s", provider,
                 fetch_method, encoded_location, fetch_scale, api_key);

        curl_easy_setopt(handle, CURLOPT_URL, url);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data_buffer);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &written_result);

        result = curl_easy_perform(handle);
        if ( result != CURLE_OK ) {
            fprintf(stderr, "Failed to retrieve data (%s)\n",
                    curl_easy_strerror(result));

            curl_easy_cleanup(handle);
            curl_global_cleanup();
            curl_free(encoded_location);
            exit(1);
        }

        curl_free(encoded_location);
    }

    int bytes_written;
    if ( file_cache_path ) {
        FILE * json_cache = fopen(file_cache_path, "w");
        bytes_written = fwrite(written_result.data, BUFFER_SIZE, 1, json_cache);

        if ( ferror(json_cache) ) {
            fclose(json_cache);
            fprintf(stderr, "Error caching file\n");
            exit(1);
        }

        fclose(json_cache);
    }

    curl_easy_cleanup(handle);
    curl_global_cleanup();

    return &written_result;
}

// Read JSON into a weather struct
struct weather *
owm_read (struct json_write_result * json) {

    json_error_t error;
    json_t * root = json_loads(json->data, 0, &error);
    free(json->data);

    if ( !root ) {
        fprintf(stderr, "Error on line %d (%s)\n", error.line, error.text);
        return NULL;
    }

    static struct weather fetched_weather;

    const char * root_key;
    json_t * root_value;
    json_object_foreach(root, root_key, root_value) {
        switch ( root_key[0] ) {
            case 'c':
                if ( json_typeof(root_value) == (int )JSON_OBJECT ) {
                    const char * sub_key;
                    json_t * sub_value;
                    json_object_foreach(root_value, sub_key, sub_value) {
                        if ( root_key[1] == 'l' ) {
                            fetched_weather.clouds =
                                json_number_value(sub_value);
                        } else if ( root_key[2] == 'o' ) {
                            if ( sub_key[1] == 'o' ) {
                                fetched_weather.longitude =
                                    json_number_value(sub_value);
                            } else if ( sub_key[1] == 'a' ) {
                                fetched_weather.latitude =
                                    json_number_value(sub_value);
                            }
                        }
                    }
                } break;

            case 'd':
                fetched_weather.dt = json_integer_value(root_value);
                break;

            case 'i':
                fetched_weather.id = json_integer_value(root_value);
                break;

            case 'm':
                if ( root_key[1] == 'e' ) {
                    fprintf(stderr, "%s\n", json_string_value(root_value));
                    free(root);
                    exit(1);
                } else if ( json_typeof(root_value) == (int )JSON_OBJECT ) {
                    const char * sub_key;
                    json_t * sub_value;
                    json_object_foreach(root_value, sub_key, sub_value) {
                        switch ( sub_key[0] ) {
                            case 't':
                                if ( strlen(sub_key) == 4 ) {
                                    fetched_weather.temperature =
                                        json_number_value(sub_value);
                                } else if ( sub_key[6] == 'i' ) {
                                    fetched_weather.temp_min =
                                        json_number_value(sub_value);
                                } else if ( sub_key[6] == 'a' ) {
                                    fetched_weather.temp_max =
                                        json_number_value(sub_value);
                                } break;

                            case 'p':
                                fetched_weather.pressure =
                                    json_number_value(sub_value);
                                break;

                            case 'h':
                                fetched_weather.humidity =
                                    json_number_value(sub_value);
                                break;
                        }
                    }
                } break;

            case 'n':
                fetched_weather.name = json_string_value(root_value);
                if ( !fetched_weather.name ) {
                    fetched_weather.name = "Unavailable";
                } break;

            case 's':
                if ( json_typeof(root_value) == (int )JSON_OBJECT ) {
                    const char * sub_key;
                    json_t * sub_value;
                    json_object_foreach(root_value, sub_key, sub_value) {
                        switch ( root_key[1] ) {
                            case 'y':
                                switch ( sub_key[4] ) {
                                    case 'i':
                                        fetched_weather.sunrise =
                                            json_integer_value(sub_value);
                                        break;

                                    case 'e':
                                        fetched_weather.sunset =
                                            json_integer_value(sub_value);
                                        break;

                                    case 't':
                                        fetched_weather.country =
                                            json_string_value(sub_value);

                                        if ( !fetched_weather.country ) {
                                            fetched_weather.country =
                                                "Unavailable";
                                        } break;
                                } break;

                            case 'n':
                                fetched_weather.precipitation_3h =
                                    json_number_value(sub_value);
                                break;
                        }
                    }
                } break;

            case 'w':
                if ( json_typeof(root_value) == (int )JSON_ARRAY ) {
                    json_t * weather = json_array_get(root_value, 0);
                    const char * sub_key;
                    json_t * sub_value;
                    json_object_foreach(weather, sub_key, sub_value) {
                        switch ( sub_key[1] ) {
                            case 'd':
                                fetched_weather.weather_code =
                                    json_integer_value(sub_value);
                                break;

                            case 'e':
                                fetched_weather.condition =
                                    json_string_value(sub_value);

                                if ( !fetched_weather.condition ) {
                                    fetched_weather.condition = "Unavailable";
                                } break;
                        }
                    }
                } else if ( json_typeof(root_value) == (int )JSON_OBJECT ) {
                    const char * sub_key;
                    json_t * sub_value;
                    json_object_foreach(root_value, sub_key, sub_value) {
                        switch ( sub_key[0] ) {
                            case 's':
                                fetched_weather.wind_speed =
                                    json_number_value(sub_value);
                                break;

                            case 'g':
                                fetched_weather.wind_gust =
                                    json_number_value(sub_value);
                                break;

                            case 'd':
                                fetched_weather.wind_direction =
                                    json_number_value(sub_value);
                                break;
                        }
                    }
                } break;

            case 'r':
                if ( json_typeof(root_value) == (int )JSON_OBJECT ) {
                    const char * sub_key;
                    json_t * sub_value;
                    json_object_foreach(root_value, sub_key, sub_value) {
                        if ( sub_key[0] == '3' ) {
                            fetched_weather.precipitation_3h =
                                json_number_value(sub_value);
                        }
                    }
                } break;
        }
    }

    free(root);

    return &fetched_weather;
}

int
check_if_stale (const char * cache_path, const unsigned int cache_update_time,
                const char verbosity) {

    struct stat cache_stats;
    int status = stat(cache_path, &cache_stats);
    int errsv = errno;

    if ( status == 0 ) {
        return ((time(NULL) - cache_stats.st_mtim.tv_sec) >= cache_update_time);
    } else {
        if ( verbosity > 0 ) {
            fprintf(stderr,
                    "Determining if cache should be freshened failed: %s\n",
                    strerror(errsv));
        } return -1;
    }
}

struct weather *
owm_easy (const char method, const char * location, const char scale,
          const char * file_cache_path, const unsigned int cache_update_time,
          const char * api_key, const char verbosity) {

    struct json_write_result * json;
    struct weather * wthr;

    if ( file_cache_path ) {
        if ( check_if_stale(file_cache_path, cache_update_time, verbosity) ) {
            json = owm_fetch_remote(method, location, scale, file_cache_path,
                                    api_key);
        } else {
            json = owm_fetch_local(file_cache_path);
            char * city = malloc(strlen(location) - 3);
            sscanf(location, "%[^,]", city);
            if ( !strstr(json->data, city) ) {
                free(json->data);
                json = owm_fetch_remote(method, location, scale,
                                        file_cache_path, api_key);
            } free(city);
        }
    } else {
        json = owm_fetch_remote(method, location, scale, NULL, api_key);
    }

    wthr = owm_read(json);
    return wthr;
}

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
                    cl += strftime(dest_str + cl, n - cl, "%H:%M:%S", localtime(&w->dt));
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
                    cl += strftime(dest_str + cl, n - cl, "%H:%M:%S", localtime(&w->sunrise));
                    continue;

                case 'S':
                    cl += strftime(dest_str + cl, n - cl, "%H:%M:%S", localtime(&w->sunset));
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
