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
        {   static struct option options[] =
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

            c = getopt_long(argc, argv, "hif:l:m", options, &optionIndex);

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

// vim:set ts=4 sw=4 et:
