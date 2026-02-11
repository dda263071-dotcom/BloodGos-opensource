#ifndef MALLOC_H
#define MALLOC_H

#include "types.h"

void* malloc(size_t size);
void  free(void* ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);

void heap_init(void* start, size_t size);   // inisialisasi heap
void heap_debug(void);                      // opsional: print info heap

#endif
