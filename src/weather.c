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
                            fetched_weather.clouds = json_number_value(sub_value);
                        } else if ( root_key[2] == 'o' ) {
                            if ( sub_key[1] == 'o' ) {
                                fetched_weather.longitude = json_number_value(sub_value);
                            } else if ( sub_key[1] == 'a' ) {
                                fetched_weather.latitude = json_number_value(sub_value);
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
                if ( json_typeof(root_value) == (int )JSON_OBJECT ) {
                    const char * sub_key;
                    json_t * sub_value;
                    json_object_foreach(root_value, sub_key, sub_value) {
                        switch ( sub_key[0] ) {
                            case 't':
                                if ( strlen(sub_key) == 4 ) {
                                    fetched_weather.temperature = json_number_value(sub_value);
                                } else if ( sub_key[6] == 'i' ) {
                                    fetched_weather.temp_min = json_number_value(sub_value);
                                } else if ( sub_key[6] == 'a' ) {
                                    fetched_weather.temp_max = json_number_value(sub_value);
                                } break;
    
                            case 'p':
                                fetched_weather.pressure = json_number_value(sub_value);
                                break;
    
                            case 'h':
                                fetched_weather.humidity = json_number_value(sub_value);
                                break;
                        }
                    }
                } break;

            case 'n':
                fetched_weather.name = strdup(json_string_value(root_value));
                break;

            case 's':
                if ( json_typeof(root_value) == (int )JSON_OBJECT ) {
                    const char * sub_key;
                    json_t * sub_value;
                    json_object_foreach(root_value, sub_key, sub_value) {
                        switch ( root_key[1] ) {
                            case 'y':
                                switch ( sub_key[4] ) {
                                    case 'i':
                                        fetched_weather.sunrise = json_integer_value(sub_value);
                                        break;

                                    case 'e':
                                        fetched_weather.sunset = json_integer_value(sub_value);
                                        break;

                                    case 't':
                                        fetched_weather.country = strdup(json_string_value(sub_value));
                                        break;
                                } break;
    
                            case 'n':
                                fetched_weather.precipitation_3h = json_number_value(sub_value);
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
                                fetched_weather.weather_code = json_integer_value(sub_value);
                                break;

                            case 'e':
                                fetched_weather.condition = strdup(json_string_value(sub_value));
                                break;
                        }
                    }
                } else if ( json_typeof(root_value) == (int )JSON_OBJECT ) {
                    const char * sub_key;
                    json_t * sub_value;
                    json_object_foreach(root_value, sub_key, sub_value) {
                        switch ( sub_key[0] ) {
                            case 's':
                                fetched_weather.wind_speed = json_number_value(sub_value);
                                break;
    
                            case 'g':
                                fetched_weather.wind_gust = json_number_value(sub_value);
                                break;
    
                            case 'd':
                                fetched_weather.wind_direction = json_integer_value(sub_value);
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
                            fetched_weather.precipitation_3h = json_number_value(sub_value);
                        }
                    }
                } break;
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
                    snprintf(dest_str + current_length, n - current_length, "%ld", weather->dt);
                    continue;

                case 'b':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->pressure);
                    continue;

                case 'c':
                    snprintf(dest_str + current_length, n - current_length, "%s", weather->condition);
                    continue;

                case 'C':
                    snprintf(dest_str + current_length, n - current_length, "%d", weather->weather_code);
                    continue;

                case 'd':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->clouds);
                    continue;

                case 'h':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->temp_min);
                    continue;

                case 'H':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->temp_max);
                    continue;

                case 'i':
                    snprintf(dest_str + current_length, n - current_length, "%ld", weather->id);
                    continue;

                case 'I':
                    snprintf(dest_str + current_length, n - current_length, "%s", weather->name);
                    continue;

                case 'j':
                    snprintf(dest_str + current_length, n - current_length, "%s", weather->country);
                    continue;

                case 'l':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->latitude);
                    continue;

                case 'L':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->longitude);
                    continue;

                case 'p':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->precipitation_3h);
                    continue;

                case 'P':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->humidity);
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
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->wind_speed);
                    continue;

                case 'W':
                    snprintf(dest_str + current_length, n - current_length, "%g", weather->wind_gust);
                    continue;

                case 'x':
                    snprintf(dest_str + current_length, n - current_length, "%d", weather->wind_direction);
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

    int bytes_written = strlen(dest_str);

    return (bytes_written == n ? 0 : bytes_written);
}

// vim: set ts=4 sw=4 et:
