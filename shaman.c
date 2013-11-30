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
static int flag_help, flag_units;
char formatString[80],
	 passloc[80];

// Prototypes //
void usage(char *progname);
void discoverConfig(void);
void parseConfig(char *configfile);
void getData(char *location);
void getReporter(char *reporterln, char *coordsln, char *elevln);
void getConditions(char *conditionsln);
void getHazards(char *hazardln);

// Main Function //
int main(int argc, char **argv)
{   if ( argc > 1 )
	{   int c;

		while (1) 
		{	static struct option long_options[] = 
			{   // Long Option Flags //
				{"help",	  no_argument,		   &flag_help,	  1},
				{"imperial",  no_argument,		   &flag_units,	  0},
				{"metric",	  no_argument,		   &flag_units,	  1},
				// Long Option Switches //
				{"config",	  required_argument,   0,			  'c'},
				{"format",	  required_argument,   0,			  'f'},
				{"location",  required_argument,   0,			  'l'},
			};

			int option_index = 0; // Stores the option index

			c = getopt_long(argc, argv, "c:f:", long_options, &option_index);

			if ( c == -1 ) break;

			switch (c)
			{   case 'h':
				    flag_help = 1;
					break;
				
				case 'c':
					parseConfig(optarg);
					break;

			    case 'f':
					strncpy(formatString, optarg, strlen(formatString));
					break;

			    case 'l':
					strncpy(passloc, optarg, strlen(passloc));
					break;
			}
		}
	}

	if ( flag_help == 1 ) usage(argv[0]);
	return 0;
}

// Error Handling Functions //
void usage(char *progname)
{   fprintf(stderr, "Usage: %s [-h] [-i|-m] [-f \"<format string>\"] -l \"<location>\"\n\n", progname);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -h, --help                  print help and exit\n");
	fprintf(stderr, "  -c, --config=filename       use specified config file\n");
	fprintf(stderr, "  -i, --imperial              use Imperial units (default)\n");
	fprintf(stderr, "  -m, --metric                use Metric units\n");
	fprintf(stderr, "  -f, --format=\"FORMAT\"       print weather information according to \"string\"\n");
	fprintf(stderr, "  -l, --location=\"location\"   print weather information for \"location\"\n");
	fprintf(stderr, "\nValid escape sequences for FORMAT are as follows:\n\n");
	fprintf(stderr, "%%%%      A literal percent sign\n");
	fprintf(stderr, "%%g      A unicode degree sign (°)\n");
	fprintf(stderr, "%%c      current condition\n");
	fprintf(stderr, "%%d      humidity\n");
	fprintf(stderr, "%%D      dew-point\n");
	fprintf(stderr, "%%h      heat index\n");
	fprintf(stderr, "%%p      pressure\n");
	fprintf(stderr, "%%r      reporter name\n");
	fprintf(stderr, "%%C      reporter coordinates\n");
	fprintf(stderr, "%%t      temperature\n");
	fprintf(stderr, "%%v      visibility\n");
	fprintf(stderr, "%%w      wind speed\n");
	fprintf(stderr, "%%W      wind direction\n");
	exit(0);
}

void invalidOption(int option)
{   fprintf(stderr, "Unrecognized option: %d\n", option);
	exit(46);
}

// Configuration Functions //
void discoverConfig(void)
{   printf("Function not yet implemented\n");
}

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
