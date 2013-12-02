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
FILE *url;
static int flag_help, flag_metric;
int defaultUnits;
char formatString[80],
	 passLoc[80];

// Prototypes //
void usage(char *progname);
void discoverConfig(void);
void parseConfig(char *configfile);
void getData(char *location);
void getReporter(char *reporterln, char *coordsln, char *elevln);
void getConditions(char *conditionsln);
void getHazards(char *hazardln);
void parseFormat(char *formatStr);

// Main Function //
int main(int argc, char **argv)
{   if ( argc > 1 )
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
					strncpy(passLoc, optarg, strlen(passLoc));
					break;

			    case 'm':
					flag_metric = 1;
					break;
			}
		}
	}

	if ( flag_help == 1 ) usage(argv[0]);
	return 0;
}

// Usage Function //
void usage(char *progname)
{   fprintf(stderr, "Usage: %s [-h] [-c configfile] [-i|-m] [-f \"<format string>\"] -l \"<location>\"\n\n", progname);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -h, --help                    print help and exit\n");
	fprintf(stderr, "  -c, --config=filename         use specified config file\n");
	fprintf(stderr, "  -i, --imperial                use Imperial units (default)\n");
	fprintf(stderr, "  -m, --metric                  use Metric units\n");
	fprintf(stderr, "  -f, --format=\"formatString\"   format output according to \"formatString\"\n");
	fprintf(stderr, "  -l, --location=\"location\"     print weather information for \"location\"\n");
	fprintf(stderr, "\nSee `man 1 %s` for more information\n", progname);
	exit(0);
}

/* Configuration Functions //
void discoverConfig(void)
{   config_t(cfg);
	const config_setting_t *Location, *Format, defaultUnits;
	const char *cwd = getenv("PWD");
	char *confString = "config";

	if ( !*passLoc && !*formatString && !defaultUnits )
    {   if ( chdir(getenv("XDG_CONFIG_HOME")) )
		{	if ( chdir("shaman") ) 
			{   if ( access("config", R_OK) != -1 ) config_init("config");
			}
		}
		else 
	    {	chdir(getenv("HOME"));
			if ( chdir(".config/shaman") ) access("config", R_OK);
			else 
			{   access(".shamanrc", R_OK);
				confString = ".shamanrc";
			}
		}
		config_lookup_string("Location", confString, passLoc);
		config_lookup_string("Format", confString, formatString);
		config_lookup_bool("Use_Metric", confString, flag_metric);
	}
	chdir(cwd);
}*/

void parseConfig(char *configFile)
{   printf("Function not yet implemented\n");
}

// Data and Analysis Functions //
void getData(char *location)
{   printf("Function not yet implemented\n");
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

void parseFormat(char *formatStr)
{   printf("Function not yet implemented\n");
}
