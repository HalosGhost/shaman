/****************************************\
* Fetch weather information from the NWS *
* Author: 2012-2013 (C) Sam Stuewe       *
* License: GPLv2                         *
\****************************************/

#include "shaman.h"

// Main Function //
int main (int argc, char ** argv)
{   static int flagHelp;
    static int flagMetric;

    if ( argc > 1 )
    {   int c;
        
        while ( 1 )
        {   static struct option longOptions[] =
            {   /* Flags */
                {"help",      no_argument,         0,    'h'   },
                {"imperial",  no_argument,         0,    'i'   },
                {"metric",    no_argument,         0,    'm'   },
                /* Switches */
                {"format",    required_argument,   0,    'f'   },
                {"location",  required_argument,   0,    'l'   },
                {0,           0,                   0,    0     },
            };

            int optionIndex = 0;

            c = getopt_long(argc, argv, "hif:l:m", longOptions, &optionIndex);

            if ( c == -1 ) break;

            switch (c)
            {   case 'h':
                    flagHelp = 1;
                    break;

                case 'i':
                    flagMetric = 0;
                    break;

                case 'f':
                    snprintf(formatString, sizeof(formatString), optarg);
                    break;

                case 'l':
                    snprintf(location, sizeof(location), optarg);
                    break;

                case 'm':
                    flagMetric = 1;
                    break;
            }
        }
    }

    if ( flagHelp == 1 ) _usage();

    if ( *location )
    {   char * end;
        const long sl = strtol(location, &end, 10);

        if ( end == location )
        {   char * pass = location;
            char * buffer = _malloc(strlen(location) * 3 + 1);
            char * buff = buffer;

            while ( *pass )
            {   if ( isalnum(*pass) ) *buff++ = *pass;
                else
                {   switch (*pass)
                    {   case '-':
                        case '.':
                        case ',':
                            *buff++ = *pass;
                            break;

                        case ' ':
                            *buff++ = '+';
                            break;
                    }
                }
                pass++;
            }

            *buff = '\0';
            _getData(buffer, flagMetric);
            free(buffer);
        }
        else if ( sl > 99999 || sl < 00000 )
        {   fprintf(stderr, "Invalid zip code: %ld\n", sl);
            exit(1);
        }
        else _getData((char * )location, flagMetric);
    }
    else
    {   fprintf(stderr, "No specified location\n");
        exit(1);
    }

    // Call other functions for parsing

    return 0;
}

// Data and Analysis Functions //
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
    {   snprintf(url, sizeof(url), "http://forecast.weather.gov/zipcity.php?inputstring=%s", location);
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

    if ( *formatString ) _formatOutput(formatString);
    else
    {   fprintf(stderr, "No specified format\n");
        exit(1);
    }
}

size_t _writeDataToBuffer (char * ptr, size_t size, size_t nmemb, void * userdata)
{   data_t * data = (data_t * )userdata;
    size_t s = size * nmemb;

    data->size += s;
    data->buffer = _realloc(data->buffer, data->size + 1);
    data->buffer[data->size] = '\0';

    if ( ptr )
    {   memcpy(data->buffer + data->offset, ptr, s);
        data->offset += s;
    }

    return s;
}

void _parseDataInBuffer (const char * buffer, int buffer_size)
{   xmlDocPtr weather;
    xmlNodePtr cur;

    weather = xmlParseMemory(buffer, buffer_size);

    if ( !weather )
    {   fprintf(stderr, "Failed to parse weather data\n");
        exit(1);
    }

    cur = xmlDocGetRootElement(weather);

    if ( !cur )
    {   fprintf(stderr, "Weather file is empty\n");
        xmlFreeDoc(weather);
        exit(1);
    }

    if ( xmlStrcmp((const xmlChar * )cur->name, (const xmlChar * )"dwml") )
    {   fprintf(stderr, "Fetched wrong file\nNode named \"%s\"", cur->name);
        xmlFreeDoc(weather);
        exit(1);
    }

    cur = cur->xmlChildrenNode;

    while ( cur )
    {   if ( !xmlStrcmp((const xmlChar * )cur->name, (const xmlChar * )"head") ) 
        {   _parseData(weather, cur);
        }

        cur = cur->next;
    }

    xmlFreeDoc(weather);
}

void _parseData (xmlDocPtr weather, xmlNodePtr cur)
{   xmlChar * datum;
    cur = cur->xmlChildrenNode;

    while ( cur )
    {   if ( !xmlStrcmp((const xmlChar * )cur->name, (const xmlChar * )"description") )
        {   datum = xmlNodeListGetString(weather, cur->xmlChildrenNode, 1);
            printf("datum: %s\n", datum);
            xmlFree(datum);
        }
        cur = cur->next;
    }
}

void _parseFormat (char * formatStr)
{   for ( ; *formatStr; ++formatStr)
    {   if ( *formatStr == '%' )
        {   if ( *formatStr == '0' ) formatStr++;

            switch (*++formatStr)
            {   case '\0':
                    --formatStr;
                    break;

                case 'c':
                    // condition
                    continue;

                case 'd':
                    // humidity
                    continue;

                case 'D':
                    // dew point
                    continue;

                case 'H':
                    // hazard warnings
                    continue;

                case 'p':
                    // pressure
                    continue;

                case 'P':
                    // probability of precipitation
                    continue;

                case 'r':
                    // reporter identity
                    continue;

                case 'R':
                    // reporter coordinates
                    continue;

                case 't':
                    // temperature
                    continue;

                case 'T':
                    // apparent temperature
                    continue;

                case 'v':
                    // visibility
                    continue;

                case 'w':
                    // wind speed
                    continue;

                case 'W':
                    // wind direction
                    continue;

                case '%':
                default:
                    break;
            }
        }
    }
}

void _formatOutput (char * formatStr)
{   for ( ; *formatStr; ++formatStr)
    {   if ( *formatStr == '%' )
        {   if ( *formatStr == '0' ) formatStr++;

            switch (*++formatStr)
            {   case '\0':
                    --formatStr;
                    break;

                case 'c':
                    // condition
                    continue;

                case 'd':
                    // humidity
                    continue;

                case 'D':
                    // dew point
                    continue;

                case 'H':
                    // hazard warnings
                    continue;

                case 'p':
                    // pressure
                    continue;

                case 'P':
                    // probability of precipitation
                    continue;

                case 'r':
                    // reporter identity
                    continue;

                case 'R':
                    // reporter coordinates
                    continue;

                case 't':
                    // temperature
                    continue;

                case 'T':
                    // apparent temperature
                    continue;

                case 'v':
                    // visibility
                    continue;

                case 'w':
                    // wind speed
                    continue;

                case 'W':
                    // wind direction
                    continue;

                case '%':
                default:
                    break;
            }
        }
    }
}
// vim:set ts=4 sw=4 et:
