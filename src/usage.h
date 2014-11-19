#ifndef __USAGE_H__
#define __USAGE_H__

/*******************************************************************\
* Usage Functions for shaman                                        *
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
#include <stdio.h>
#include <stdlib.h>

// Usage //
void
_usage (int error) __attribute__((noreturn));

void 
_usage (int error) {

    fputs("Usage: shaman [options]\n\n"
          "Options:\n"
          "  -h, --help\t\t\tprint help and exit\n"
          "  -i, --imperial\t\tuse Imperial units (default)\n"
          "  -m, --metric\t\t\tuse Metric units\n"
          "  -q, --quiet\t\t\tsilence all output\n"
          "  -r, --refresh\t\t\tforce update of local cache\n"
          "  -v, --verbose\t\t\toutput more status information\n"
          "  -c, --cache\t\t\tspecify location of local cache\n"
          "  -f, --format=\"format\"\t\tformat output according to \"format\"\n"
          "  -l, --location=\"location\"\tprint weather information for "
          "\"location\"\n\n"
          "See `man 1 shaman` for more information\n", 
          (error ? stderr : stdout));
    exit(error);
}

#endif // __USAGE_H__
