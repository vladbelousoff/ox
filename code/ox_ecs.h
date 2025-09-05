#pragma once

#include "ox_core.h"

#include <bitset.h>
#include <stdint.h>

// Configuration constants
#define OX_COMPONENTS_MAX              448
#define OX_ECS_ARCHETYPES_MAX          1024
#define OX_ECS_QUERIES_MAX             1024
#define OX_ENTITY_NONCE_BITS           24
#define OX_ENTITY_INDEX_BITS           24
#define OX_ECS_POOL_DEFAULT_CHUNK_SIZE 512
#define OX_ECS_POOL_MAX_CHUNKS         128

#define OX_ENTITY_USER_DATA_BITS                                               \
  (sizeof(uint64_t) * 8 - OX_ENTITY_NONCE_BITS - OX_ENTITY_INDEX_BITS)

OX_DECLARE_ID(ox_component_id);
OX_DECLARE_ID(ox_query_id);
OX_DECLARE_ID(ox_system_id);
OX_DECLARE_ID(ox_archetype_id);

// Entity structure (64-bit handle)
typedef struct {
  union {
    struct {
      uint64_t index : OX_ENTITY_INDEX_BITS;
      uint64_t nonce : OX_ENTITY_NONCE_BITS;
      uint64_t userdata : OX_ENTITY_USER_DATA_BITS;
    };
    uint64_t value;
  };
} ox_entity_id;

typedef struct {
  size_t size;
  const char* name;
} ox_component_info_t;

typedef struct {
  bitset_t* bitset;
} ox_component_mask_t;

static void ox_component_mask_init(ox_component_mask_t* self)
{
  self->bitset = bitset_create_with_capacity(OX_COMPONENTS_MAX);
}

static void ox_component_mask_term(const ox_component_mask_t* self)
{
  bitset_free(self->bitset);
}

typedef struct {
  ox_component_info_t components[OX_COMPONENTS_MAX];
  size_t component_count;
  ox_component_mask_t all_components_mask;
} ox_component_registry_t;

void ox_component_registry_init(ox_component_registry_t* registry);

typedef struct {
  void* data;
  size_t capacity;
  size_t used;
} ox_memory_chunk_t;

typedef struct {
  ox_memory_chunk_t chunks[OX_ECS_POOL_MAX_CHUNKS];
  size_t chunk_count;
  size_t element_size;
  size_t elements_per_chunk;
} ox_memory_pool_t;

typedef struct {
  ox_component_mask_t component_mask;
  ox_component_mask_t all_components_mask;

  // Component pools (one per component type)
  ox_memory_pool_t* component_pools;
  size_t component_pool_count;

  size_t entity_count;
  size_t capacity;
} ox_archetype_t;

typedef struct {
  ox_component_mask_t include_mask;
  ox_component_mask_t exclude_mask;
} ox_query_filter_t;

typedef struct {
  ox_component_registry_t component_registry;
  ox_entity_id* entities;
  size_t entities_count;
  size_t entities_capacity;
} ox_world_t;