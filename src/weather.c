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
#include "weather.h"

// Function drawn from Petri Lehtinen's GitHub Jansson example
static size_t write_data_buffer (void * ptr, size_t size, size_t nmemb, void * stream) {

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
struct json_write_result * fetch_data_file (char * json_file_path) {
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
struct json_write_result * fetch_data_owm (const char method, const char * location, const char scale, const char * file_cache_path) {
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
        const char * provider = "http://api.openweathermap.org/data/2.5/weather";
        char * encoded_location = curl_easy_escape(handle, location, 0);
        
        snprintf(url, sizeof(url), "%s?%s=%s&units=%s", provider, fetch_method, encoded_location, fetch_scale);

        curl_easy_setopt(handle, CURLOPT_URL, url);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data_buffer);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &written_result);

        result = curl_easy_perform(handle);
        if ( result != CURLE_OK ) {
            fprintf(stderr, "Failed to retrieve data (%s)\n", curl_easy_strerror(result));

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
struct weather * read_weather (struct json_write_result * json) {
    json_error_t error;
    json_t * root = json_loads(json->data, 0, &error);
    free(json->data);

    if ( !root ) {
        fprintf(stderr, "Error on line %d (%s)\n", error.line, error.text);
        return NULL;
    }

    static struct weather fetched_weather;

    const char * key;
    json_t * value;
    json_object_foreach(root, key, value) {
        switch ( key[0] ) {
            case 'c':
                if ( key[1] == 'l' ) {
                    fetched_weather.clouds = json_number_value(json_object_get(value, "all"));
                } else if ( key[2] == 'o' ) {
                    fetched_weather.longitude = json_number_value(json_object_get(value, "lon"));
                    fetched_weather.latitude = json_number_value(json_object_get(value, "lat"));
                } break;

            case 'd':
                fetched_weather.dt = json_integer_value(value);
                break;

            case 'i':
                fetched_weather.id = json_integer_value(value);
                break;

            case 'm':
                fetched_weather.temperature = json_number_value(json_object_get(value, "temp"));
                fetched_weather.pressure = json_number_value(json_object_get(value, "pressure"));
                fetched_weather.temp_min = json_number_value(json_object_get(value, "temp_min"));
                fetched_weather.temp_max = json_number_value(json_object_get(value, "temp_max"));
                fetched_weather.humidity = json_number_value(json_object_get(value, "humidity"));
                break;

            case 'n': {
                int city_name_length = strlen(json_string_value(value)) + 1;
                fetched_weather.name = malloc(city_name_length);
                snprintf(fetched_weather.name, city_name_length, "%s", json_string_value(value));
                break;
            }

            case 's':
                if ( key[1] == 'y' ) {
                    fetched_weather.sunrise = json_integer_value(json_object_get(value, "sunrise"));
                    fetched_weather.sunset = json_integer_value(json_object_get(value, "sunset"));
                    int country_name_length = strlen(json_string_value(json_object_get(value, "country"))) + 1;
                    fetched_weather.country = malloc(country_name_length);
                    snprintf(fetched_weather.country, country_name_length, "%s", json_string_value(json_object_get(value, "country")));
                } else if ( key[1] == 'n' ) {
                    fetched_weather.precipitation_3h = json_number_value(json_object_get(value, "3h"));
                } break;

            case 'w':
                if ( key[1] == 'e' ) {
                    fetched_weather.weather_code = json_integer_value(json_object_get(json_array_get(value, 0), "id"));
                    int condition_length = strlen(json_string_value(json_object_get(json_array_get(value, 0), "description"))) + 1;
                    fetched_weather.condition = malloc(condition_length);
                    snprintf(fetched_weather.condition, condition_length, "%s", json_string_value(json_object_get(json_array_get(value, 0), "description")));
                } else if ( key[1] == 'i' ) {
                    fetched_weather.wind_speed = json_number_value(json_object_get(value, "speed"));
                    fetched_weather.wind_gust = json_number_value(json_object_get(value, "gust"));
                    fetched_weather.wind_direction = json_integer_value(json_object_get(value, "deg"));
                } break;

            case 'r':
                fetched_weather.precipitation_3h = json_number_value(json_object_get(value, "3h"));
                break;
        }
    }

    return &fetched_weather;
}

// TODO: Add support for formatting time variables
// TODO: Add support for Apparent Temperature
size_t strfweather (char * dest_str, size_t n, const char * format, const struct weather * weather) {
    for ( ; *format; ++format) {
        int current_length = strlen(dest_str);

        if ( *format == '%' ) {
            switch ( *++format ) {
                case '\0':
                    --format;
                    break;

                case 'a':
                    snprintf(dest_str + current_length, n - current_length, "%s", weather->condition);
                    continue;

                case 'c':
                    snprintf(dest_str + current_length, n - current_length, "%s, %s", weather->name, weather->country);
                    continue;

                case 'C':
                    snprintf(dest_str + current_length, n - current_length, "%ld", weather->id);
                    continue;

                case 'd':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->clouds);
                    continue;

                case 'D':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->humidity);
                    continue;

                case 'g':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->wind_gust);
                    continue;

                case 'G':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->wind_speed);
                    continue;

                case 'l':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->latitude);
                    continue;

                case 'L':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->longitude);
                    continue;

                case 'm':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->temp_min);
                    continue;

                case 'M':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->temp_max);
                    continue;

                case 'p':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->pressure);
                    continue;

                case 'P':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->precipitation_3h);
                    continue;

                case 'r':
                    snprintf(dest_str + current_length, n - current_length, "%ld", weather->dt);
                    continue;

                case 's':
                    snprintf(dest_str + current_length, n - current_length, "%ld", weather->sunrise);
                    continue;

                case 'S':
                    snprintf(dest_str + current_length, n - current_length, "%ld", weather->sunset);
                    continue;

                case 't':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->temperature);
                    continue;

                case 'T':
                    // AU / "Feels Like"
                    continue;

                case 'w':
                    snprintf(dest_str + current_length, n - current_length, "%d", weather->wind_direction);
                    continue;

                case 'W':
                    snprintf(dest_str + current_length, n - current_length, "%d", weather->weather_code);
                    continue;

                case '%':
                default:
                    break;
            }
        } else {
            snprintf(dest_str + current_length, n - current_length, "%c", *format);
        }
    }
    
    free(weather->name);
    free(weather->country);
    free(weather->condition);

    return strlen(dest_str);
}

// vim: set ts=4 sw=4 et:
