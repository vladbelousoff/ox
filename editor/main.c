#include "ox_editor.h"
#include <stdlib.h>

static void on_activate(GtkApplication* app, gpointer user_data) {
    ox_editor_t* editor = (ox_editor_t*)user_data;
    editor->app = app;
    
    // Create and show the main window
    editor->main_window = ox_editor_create_main_window(editor);
    gtk_widget_show_all(editor->main_window);
}

int main(int argc, char* argv[]) {
    // Initialize editor
    ox_editor_t editor;
    long result = ox_editor_init(&editor);
    if (result != OX_SUCCESS) {
        fprintf(stderr, "Failed to initialize editor\n");
        return EXIT_FAILURE;
    }
    
    // Create GTK application
    GtkApplication* app = gtk_application_new("com.oxengine.editor", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), &editor);
    
    // Run the application
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    
    // Cleanup
    g_object_unref(app);
    ox_editor_cleanup(&editor);
    
    return status;
}