#pragma once

#include "ox_core.h"
#include "ox_ecs.h"
#include "ox_editor_components.h"
#include <raylib.h>

// Scene file format structures
typedef struct {
    char magic[4]; // "OXS\0"
    uint32_t version;
    uint32_t entity_count;
    uint32_t component_count;
} ox_scene_header_t;

typedef struct {
    ox_entity_id entity;
    ox_component_id component_id;
    uint32_t data_size;
    // Component data follows
} ox_scene_component_entry_t;

// Scene management functions
long ox_scene_init(ox_world_t* world);
void ox_scene_cleanup(ox_world_t* world);

// Scene I/O functions
long ox_scene_save(ox_world_t* world, const char* filename);
long ox_scene_load(ox_world_t* world, const char* filename);

// Scene manipulation functions
void ox_scene_clear(ox_world_t* world);
ox_entity_id ox_scene_add_entity(ox_world_t* world, const char* name);
void ox_scene_remove_entity(ox_world_t* world, ox_entity_id entity);

// Component management
long ox_scene_add_component(ox_world_t* world, ox_entity_id entity, ox_component_id component_id, const void* data);
void* ox_scene_get_component(ox_world_t* world, ox_entity_id entity, ox_component_id component_id);
long ox_scene_remove_component(ox_world_t* world, ox_entity_id entity, ox_component_id component_id);

// Scene rendering
void ox_scene_render(ox_world_t* world, RenderTexture2D target);