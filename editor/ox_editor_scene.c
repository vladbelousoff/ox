#include "ox_editor_scene.h"
#include "ox_memory.h"
#include "ox_log.h"
#include <stdio.h>
#include <string.h>

#define OX_SCENE_MAGIC "OXS\0"
#define OX_SCENE_VERSION 1

// Simple scene data structure for now
typedef struct {
    ox_entity_id* entities;
    size_t entity_count;
    size_t entity_capacity;
    
    // Component storage (simplified)
    ox_transform_component_t* transforms;
    ox_render_component_t* renders;
    ox_name_component_t* names;
} ox_scene_data_t;

static ox_scene_data_t* g_scene_data = NULL;

long ox_scene_init(ox_world_t* world) {
    if (!world) {
        OX_LOG_ERR("World is null");
        return OX_FAILURE;
    }
    
    // Initialize scene data
    g_scene_data = ox_mem_acquire(sizeof(ox_scene_data_t), OX_SOURCE_LOCATION);
    if (!g_scene_data) {
        OX_LOG_ERR("Failed to allocate scene data");
        return OX_FAILURE;
    }
    
    memset(g_scene_data, 0, sizeof(ox_scene_data_t));
    
    // Initialize entity storage
    g_scene_data->entity_capacity = 1000;
    g_scene_data->entities = ox_mem_acquire(sizeof(ox_entity_id) * g_scene_data->entity_capacity, OX_SOURCE_LOCATION);
    g_scene_data->transforms = ox_mem_acquire(sizeof(ox_transform_component_t) * g_scene_data->entity_capacity, OX_SOURCE_LOCATION);
    g_scene_data->renders = ox_mem_acquire(sizeof(ox_render_component_t) * g_scene_data->entity_capacity, OX_SOURCE_LOCATION);
    g_scene_data->names = ox_mem_acquire(sizeof(ox_name_component_t) * g_scene_data->entity_capacity, OX_SOURCE_LOCATION);
    
    if (!g_scene_data->entities || !g_scene_data->transforms || 
        !g_scene_data->renders || !g_scene_data->names) {
        OX_LOG_ERR("Failed to allocate scene component arrays");
        ox_scene_cleanup(world);
        return OX_FAILURE;
    }
    
    // Register components
    ox_editor_components_register(&world->component_registry);
    
    OX_LOG_INFO("Scene initialized successfully");
    return OX_SUCCESS;
}

void ox_scene_cleanup(ox_world_t* world) {
    if (!g_scene_data) return;
    
    if (g_scene_data->entities) {
        ox_mem_release(g_scene_data->entities);
    }
    if (g_scene_data->transforms) {
        ox_mem_release(g_scene_data->transforms);
    }
    if (g_scene_data->renders) {
        ox_mem_release(g_scene_data->renders);
    }
    if (g_scene_data->names) {
        ox_mem_release(g_scene_data->names);
    }
    
    ox_mem_release(g_scene_data);
    g_scene_data = NULL;
    
    OX_LOG_INFO("Scene cleaned up");
}

