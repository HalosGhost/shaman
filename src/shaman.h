#ifndef __SHAMAN_H__
#define __SHAMAN_H__

// Libraries //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

#include "usage.h"
#include "memsafety.h"
#include "fetch.h"
#include "format.h"
#include "parse.h"

// Prototypes //
extern void _usage (void);
extern void * _memoryAbort (void);
extern void * _malloc (size_t size);
extern void * _realloc (void * ptr, size_t size);
extern void _getData (const char * location, const int scale);
extern size_t _writeDataToBuffer (char * ptr, size_t size, size_t nmemb, void * userdata);
extern void _parseDataInBuffer (const char * buffer, int buffer_size);
extern void _parseData (xmlDocPtr weather, xmlNodePtr cur);
extern void _parseFormat (char * formatString);
extern void _formatOutput (char * formatString);

#endif // __SHAMAN_H__
