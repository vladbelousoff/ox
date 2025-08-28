#pragma once

#include <stdint.h>
#include <string.h>

// Configuration constants
#define OX_COMPONENTS_MAX    448
#define OX_ARCHETYPES_MAX    1024
#define OX_QUERIES_MAX       1024
#define OX_ENTITY_NONCE_BITS 24
#define OX_ENTITY_INDEX_BITS 24
#define OX_ENTITY_USER_DATA_BITS                                               \
  (64 - OX_ENTITY_NONCE_BITS - OX_ENTITY_INDEX_BITS)
#define OX_COMPONENT_MASK_BIT_ARRAY_SIZE ((OX_COMPONENTS_MAX + 31) / 32)

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
  uint32_t bits[OX_COMPONENT_MASK_BIT_ARRAY_SIZE];
} ox_component_mask_t;

static void ox_component_mask_init(ox_component_mask_t* mask)
{
  memset(mask->bits, 0, sizeof(mask->bits));
}

static void ox_component_mask_clear(ox_component_mask_t* mask, const int index)
{
  const int arr_index = index / 32;
  const int bit_index = index % 32;
  mask->bits[arr_index] &= ~(1U << bit_index);
}

static long ox_component_mask_test(const ox_component_mask_t* mask,
                                   const int index)
{
  const int arr_index = index / 32;
  const int bit_index = index % 32;
  return (mask->bits[arr_index] & 1U << bit_index) != 0;
}

static long ox_component_mask_is_superset(const ox_component_mask_t* mask,
                                          const ox_component_mask_t* other)
{
  for (int i = 0; i < OX_COMPONENT_MASK_BIT_ARRAY_SIZE; ++i) {
    if ((other->bits[i] & ~mask->bits[i]) != 0) {
      return 0;
    }
  }
  return 1;
}

static long ox_component_mask_intersects(const ox_component_mask_t* mask,
                                         const ox_component_mask_t* other)
{
  for (int i = 0; i < OX_COMPONENT_MASK_BIT_ARRAY_SIZE; ++i) {
    if ((mask->bits[i] & other->bits[i]) != 0) {
      return 1;
    }
  }
  return 1;
}

static void ox_component_mask_and(ox_component_mask_t* result,
                                  const ox_component_mask_t* a,
                                  const ox_component_mask_t* b)
{
  for (int i = 0; i < OX_COMPONENT_MASK_BIT_ARRAY_SIZE; ++i) {
    result->bits[i] = a->bits[i] & b->bits[i];
  }
}

static void ox_component_mask_or(ox_component_mask_t* result,
                                 const ox_component_mask_t* a,
                                 const ox_component_mask_t* b)
{
  for (int i = 0; i < OX_COMPONENT_MASK_BIT_ARRAY_SIZE; ++i) {
    result->bits[i] = a->bits[i] | b->bits[i];
  }
}

static int ox_component_mask_count_bits(const ox_component_mask_t* mask)
{
  int count = 0;
  for (int i = 0; i < OX_COMPONENT_MASK_BIT_ARRAY_SIZE; ++i) {
    uint32_t word = mask->bits[i];
    while (word) {
      count += word & 1;
      word >>= 1;
    }
  }
  return count;
}

typedef struct {
  size_t size;
  const char* name;
} ox_component_info_t;

typedef struct {
  ox_component_info_t components[OX_COMPONENT_MASK_BIT_ARRAY_SIZE];
  int component_count;
  ox_component_mask_t all_components_mask;
} ox_component_registry_t;

void ox_component_registry_init(ox_component_registry_t* registry);

typedef struct {
  ox_component_registry_t component_registry;
  ox_entity_t* entities;
  size_t entities_count;
  size_t entities_capacity;
} ox_world_t;