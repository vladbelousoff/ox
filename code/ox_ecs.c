#include "ox_ecs.h"
#include "ox_memory.h"
#include "ox_log.h"

#include <string.h>

void ox_component_registry_init(ox_component_registry_t* registry)
{
    if (!registry) {
        OX_LOG_ERR("Registry is NULL");
        return;
    }
    
    registry->component_count = 0;
    ox_component_mask_init(&registry->all_components_mask);
    
    // Clear component info array
    memset(registry->components, 0, sizeof(registry->components));
    
    OX_LOG_DBG("Component registry initialized");
}

void ox_component_registry_term(ox_component_registry_t* registry)
{
    if (!registry) {
        return;
    }
    
    ox_component_mask_term(&registry->all_components_mask);
    registry->component_count = 0;
    
    OX_LOG_DBG("Component registry terminated");
}

ox_component_id ox_component_registry_register(ox_component_registry_t* registry, 
                                              const char* name, 
                                              size_t size)
{
    if (!registry || !name) {
        ox_component_id invalid = {-1};
        return invalid;
    }
    
    if (registry->component_count >= OX_COMPONENTS_MAX) {
        OX_LOG_ERR("Maximum number of components reached");
        ox_component_id invalid = {-1};
        return invalid;
    }
    
    ox_component_id id = {(int)registry->component_count};
    
    registry->components[registry->component_count].name = name;
    registry->components[registry->component_count].size = size;
    
    // Set bit in all components mask
    bitset_set(registry->all_components_mask.bitset, registry->component_count);
    
    registry->component_count++;
    
    OX_LOG_DBG("Registered component '%s' with ID %d, size %zu", name, id.value, size);
    return id;
}

void ox_memory_pool_init(ox_memory_pool_t* pool, size_t element_size, size_t elements_per_chunk)
{
    if (!pool) {
        return;
    }
    
    pool->chunk_count = 0;
    pool->element_size = element_size;
    pool->elements_per_chunk = elements_per_chunk;
    
    // Initialize all chunks to NULL
    for (size_t i = 0; i < OX_ECS_POOL_MAX_CHUNKS; ++i) {
        pool->chunks[i].data = NULL;
        pool->chunks[i].capacity = 0;
        pool->chunks[i].used = 0;
    }
    
    OX_LOG_DBG("Memory pool initialized: element_size=%zu, elements_per_chunk=%zu", 
               element_size, elements_per_chunk);
}

void ox_memory_pool_term(ox_memory_pool_t* pool)
{
    if (!pool) {
        return;
    }
    
    // Free all allocated chunks
    for (size_t i = 0; i < pool->chunk_count; ++i) {
        if (pool->chunks[i].data) {
            ox_mem_release(pool->chunks[i].data);
            pool->chunks[i].data = NULL;
        }
    }
    
    pool->chunk_count = 0;
    
    OX_LOG_DBG("Memory pool terminated");
}

void ox_world_init(ox_world_t* world)
{
    if (!world) {
        return;
    }
    
    ox_component_registry_init(&world->component_registry);
    
    world->entities = NULL;
    world->entities_count = 0;
    world->entities_capacity = 0;
    
    OX_LOG_DBG("World initialized");
}

void ox_world_term(ox_world_t* world)
{
    if (!world) {
        return;
    }
    
    ox_component_registry_term(&world->component_registry);
    
    if (world->entities) {
        ox_mem_release(world->entities);
        world->entities = NULL;
    }
    
    world->entities_count = 0;
    world->entities_capacity = 0;
    
    OX_LOG_DBG("World terminated");
}