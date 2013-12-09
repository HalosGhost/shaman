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
#include <libconfig.h>
#include <getopt.h>

// Variables //
static int flag_help, flag_metric;
int defaultUnits;
char provider[] = "http://forecast.weather.gov/zipcity.php?inputstring=",
     formatString[80] = "",
	 passLoc[80] = "",
	 confPath[80] = "";

// Prototypes //
void usage(char *progname);
void discoverConfig(void);
void parseConfig(char *configfile);
void getData(char *location);
void getReporter(char *reporterln, char *coordsln, char *elevln);
void getConditions(char *conditionsln);
void getHazards(char *hazardln);
void formatOutput(char *formatStr);

// Main Function //
int main(int argc, char **argv)
{   //discoverConfig(); // Look for default configuration
	if ( argc > 1 )
	{   int c;

		while (1) 
		{	static struct option long_options[] = 
			{   // Long Option Flags //
				{"help",	  no_argument,		   0,	 'h'   },
				{"imperial",  no_argument,		   0,	 'i'   },
				{"metric",	  no_argument,		   0,	 'm'   },
				// Long Option Switches //
				{"config",	  required_argument,   0,	 'c'   },
				{"format",	  required_argument,   0,	 'f'   },
				{"location",  required_argument,   0,	 'l'   },
				{0,			  0,				   0,	 0	   },
			};

			int option_index = 0; // Stores the option index

			c = getopt_long(argc, argv, "hc:if:l:m", long_options, &option_index);

			if ( c == -1 ) break;

			switch (c)
			{   case 'h':
				    flag_help = 1;
					break;
				
				case 'c':
					parseConfig(optarg);
					break;

			    case 'i':
					flag_metric = 0;
					break;

			    case 'f':
					strncpy(formatString, optarg, strlen(formatString));
					break;

			    case 'l':
					strncpy(passLoc, optarg, 80);
					break;

			    case 'm':
					flag_metric = 1;
					break;
			}
		}
	}

	if ( flag_help == 1 ) usage(argv[0]);
	
	if ( *passLoc ) getData(passLoc);
	else
    {   fprintf(stderr, "No specified location\n");
		exit(1);
	}

	// Call other functions for parsing

	return 0;
}

// Usage Function //
void usage(char *progname)
{   fprintf(stderr, "Usage: %s [options]\n\n", progname);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -h, --help                    print help and exit\n");
	fprintf(stderr, "  -c, --config=/path/to/config  use specified config file\n");
	fprintf(stderr, "  -i, --imperial                use Imperial units (default)\n");
	fprintf(stderr, "  -m, --metric                  use Metric units\n");
	fprintf(stderr, "  -f, --format=\"FORMAT\"         format output according to \"FORMAT\"\n");
	fprintf(stderr, "  -l, --location=\"location\"     print weather information for \"location\"\n");
	fprintf(stderr, "\nSee `man 1 %s` for more information\n", progname);
	exit(0);
}

// Configuration Functions //
void discoverConfig(void)
{   const char *cwd = getenv("PWD");
	strncpy(confPath, strcat(getenv("XDG_CONFIG_HOME"), "/shaman/config"), 80);
	if ( access(confPath, F_OK) == -1 )
    {   strncpy(confPath, strcat(getenv("HOME"), "/.shamanrc"), 80);
	}
	if ( access(confPath, F_OK) != -1 ) parseConfig(confPath);
	chdir(cwd);
}

void parseConfig(char *configFile)
{   printf("Configuration Parsing is not yet implemented\n");
	/*config_t(cfg);
	const config_setting_t *Location, *Format, defaultUnits;
	char *confString = "config";
	config_lookup_string("Location", confString, passLoc);
	config_lookup_string("Format", confString, formatString);
	config_lookup_bool("Use_Metric", confString, flag_metric);*/
}

// Data and Analysis Functions //
void getData(char *location)
{   CURL *handle;
	CURLcode res;
	char url[256] = "";
	FILE *suppressOutput = fopen("/dev/null", "wb"),
		 *xml = fopen("/tmp/weather.xml","w");

	curl_global_init(CURL_GLOBAL_ALL);
	handle = curl_easy_init();

	if (handle)
    {   strncpy(url, provider, strlen(provider));
		strncat(url, location, 255-strlen(provider));
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, suppressOutput);
		curl_easy_setopt(handle, CURLOPT_URL, url);
		curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);

		res = curl_easy_perform(handle);
		if ( res == CURLE_OK )
	    {	char *interim;
			res = curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &interim);
			strncpy(url, interim, 255);
			if ( res == CURLE_OK )
			{   strncat(url, "&FcstType=dwml", 15);
				curl_easy_setopt(handle, CURLOPT_URL, url);
				curl_easy_setopt(handle, CURLOPT_WRITEDATA, xml);
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
	fclose(xml);
	curl_easy_cleanup(handle);
	curl_global_cleanup();

	// Parse XML file
	// Call getReporter(), getConditions() and getHazards() if needed

	if ( *formatString ) formatOutput(formatString);
	else
    {   fprintf(stderr, "No specified format\n");
		exit(1);
	}
}

void getReporter(char *reporterln, char *coordsln, char *elevln)
{   printf("Function not yet implemented\n");
}

void getConditions(char *conditionsln)
{   printf("Function not yet implemented\n");
}

void getHazards(char *hazardln)
{   printf("Function not yet implemented\n");
}

void formatOutput(char *formatStr)
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