long ox_scene_save(ox_world_t* world, const char* filename) {
    if (!world || !filename || !g_scene_data) {
        OX_LOG_ERR("Invalid parameters for scene save");
        return OX_FAILURE;
    }
    
    FILE* file = fopen(filename, "wb");
    if (!file) {
        OX_LOG_ERR("Failed to open file for writing: %s", filename);
        return OX_FAILURE;
    }
    
    // Write header
    ox_scene_header_t header;
    memcpy(header.magic, OX_SCENE_MAGIC, 4);
    header.version = OX_SCENE_VERSION;
    header.entity_count = (uint32_t)g_scene_data->entity_count;
    header.component_count = 3; // Transform, Render, Name
    
    if (fwrite(&header, sizeof(ox_scene_header_t), 1, file) != 1) {
        OX_LOG_ERR("Failed to write scene header");
        fclose(file);
        return OX_FAILURE;
    }
    
    // Write entities and their components
    for (size_t i = 0; i < g_scene_data->entity_count; ++i) {
        ox_entity_id entity = g_scene_data->entities[i];
        
        // Write transform component
        ox_scene_component_entry_t entry;
        entry.entity = entity;
        entry.component_id = OX_COMPONENT_TRANSFORM;
        entry.data_size = sizeof(ox_transform_component_t);
        
        if (fwrite(&entry, sizeof(ox_scene_component_entry_t), 1, file) != 1 ||
            fwrite(&g_scene_data->transforms[i], sizeof(ox_transform_component_t), 1, file) != 1) {
            OX_LOG_ERR("Failed to write transform component");
            fclose(file);
            return OX_FAILURE;
        }
        
        // Write render component
        entry.component_id = OX_COMPONENT_RENDER;
        entry.data_size = sizeof(ox_render_component_t);
        
        if (fwrite(&entry, sizeof(ox_scene_component_entry_t), 1, file) != 1 ||
            fwrite(&g_scene_data->renders[i], sizeof(ox_render_component_t), 1, file) != 1) {
            OX_LOG_ERR("Failed to write render component");
            fclose(file);
            return OX_FAILURE;
        }
        
        // Write name component
        entry.component_id = OX_COMPONENT_NAME;
        entry.data_size = sizeof(ox_name_component_t);
        
        if (fwrite(&entry, sizeof(ox_scene_component_entry_t), 1, file) != 1 ||
            fwrite(&g_scene_data->names[i], sizeof(ox_name_component_t), 1, file) != 1) {
            OX_LOG_ERR("Failed to write name component");
            fclose(file);
            return OX_FAILURE;
        }
    }
    
    fclose(file);
    OX_LOG_INFO("Scene saved successfully: %s", filename);
    return OX_SUCCESS;
}

long ox_scene_load(ox_world_t* world, const char* filename) {
    if (!world || !filename || !g_scene_data) {
        OX_LOG_ERR("Invalid parameters for scene load");
        return OX_FAILURE;
    }
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        OX_LOG_ERR("Failed to open file for reading: %s", filename);
        return OX_FAILURE;
    }
    
    // Read header
    ox_scene_header_t header;
    if (fread(&header, sizeof(ox_scene_header_t), 1, file) != 1) {
        OX_LOG_ERR("Failed to read scene header");
        fclose(file);
        return OX_FAILURE;
    }
    
    // Validate header
    if (memcmp(header.magic, OX_SCENE_MAGIC, 4) != 0) {
        OX_LOG_ERR("Invalid scene file magic");
        fclose(file);
        return OX_FAILURE;
    }
    
    if (header.version != OX_SCENE_VERSION) {
        OX_LOG_ERR("Unsupported scene version: %u", header.version);
        fclose(file);
        return OX_FAILURE;
    }
    
    // Clear current scene
    ox_scene_clear(world);
    
    // Load entities
    for (uint32_t i = 0; i < header.entity_count; ++i) {
        // Read transform component
        ox_scene_component_entry_t entry;
        if (fread(&entry, sizeof(ox_scene_component_entry_t), 1, file) != 1) {
            OX_LOG_ERR("Failed to read component entry");
            fclose(file);
            return OX_FAILURE;
        }
        
        if (entry.component_id.value == OX_COMPONENT_TRANSFORM.value) {
            if (fread(&g_scene_data->transforms[i], sizeof(ox_transform_component_t), 1, file) != 1) {
                OX_LOG_ERR("Failed to read transform component");
                fclose(file);
                return OX_FAILURE;
            }
        }
        
        // Read render component
        if (fread(&entry, sizeof(ox_scene_component_entry_t), 1, file) != 1) {
            OX_LOG_ERR("Failed to read component entry");
            fclose(file);
            return OX_FAILURE;
        }
        
        if (entry.component_id.value == OX_COMPONENT_RENDER.value) {
            if (fread(&g_scene_data->renders[i], sizeof(ox_render_component_t), 1, file) != 1) {
                OX_LOG_ERR("Failed to read render component");
                fclose(file);
                return OX_FAILURE;
            }
        }
        
        // Read name component
        if (fread(&entry, sizeof(ox_scene_component_entry_t), 1, file) != 1) {
            OX_LOG_ERR("Failed to read component entry");
            fclose(file);
            return OX_FAILURE;
        }
        
        if (entry.component_id.value == OX_COMPONENT_NAME.value) {
            if (fread(&g_scene_data->names[i], sizeof(ox_name_component_t), 1, file) != 1) {
                OX_LOG_ERR("Failed to read name component");
                fclose(file);
                return OX_FAILURE;
            }
        }
        
        g_scene_data->entities[i] = entry.entity;
        g_scene_data->entity_count++;
    }
    
    fclose(file);
    OX_LOG_INFO("Scene loaded successfully: %s", filename);
    return OX_SUCCESS;
}

