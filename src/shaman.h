#ifndef __SHAMAN_H__
#define __SHAMAN_H__

// Libraries //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <curl/curl.h>
#include <getopt.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "usage.h"
#include "memsafety.h"

// Variables //
#define BUFFER_SIZE 80

char formatString[BUFFER_SIZE] = {'\0'};
char location[BUFFER_SIZE] = {'\0'};

typedef struct 
{   char * buffer;
    size_t size;
    size_t offset;
} data_t;

// Prototypes //
extern void * _memoryAbort (void);
extern void * _malloc (size_t size);
extern void * _realloc (void * ptr, size_t size);
extern void _usage (void);
static void _getData (const char * location, const int scale);
static size_t _writeDataToBuffer (char * ptr, size_t size, size_t nmemb, void * userdata);
static void _parseDataInBuffer (const char * buffer, int buffer_size);
static void _parseData (xmlDocPtr weather, xmlNodePtr cur);
static void _parseFormat (char * formatStr);
static void _formatOutput (char * formatStr);

#endif // __SHAMAN_H__
