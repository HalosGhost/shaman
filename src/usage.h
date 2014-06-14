#ifndef __USAGE_H__
#define __USAGE_H__

#include <stdio.h>
#include <stdlib.h>

// Usage //
void _usage (void)
{   fputs("Usage: shaman [options]\n\n"
          "Options:\n"
          "-h, --help\t\t\tprint help and exit\n"
          "-i, --imperial\t\t\tuse Imperial units (default)\n"
          "-m, --metric\t\t\tuse Metric units\n"
          "-f, --format=\"FORMAT\"\t\tformat output according to \"FORMAT\"\n"
          "-l, --location=\"location\"\tprint weather information for \"location\"\n"
          "-v, --verbose\t\t\tprint verbosely\n"
          "See `man 1 shaman` for more information\n", stderr);
    exit(0);
}

#endif // __USAGE_H__
