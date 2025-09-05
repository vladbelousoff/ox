#pragma once

#include "bitset.h"

#include <stdint.h>

// Configuration constants
#define OX_COMPONENTS_MAX    448
#define OX_ARCHETYPES_MAX    1024
#define OX_QUERIES_MAX       1024
#define OX_ENTITY_NONCE_BITS 24
#define OX_ENTITY_INDEX_BITS 24
#define OX_ENTITY_USER_DATA_BITS                                               \
  (sizeof(uint64_t) * 8 - OX_ENTITY_NONCE_BITS - OX_ENTITY_INDEX_BITS)

typedef struct {
  int value;
} ox_component_id_t;

typedef struct {
  int value;
} ox_query_id_t;

typedef struct {
  int value;
} ox_system_id_t;

typedef struct {
  int value;
} ox_archetype_id_t;

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
} ox_entity_t;

typedef struct {
  size_t size;
  const char* name;
} ox_component_info_t;

typedef struct {
  ox_component_info_t components[OX_COMPONENTS_MAX];
  size_t component_count;
  bitset_t* all_components_mask;
} ox_component_registry_t;

void ox_component_registry_init(ox_component_registry_t* registry);

typedef struct {
  ox_component_registry_t component_registry;
  ox_entity_t* entities;
  size_t entities_count;
  size_t entities_capacity;
} ox_world_t;