#ifndef __FORMAT_H__
#define __FORMAT_H__

// Prototypes //
static void _parseFormat (char * formatString);
static void _formatOutput (char * formatString);

// Format Functions //
void _parseFormat (char * formatString)
{   for ( ; *formatString; ++formatString)
    {   if ( *formatString == '%' )
        {   if ( *formatString == '0' ) formatString++;

            switch (*++formatString)
            {   case '\0':
                    --formatString;
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

void _formatOutput (char * formatString)
{   for ( ; *formatString; ++formatString)
    {   if ( *formatString == '%' )
        {   if ( *formatString == '0' ) formatString++;

            switch (*++formatString)
            {   case '\0':
                    --formatString;
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

#endif // __FORMAT_H__