void ox_scene_clear(ox_world_t* world) {
    if (!g_scene_data) return;
    
    g_scene_data->entity_count = 0;
    OX_LOG_INFO("Scene cleared");
}

ox_entity_id ox_scene_add_entity(ox_world_t* world, const char* name) {
    if (!world || !g_scene_data || g_scene_data->entity_count >= g_scene_data->entity_capacity) {
        ox_entity_id empty = {0};
        return empty;
    }
    
    size_t index = g_scene_data->entity_count;
    
    // Create entity
    ox_entity_id entity = {0};
    entity.value = index + 1;
    g_scene_data->entities[index] = entity;
    
    // Initialize components
    g_scene_data->transforms[index].position = (Vector2){0, 0};
    g_scene_data->transforms[index].rotation = 0.0f;
    g_scene_data->transforms[index].scale = (Vector2){1, 1};
    
    g_scene_data->renders[index].color = WHITE;
    g_scene_data->renders[index].radius = 10.0f;
    g_scene_data->renders[index].rect = (Rectangle){0, 0, 20, 20};
    g_scene_data->renders[index].shape_type = 0; // Circle
    
    if (name) {
        strncpy(g_scene_data->names[index].name, name, sizeof(g_scene_data->names[index].name) - 1);
        g_scene_data->names[index].name[sizeof(g_scene_data->names[index].name) - 1] = '\0';
    } else {
        snprintf(g_scene_data->names[index].name, sizeof(g_scene_data->names[index].name), "Entity_%zu", index);
    }
    
    g_scene_data->entity_count++;
    
    return entity;
}

void ox_scene_remove_entity(ox_world_t* world, ox_entity_id entity) {
    if (!world || !g_scene_data || entity.value == 0) return;
    
    // Find entity index
    size_t index = SIZE_MAX;
    for (size_t i = 0; i < g_scene_data->entity_count; ++i) {
        if (g_scene_data->entities[i].value == entity.value) {
            index = i;
            break;
        }
    }
    
    if (index == SIZE_MAX) return;
    
    // Move last entity to this position
    if (index < g_scene_data->entity_count - 1) {
        g_scene_data->entities[index] = g_scene_data->entities[g_scene_data->entity_count - 1];
        g_scene_data->transforms[index] = g_scene_data->transforms[g_scene_data->entity_count - 1];
        g_scene_data->renders[index] = g_scene_data->renders[g_scene_data->entity_count - 1];
        g_scene_data->names[index] = g_scene_data->names[g_scene_data->entity_count - 1];
    }
    
    g_scene_data->entity_count--;
}

void ox_scene_render(ox_world_t* world, RenderTexture2D target) {
    if (!world || !g_scene_data) return;
    
    BeginTextureMode(target);
    ClearBackground(BLACK);
    
    // Render all entities
    for (size_t i = 0; i < g_scene_data->entity_count; ++i) {
        ox_transform_component_t* transform = &g_scene_data->transforms[i];
        ox_render_component_t* render = &g_scene_data->renders[i];
        
        if (render->shape_type == 0) {
            // Draw circle
            DrawCircleV(transform->position, render->radius, render->color);
        } else {
            // Draw rectangle
            Rectangle rect = render->rect;
            rect.x += transform->position.x;
            rect.y += transform->position.y;
            DrawRectangleRec(rect, render->color);
        }
    }
    
    EndTextureMode();
}