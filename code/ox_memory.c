#include "ox_memory.h"

#include "ox_core.h"
#include "ox_log.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef OX_DEBUG_BUILD
#include <threads.h>
#endif

#ifdef OX_DEBUG_BUILD
static mtx_t mem_mtx;
static ox_list_head_t mem_allocs;
#endif

long ox_memory_init(void)
{
#ifdef OX_DEBUG_BUILD
  if (mtx_init(&mem_mtx, mtx_plain) != thrd_success) {
    return OX_FAILURE;
  }
  ox_list_init(&mem_allocs);
#endif
  return OX_SUCCESS;
}

void ox_memory_exit(void)
{
#ifdef OX_DEBUG_BUILD
  ox_list_entry_t* entry;
  ox_list_entry_t* tmp;
  OX_LIST_FOR_EACH_SAFE(entry, tmp, &mem_allocs)
  {
    ox_memory_header_t* header =
      OX_LIST_OFFSET(entry, ox_memory_header_t, link);
    OX_LOG_ERR("Leaked memory, file: %s, line: %u, size: %u",
               ox_filename(header->source_location.file),
               (unsigned)header->source_location.line,
               (unsigned)header->buffer_size);
    ox_list_remove(&header->link);
    free(header);
  }

  mtx_destroy(&mem_mtx);
#endif
}

void* ox_malloc(const size_t size, const ox_source_location_t source_location)
{
#if OX_DEBUG_BUILD
  // ReSharper disable once CppDFAMemoryLeak
  char* data = malloc(size + sizeof(ox_memory_header_t));
  if (data) {
    ox_memory_header_t* header = (ox_memory_header_t*)data;
    header->source_location = source_location;
    header->buffer_size = size;
    mtx_lock(&mem_mtx);
    ox_list_add_tail(&mem_allocs, &header->link);
    mtx_unlock(&mem_mtx);
    // ReSharper disable once CppDFAMemoryLeak
    return &data[sizeof(ox_memory_header_t)];
  }
  return data;
#else
  (void)source_location;
  return malloc(size);
#endif
}

void* ox_realloc(void* memory, const size_t size,
                 const ox_source_location_t source_location)
{
#if OX_DEBUG_BUILD
  ox_memory_header_t* header =
    (ox_memory_header_t*)((char*)memory - sizeof(ox_memory_header_t));
  header->source_location = source_location;
  header->buffer_size = size;
  return realloc(memory, size + sizeof(ox_memory_header_t));
#else
  (void)source_location;
  return realloc(memory, size);
#endif
}

void ox_free(void* memory)
{
#if OX_DEBUG_BUILD
  ox_memory_header_t* header =
    (ox_memory_header_t*)((char*)memory - sizeof(ox_memory_header_t));
  ox_list_remove(&header->link);
  free(header);
#else
  free(memory);
#endif
}