#include "ox_editor.h"
#include "../code/ox_log.h"
#include "../code/ox_memory.h"

#include <stdlib.h>

// Callback for application activation
static void on_app_activate(GtkApplication* app, gpointer user_data)
{
    ox_editor_t* editor = (ox_editor_t*)user_data;
    
    // Create main window
    editor->main_window = ox_editor_window_create(editor);
    if (!editor->main_window) {
        OX_LOG_ERR("Failed to create main editor window");
        return;
    }
    
    // Show the window
    gtk_widget_show_all(editor->main_window->window);
    
    OX_LOG_DBG("Editor window created and shown");
}

// Callback for window close
static gboolean on_window_delete_event(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
    ox_editor_t* editor = (ox_editor_t*)user_data;
    editor->is_running = false;
    return FALSE; // Allow window to close
}

int ox_editor_init(ox_editor_t* editor, int argc, char** argv)
{
    if (!editor) {
        return OX_FAILURE;
    }
    
    // Initialize GTK
    gtk_init(&argc, &argv);
    
    // Create GTK application
    editor->app = gtk_application_new("com.ox.editor", G_APPLICATION_FLAGS_NONE);
    if (!editor->app) {
        OX_LOG_ERR("Failed to create GTK application");
        return OX_FAILURE;
    }
    
    // Connect signals
    g_signal_connect(editor->app, "activate", G_CALLBACK(on_app_activate), editor);
    
    // Initialize world (ECS)
    editor->world = ox_mem_acquire(sizeof(ox_world_t), OX_SOURCE_LOCATION);
    if (!editor->world) {
        OX_LOG_ERR("Failed to allocate world");
        g_object_unref(editor->app);
        return OX_FAILURE;
    }
    
    // Initialize world
    ox_world_init(editor->world);
    
    editor->is_running = true;
    editor->main_window = NULL;
    
    OX_LOG_DBG("Editor initialized successfully");
    return OX_SUCCESS;
}

void ox_editor_run(ox_editor_t* editor)
{
    if (!editor || !editor->app) {
        return;
    }
    
    OX_LOG_DBG("Starting editor application");
    
    // Run the GTK application
    int status = g_application_run(G_APPLICATION(editor->app), 0, NULL);
    
    OX_LOG_DBG("Editor application finished with status: %d", status);
}

void ox_editor_shutdown(ox_editor_t* editor)
{
    if (!editor) {
        return;
    }
    
    OX_LOG_DBG("Shutting down editor");
    
    // Cleanup main window
    if (editor->main_window) {
        ox_editor_window_destroy(editor->main_window);
        editor->main_window = NULL;
    }
    
    // Cleanup world
    if (editor->world) {
        ox_world_term(editor->world);
        ox_mem_release(editor->world);
        editor->world = NULL;
    }
    
    // Cleanup GTK application
    if (editor->app) {
        g_object_unref(editor->app);
        editor->app = NULL;
    }
    
    editor->is_running = false;
    
    OX_LOG_DBG("Editor shutdown complete");
}