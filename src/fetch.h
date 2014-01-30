#ifndef __FETCH_H__
#define __FETCH_H__

// Includes //
#include <curl/curl.h>
#include "parse.h"
#include "format.h"

// Variables //
#define BUFFER_SIZE 80

char formatString[BUFFER_SIZE] = {'\0'};
char location[BUFFER_SIZE] = {'\0'};

// Prototypes //
static void _getData (const char * location, const int scale);

// Fetch DWML //
void _getData (const char * location, const int scale)
{   CURL * handle;
    CURLcode res;
    char url[256] = "";
    FILE * suppressOutput = fopen("/dev/null", "wb");

    data_t data;
    memset(&data, '\0', sizeof(data_t));

    curl_global_init(CURL_GLOBAL_ALL);
    handle = curl_easy_init();

    if ( handle )
    {   static const char * provider = "http://forecast.weather.gov/zipcity.php?inputstring=";
        snprintf(url, sizeof(url), "%s%s", provider, location);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, suppressOutput);
        curl_easy_setopt(handle, CURLOPT_URL, url);
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);

        res = curl_easy_perform(handle);
        if ( res == CURLE_OK )
        {   char * interim;
            res = curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &interim);

            snprintf(url, sizeof(url), "%s&FcstType=dwml&unit=%d", interim, scale);

            if ( res == CURLE_OK )
            {   curl_easy_setopt(handle, CURLOPT_URL, url);
                curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, _writeDataToBuffer);
                curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void * )&data);
                res = curl_easy_perform(handle);
            }
            else
            {   fprintf(stderr, "Failed to get weather page (%s)\n", curl_easy_strerror(res));
                fclose(suppressOutput);
                curl_easy_cleanup(handle);
                curl_global_cleanup();
                free(data.buffer);
                exit(1);
            }
        }
        else
        {   fprintf(stderr, "Failed to get location (%s)\n", curl_easy_strerror(res));
            fclose(suppressOutput);
            curl_easy_cleanup(handle);
            curl_global_cleanup();
            free(data.buffer);
            exit(1);
        }
    }

    fclose(suppressOutput);
    curl_easy_cleanup(handle);
    curl_global_cleanup();

    //puts(data.buffer);

    _parseDataInBuffer(data.buffer, data.size);

    free(data.buffer);
}

#endif // __FETCH_H__
