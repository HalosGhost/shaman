/****************************************\
* Fetch weather information from the NWS *
* Author: 2012-2013 (C) Sam Stuewe       *
* License: GPLv2                         *
\****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <curl/curl.h>
#include <getopt.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define BUFFER_SIZE 80

// Variables //
char formatString[BUFFER_SIZE] = {'\0'};
char passLoc[BUFFER_SIZE] = {'\0'};

typedef struct
{   char * buffer;
	size_t size;
	size_t offset;
}data_t;

// Prototypes //
static void * _realloc(void * ptr, size_t size);
static void * _malloc(size_t size);
static void * _memoryAbort(void);
static void _usage(void);
static void _getData(const char * location, const int scale);
static size_t _writeDataToBuffer(char * ptr, size_t size, size_t nmemb,
		void * userdata);
static void _parseDataInBuffer(const char * buffer, int buffer_size);
static void _parseData(xmlDocPtr weather, xmlNodePtr cur);
static void _formatOutput(char * formatStr);

// Main Function //
int main(int argc, char ** argv)
{   static int flag_help;
	static int flag_metric;

	if ( argc > 1 )
	{   int c;

		while (1)
		{	static struct option long_options[] =
			{   // Long Option Flags //
				{"help",	  no_argument,		   0,	 'h'   },
				{"imperial",  no_argument,		   0,	 'i'   },
				{"metric",	  no_argument,		   0,	 'm'   },
				// Long Option Switches //
				{"format",	  required_argument,   0,	 'f'   },
				{"location",  required_argument,   0,	 'l'   },
				{0,			  0,				   0,	 0	   },
			};

			int option_index = 0; // Stores the option index

			c = getopt_long(argc, argv, "hif:l:m", long_options, &option_index);

			if ( c == -1 ) break;

			switch (c)
			{   case 'h':
				    flag_help = 1;
					break;

			    case 'i':
					flag_metric = 0;
					break;

			    case 'f':
					snprintf(formatString, sizeof(formatString), "%s", optarg);
					break;

			    case 'l':
					snprintf(passLoc, sizeof(passLoc), "%s", optarg);
					break;

			    case 'm':
					flag_metric = 1;
					break;
			}
		}
	}

	if ( flag_help == 1 ) _usage();

	if ( *passLoc ) // Inspiration: http://www.geekhideout.com/urlcode.shtml
    {   char * end;
		const long sl = strtol(passLoc, &end, 10);

		if ( end == passLoc )
	    {	char * pass = passLoc;
			char * buffer = _malloc(strlen(passLoc) * 3 + 1);
			char * buff = buffer;

			while ( *pass )
			{   if ( isalnum(*pass) ||\
					 *pass == '-' ||\
					 *pass == '.' ||\
					 *pass == ',' ) *buff++ = *pass;

				else if ( *pass == ' ' ) *buff++ = '+';
				pass++;
			}

			*buff = '\0';
			_getData(buffer, flag_metric);
			free(buffer);
		}
		else if ( sl > 99999 || sl < 00000 )
	    {	fprintf(stderr, "Invalid zip code: %ld\n", sl);
			exit(1);
		}
		else _getData((char * )passLoc, flag_metric);
	}
	else
    {   fprintf(stderr, "No specified location\n");
		exit(1);
	}

	// Call other functions for parsing

	return 0;
}

// Memory Safety //
void * _realloc(void * ptr, size_t size)
{   void * e = realloc(ptr, size);
	if ( e ) return e;
	else return _memoryAbort();
}

void * _malloc(size_t size)
{   void * e = malloc(size);
	if ( e ) return e;
	else return _memoryAbort();
}

void * _memoryAbort(void)
{	puts("Failed to allocate memory");
	exit(1);
	return NULL;
}

// Usage //
void _usage(void)
{   fputs("\
Usage: shaman [options]\n\n\
Options:\n\
  -h, --help                    print help and exit\n\
  -i, --imperial                use Imperial units (default)\n\
  -m, --metric                  use Metric units\n\
  -f, --format=\"FORMAT\"         format output according to \"FORMAT\"\n\
  -l, --location=\"location\"     print weather information for \"location\"\n\n\
See `man 1 shaman` for more information\n", stderr);
	exit(0);
}

// Data and Analysis Functions //
void _getData(const char * location, const int scale)
{   CURL * handle;
	CURLcode res;
	char url[256] = "";
	FILE * suppressOutput = fopen("/dev/null", "wb");

	data_t data;
	memset(&data, '\0', sizeof(data_t));

	curl_global_init(CURL_GLOBAL_ALL);
	handle = curl_easy_init();

	if (handle)
    {   snprintf(url, sizeof(url), "http://forecast.weather.gov/zipcity.php?inputstring=%s", location);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, suppressOutput);
		curl_easy_setopt(handle, CURLOPT_URL, url);
		curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);

		res = curl_easy_perform(handle);
		if ( res == CURLE_OK )
	    {	char * interim;
			res = curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &interim);

			if ( scale == 1 ) snprintf(url, sizeof(url), "%s&FcstType=dwml&unit=1", interim);
			else snprintf(url, sizeof(url), "%s&FcstType=dwml&unit=0", interim);

			if ( res == CURLE_OK )
			{   curl_easy_setopt(handle, CURLOPT_URL, url);
				curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, _writeDataToBuffer);
				curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void * )&data);
				res = curl_easy_perform(handle);
			}
			else
			{   fprintf(stderr, "Failed to get weather page (%s)\n", curl_easy_strerror(res));
			}
		}
		else
	    {	fprintf(stderr, "Failed to get location (%s)\n", curl_easy_strerror(res));
		}
	}

	fclose(suppressOutput);
	curl_easy_cleanup(handle);
	curl_global_cleanup();

	// Uncomment the following to see the fetched file
	//puts(data.buffer);

	_parseDataInBuffer(data.buffer, data.size);

	free(data.buffer);

	if ( *formatString ) _formatOutput(formatString);
	else
    {   fprintf(stderr, "No specified format\n");
		exit(1);
	}
}

size_t _writeDataToBuffer(char * ptr, size_t size, size_t nmemb, void * userdata)
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

void _parseDataInBuffer(const char * buffer, int buffer_size)
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
		{	_parseData(weather, cur);
		}

		cur = cur->next;
	}

	xmlFreeDoc(weather);
}

void _parseData(xmlDocPtr weather, xmlNodePtr cur)
{   xmlChar *datum;
	cur = cur->xmlChildrenNode;

	while ( cur )
    {   if ( !xmlStrcmp((const xmlChar * )cur->name, (const xmlChar * )"description") )
		{	datum = xmlNodeListGetString(weather, cur->xmlChildrenNode, 1);
			printf("datum: %s\n", datum);
			xmlFree(datum);
		}
		cur = cur->next;
	}
}

void _formatOutput(char * formatStr)
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

				case 'h':
					// heat index
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
