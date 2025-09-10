#include "ox_editor_components.h"
#include "ox_memory.h"
#include <string.h>

// Component IDs
ox_component_id OX_COMPONENT_TRANSFORM = {0};
ox_component_id OX_COMPONENT_RENDER = {0};
ox_component_id OX_COMPONENT_NAME = {0};

void ox_editor_components_register(ox_component_registry_t* registry) {
    // Register transform component
    OX_COMPONENT_TRANSFORM.value = registry->component_count++;
    registry->components[OX_COMPONENT_TRANSFORM.value].size = sizeof(ox_transform_component_t);
    registry->components[OX_COMPONENT_TRANSFORM.value].name = "Transform";
    
    // Register render component
    OX_COMPONENT_RENDER.value = registry->component_count++;
    registry->components[OX_COMPONENT_RENDER.value].size = sizeof(ox_render_component_t);
    registry->components[OX_COMPONENT_RENDER.value].name = "Render";
    
    // Register name component
    OX_COMPONENT_NAME.value = registry->component_count++;
    registry->components[OX_COMPONENT_NAME.value].size = sizeof(ox_name_component_t);
    registry->components[OX_COMPONENT_NAME.value].name = "Name";
}

ox_transform_component_t* ox_editor_get_transform(ox_world_t* world, ox_entity_id entity) {
    // TODO: Implement proper component retrieval from ECS
    // For now, return NULL as placeholder
    return NULL;
}

ox_render_component_t* ox_editor_get_render(ox_world_t* world, ox_entity_id entity) {
    // TODO: Implement proper component retrieval from ECS
    // For now, return NULL as placeholder
    return NULL;
}

ox_name_component_t* ox_editor_get_name(ox_world_t* world, ox_entity_id entity) {
    // TODO: Implement proper component retrieval from ECS
    // For now, return NULL as placeholder
    return NULL;
}

ox_entity_id ox_editor_create_ball_entity(ox_world_t* world, Vector2 position, Color color, float radius) {
    if (!world) {
        ox_entity_id empty = {0};
        return empty;
    }
    
    // Create entity
    ox_entity_id entity = {0};
    entity.value = world->entities_count + 1;
    
    // TODO: Add components to entity using proper ECS system
    // For now, just increment entity count
    world->entities_count++;
    
    return entity;
}

ox_entity_id ox_editor_create_rect_entity(ox_world_t* world, Vector2 position, Color color, Rectangle rect) {
    if (!world) {
        ox_entity_id empty = {0};
        return empty;
    }
    
    // Create entity
    ox_entity_id entity = {0};
    entity.value = world->entities_count + 1;
    
    // TODO: Add components to entity using proper ECS system
    // For now, just increment entity count
    world->entities_count++;
    
    return entity;
}