#pragma once

#include <stddef.h>

#ifdef OX_DEBUG_BUILD
#include "ox_list.h"
#endif

#ifdef OX_DEBUG_BUILD

typedef struct {
  const char* file;
  size_t line;
} ox_source_location_t;

typedef struct {
  ox_list_entry_t link;
  ox_source_location_t source_location;
  size_t buffer_size;
} ox_memory_header_t;

#define OX_SOURCE_LOCATION                                                     \
  (ox_source_location_t)                                                       \
  {                                                                            \
    .file = __FILE__, .line = __LINE__                                         \
  }

#else

typedef int ox_source_location_t;
#define OX_SOURCE_LOCATION 0

#endif

long ox_memory_init(void);
void ox_memory_exit(void);

void* ox_malloc(size_t size, ox_source_location_t source_location);
void* ox_realloc(void* memory, size_t size,
                 ox_source_location_t source_location);
void ox_free(void* memory);