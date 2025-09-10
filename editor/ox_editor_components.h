#pragma once

#include "ox_core.h"
#include "ox_ecs.h"
#include <raylib.h>

// Basic components for the editor

// Transform component
typedef struct {
    Vector2 position;
    float rotation;
    Vector2 scale;
} ox_transform_component_t;

// Render component
typedef struct {
    Color color;
    float radius; // For circles
    Rectangle rect; // For rectangles
    int shape_type; // 0 = circle, 1 = rectangle
} ox_render_component_t;

// Name component
typedef struct {
    char name[64];
} ox_name_component_t;

// Editor-specific component IDs
extern ox_component_id OX_COMPONENT_TRANSFORM;
extern ox_component_id OX_COMPONENT_RENDER;
extern ox_component_id OX_COMPONENT_NAME;

// Component registration functions
void ox_editor_components_register(ox_component_registry_t* registry);

// Component utility functions
ox_transform_component_t* ox_editor_get_transform(ox_world_t* world, ox_entity_id entity);
ox_render_component_t* ox_editor_get_render(ox_world_t* world, ox_entity_id entity);
ox_name_component_t* ox_editor_get_name(ox_world_t* world, ox_entity_id entity);

// Entity creation helpers
ox_entity_id ox_editor_create_ball_entity(ox_world_t* world, Vector2 position, Color color, float radius);
ox_entity_id ox_editor_create_rect_entity(ox_world_t* world, Vector2 position, Color color, Rectangle rect);