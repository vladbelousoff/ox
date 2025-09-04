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

/**
 * @brief Initialize the memory management system
 * 
 * This function must be called before any memory allocation operations.
 * It sets up the internal memory tracking structures and prepares the
 * memory management system for use.
 * 
 * @return 0 on success, negative value on failure
 */
long ox_memory_init(void);

/**
 * @brief Clean up the memory management system
 * 
 * This function should be called when the memory management system is
 * no longer needed. It performs cleanup operations and releases any
 * internal resources.
 * 
 * In debug builds, this function will also report any memory leaks
 * that occurred during program execution.
 */
void ox_memory_exit(void);

/**
 * @brief Allocate memory with source location tracking
 * 
 * Allocates a block of memory of the specified size. In debug builds,
 * the allocation is tracked with source location information for
 * memory leak detection and debugging.
 * 
 * @param size The size of memory to allocate in bytes
 * @param source_location Source location information (use OX_SOURCE_LOCATION macro)
 * @return Pointer to allocated memory on success, NULL on failure
 * 
 * @note The allocated memory is not initialized and may contain garbage data
 * @note In debug builds, memory allocations are tracked for leak detection
 * @note Use ox_mem_release() to free memory allocated by this function
 * 
 * @see ox_mem_release
 * @see ox_mem_reclaim
 */
void* ox_mem_acquire(size_t size, ox_source_location_t source_location);

/**
 * @brief Reallocate memory with source location tracking
 * 
 * Changes the size of a previously allocated memory block. The contents
 * of the memory block are preserved up to the minimum of the old and new sizes.
 * 
 * @param mem Pointer to previously allocated memory (can be NULL)
 * @param size New size of the memory block in bytes
 * @param source_location Source location information (use OX_SOURCE_LOCATION macro)
 * @return Pointer to reallocated memory on success, NULL on failure
 * 
 * @note If mem is NULL, this function behaves like ox_mem_acquire()
 * @note If size is 0, the memory is freed and NULL is returned
 * @note The returned pointer may be different from the input pointer
 * @note In debug builds, reallocations are tracked for leak detection
 * 
 * @see ox_mem_acquire
 * @see ox_mem_release
 */
void* ox_mem_reclaim(void* mem, size_t size,
                     ox_source_location_t source_location);

/**
 * @brief Free previously allocated memory
 * 
 * Releases memory that was previously allocated by ox_mem_acquire() or
 * ox_mem_reclaim(). The pointer becomes invalid after this call.
 * 
 * @param mem Pointer to memory to free (can be NULL)
 * 
 * @note Passing NULL is safe and has no effect
 * @note In debug builds, this function validates the pointer and updates
 *       memory tracking information
 * @note Attempting to free the same pointer twice results in undefined behavior
 * 
 * @see ox_mem_acquire
 * @see ox_mem_reclaim
 */
void ox_mem_release(void* mem);