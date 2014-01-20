#ifndef __MEMSAFETY_H__
#define __MEMSAFETY_H__

#include <stdio.h>
#include <stdlib.h>

// Memory Safety //
void * _memoryAbort (void)
{   puts("Failed to allocate memory");
    exit(1);
    return NULL;
}

void * _malloc (size_t size)
{   void * e = malloc(size);
    if ( e ) return e;
    else return _memoryAbort();
}

void * _realloc (void * ptr, size_t size)
{   void * e = realloc(ptr, size);
    if ( e ) return e;
    else return _memoryAbort();
}

#endif // __MEMSAFETY_H__
