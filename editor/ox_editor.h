#pragma once

#include "ox_core.h"
#include "ox_ecs.h"
#include "ox_render.h"
#include "ox_log.h"
#include "ox_memory.h"
#include "ox_editor_scene.h"
#include "ox_editor_components.h"

#include <gtk/gtk.h>
#include <raylib.h>
#include <stdbool.h>

// Editor configuration
#define EDITOR_WINDOW_WIDTH 1200
#define EDITOR_WINDOW_HEIGHT 800
#define SCENE_VIEW_WIDTH 800
#define SCENE_VIEW_HEIGHT 600

// Editor state
typedef struct {
    GtkApplication* app;
    GtkWidget* main_window;
    GtkWidget* scene_view;
    GtkWidget* property_panel;
    GtkWidget* hierarchy_panel;
    GtkWidget* toolbar;
    GtkWidget* status_bar;
    
    // Scene data
    ox_world_t* world;
    ox_entity_id selected_entity;
    bool is_playing;
    
    // Rendering context
    RenderTexture2D scene_texture;
    bool scene_view_initialized;
} ox_editor_t;

// Editor functions
long ox_editor_init(ox_editor_t* editor);
void ox_editor_cleanup(ox_editor_t* editor);
void ox_editor_run(ox_editor_t* editor);

// UI creation functions
GtkWidget* ox_editor_create_main_window(ox_editor_t* editor);
GtkWidget* ox_editor_create_menu_bar(ox_editor_t* editor);
GtkWidget* ox_editor_create_toolbar(ox_editor_t* editor);
GtkWidget* ox_editor_create_scene_view(ox_editor_t* editor);
GtkWidget* ox_editor_create_property_panel(ox_editor_t* editor);
GtkWidget* ox_editor_create_hierarchy_panel(ox_editor_t* editor);
GtkWidget* ox_editor_create_status_bar(ox_editor_t* editor);

// Scene management
void ox_editor_new_scene(ox_editor_t* editor);
void ox_editor_save_scene(ox_editor_t* editor, const char* filename);
void ox_editor_load_scene(ox_editor_t* editor, const char* filename);

// Entity management
ox_entity_id ox_editor_create_entity(ox_editor_t* editor);
void ox_editor_delete_entity(ox_editor_t* editor, ox_entity_id entity);
void ox_editor_select_entity(ox_editor_t* editor, ox_entity_id entity);

// Rendering
void ox_editor_render_scene(ox_editor_t* editor);
gboolean ox_editor_scene_view_draw(GtkWidget* widget, cairo_t* cr, gpointer user_data);

// Callbacks
void ox_editor_on_file_new(GtkWidget* widget, gpointer user_data);
void ox_editor_on_file_open(GtkWidget* widget, gpointer user_data);
void ox_editor_on_file_save(GtkWidget* widget, gpointer user_data);
void ox_editor_on_file_save_as(GtkWidget* widget, gpointer user_data);
void ox_editor_on_play(GtkWidget* widget, gpointer user_data);
void ox_editor_on_stop(GtkWidget* widget, gpointer user_data);
void ox_editor_on_entity_selected(GtkWidget* widget, gpointer user_data);