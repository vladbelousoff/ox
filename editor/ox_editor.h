#pragma once

#include <gtk/gtk.h>
#include "../code/ox_core.h"
#include "../code/ox_ecs.h"

// Forward declarations
typedef struct ox_editor_t ox_editor_t;
typedef struct ox_editor_window_t ox_editor_window_t;
typedef struct ox_editor_viewport_t ox_editor_viewport_t;
typedef struct ox_editor_hierarchy_t ox_editor_hierarchy_t;
typedef struct ox_editor_properties_t ox_editor_properties_t;

// Editor application structure
typedef struct ox_editor_t {
    GtkApplication* app;
    ox_editor_window_t* main_window;
    ox_world_t* world;
    bool is_running;
} ox_editor_t;

// Main editor window
typedef struct ox_editor_window_t {
    GtkWidget* window;
    GtkWidget* main_container;
    GtkWidget* menubar;
    GtkWidget* toolbar;
    GtkWidget* paned_horizontal;
    GtkWidget* paned_vertical;
    
    ox_editor_viewport_t* viewport;
    ox_editor_hierarchy_t* hierarchy;
    ox_editor_properties_t* properties;
} ox_editor_window_t;

// Viewport for rendering the game scene
typedef struct ox_editor_viewport_t {
    GtkWidget* container;
    GtkWidget* drawing_area;
    GtkWidget* toolbar;
    
    // Raylib integration
    bool is_initialized;
    int width;
    int height;
} ox_editor_viewport_t;

// Scene hierarchy panel
typedef struct ox_editor_hierarchy_t {
    GtkWidget* container;
    GtkWidget* tree_view;
    GtkTreeStore* tree_store;
    GtkWidget* toolbar;
    
    ox_entity_id selected_entity;
} ox_editor_hierarchy_t;

// Properties panel for editing entity components
typedef struct ox_editor_properties_t {
    GtkWidget* container;
    GtkWidget* scrolled_window;
    GtkWidget* properties_box;
    
    ox_entity_id current_entity;
} ox_editor_properties_t;

// Function declarations
int ox_editor_init(ox_editor_t* editor, int argc, char** argv);
void ox_editor_run(ox_editor_t* editor);
void ox_editor_shutdown(ox_editor_t* editor);

// Window management
ox_editor_window_t* ox_editor_window_create(ox_editor_t* editor);
void ox_editor_window_destroy(ox_editor_window_t* window);

// Viewport management
ox_editor_viewport_t* ox_editor_viewport_create(ox_editor_window_t* window);
void ox_editor_viewport_destroy(ox_editor_viewport_t* viewport);
void ox_editor_viewport_render(ox_editor_viewport_t* viewport);

// Hierarchy management
ox_editor_hierarchy_t* ox_editor_hierarchy_create(ox_editor_window_t* window);
void ox_editor_hierarchy_destroy(ox_editor_hierarchy_t* hierarchy);
void ox_editor_hierarchy_refresh(ox_editor_hierarchy_t* hierarchy, ox_world_t* world);

// Properties management
ox_editor_properties_t* ox_editor_properties_create(ox_editor_window_t* window);
void ox_editor_properties_destroy(ox_editor_properties_t* properties);
void ox_editor_properties_set_entity(ox_editor_properties_t* properties, ox_entity_id entity);

// Global window accessor for inter-panel communication
ox_editor_window_t* get_current_editor_window(void);