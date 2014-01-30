#ifndef __PARSE_H__
#define __PARSE_H__

// Includes //
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include "memsafety.h"

// XML Data Buffer //
typedef struct 
{   char * buffer;
    size_t size;
    size_t offset;
} data_t;

// Prototypes //
static size_t _writeDataToBuffer (char * ptr, size_t size, size_t nmemb, void * userdata);
static void _parseDataInBuffer (const char * buffer, int buffer_size);
static void _parseData (xmlDocPtr weather, xmlNodePtr cur);

// DWML Parsing //
size_t _writeDataToBuffer (char * ptr, size_t size, size_t nmemb, void * userdata)
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

void _parseDataInBuffer (const char * buffer, int buffer_size)
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
        {   _parseData(weather, cur);
        }

        cur = cur->next;
    }

    xmlFreeDoc(weather);
}

void _parseData (xmlDocPtr weather, xmlNodePtr cur)
{   xmlChar * datum;
    cur = cur->xmlChildrenNode;

    while ( cur )
    {   if ( !xmlStrcmp((const xmlChar * )cur->name, (const xmlChar * )"description") )
        {   datum = xmlNodeListGetString(weather, cur->xmlChildrenNode, 1);
            printf("datum: %s\n", datum);
            xmlFree(datum);
        }
        cur = cur->next;
    }
}

#endif // __PARSE_H__
