#include "ox_editor.h"
#include <string.h>
#include <stdio.h>

static ox_editor_t* g_editor = NULL;

// Global editor instance getter
ox_editor_t* ox_editor_get_instance(void) {
    return g_editor;
}

long ox_editor_init(ox_editor_t* editor) {
    if (!editor) {
        OX_LOG_ERR("Editor instance is null");
        return OX_FAILURE;
    }
    
    memset(editor, 0, sizeof(ox_editor_t));
    g_editor = editor;
    
    // Initialize GTK
    if (!gtk_init_check(NULL, NULL)) {
        OX_LOG_ERR("Failed to initialize GTK");
        return OX_FAILURE;
    }
    
    // Initialize engine systems
    long ret = ox_memory_init();
    if (ret != OX_SUCCESS) {
        OX_LOG_ERR("Failed to initialize memory system");
        return ret;
    }
    
    ret = ox_render_init();
    if (ret != OX_SUCCESS) {
        OX_LOG_ERR("Failed to initialize render system");
        ox_memory_exit();
        return ret;
    }
    
    // Initialize world
    editor->world = ox_mem_acquire(sizeof(ox_world_t), OX_SOURCE_LOCATION);
    if (!editor->world) {
        OX_LOG_ERR("Failed to allocate world");
        ox_render_exit();
        ox_memory_exit();
        return OX_FAILURE;
    }
    
    // Initialize scene system
    ret = ox_scene_init(editor->world);
    if (ret != OX_SUCCESS) {
        OX_LOG_ERR("Failed to initialize scene system");
        ox_mem_release(editor->world);
        ox_render_exit();
        ox_memory_exit();
        return ret;
    }
    
    // Initialize scene texture
    editor->scene_texture = LoadRenderTexture(SCENE_VIEW_WIDTH, SCENE_VIEW_HEIGHT);
    editor->scene_view_initialized = false;
    editor->is_playing = false;
    editor->selected_entity.value = 0;
    
    OX_LOG_INFO("Editor initialized successfully");
    return OX_SUCCESS;
}

void ox_editor_cleanup(ox_editor_t* editor) {
    if (!editor) return;
    
    // Cleanup scene texture
    if (editor->scene_view_initialized) {
        UnloadRenderTexture(editor->scene_texture);
    }
    
    // Cleanup world
    if (editor->world) {
        ox_scene_cleanup(editor->world);
        ox_mem_release(editor->world);
    }
    
    // Cleanup engine systems
    ox_render_exit();
    ox_memory_exit();
    
    g_editor = NULL;
    OX_LOG_INFO("Editor cleaned up");
}

void ox_editor_run(ox_editor_t* editor) {
    if (!editor) {
        OX_LOG_ERR("Editor instance is null");
        return;
    }
    
    // Create main window
    editor->main_window = ox_editor_create_main_window(editor);
    if (!editor->main_window) {
        OX_LOG_ERR("Failed to create main window");
        return;
    }
    
    gtk_widget_show_all(editor->main_window);
    
    // Start GTK main loop
    gtk_main();
}

GtkWidget* ox_editor_create_main_window(ox_editor_t* editor) {
    GtkWidget* window = gtk_application_window_new(editor->app);
    gtk_window_set_title(GTK_WINDOW(window), "OX Game Engine Editor");
    gtk_window_set_default_size(GTK_WINDOW(window), EDITOR_WINDOW_WIDTH, EDITOR_WINDOW_HEIGHT);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    
    // Create main container
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    // Create menu bar
    GtkWidget* menu_bar = ox_editor_create_menu_bar(editor);
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);
    
    // Create toolbar
    GtkWidget* toolbar = ox_editor_create_toolbar(editor);
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
    
    // Create main content area
    GtkWidget* hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), hpaned, TRUE, TRUE, 0);
    
    // Left panel (hierarchy)
    GtkWidget* left_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget* hierarchy = ox_editor_create_hierarchy_panel(editor);
    gtk_box_pack_start(GTK_BOX(left_panel), hierarchy, TRUE, TRUE, 0);
    gtk_paned_add1(GTK_PANED(hpaned), left_panel);
    
    // Center panel (scene view)
    GtkWidget* center_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget* scene_view = ox_editor_create_scene_view(editor);
    gtk_box_pack_start(GTK_BOX(center_panel), scene_view, TRUE, TRUE, 0);
    gtk_paned_add2(GTK_PANED(hpaned), center_panel);
    
    // Right panel (properties)
    GtkWidget* right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget* properties = ox_editor_create_property_panel(editor);
    gtk_box_pack_start(GTK_BOX(right_panel), properties, TRUE, TRUE, 0);
    
    // Create another paned widget for right side
    GtkWidget* right_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_add1(GTK_PANED(right_paned), center_panel);
    gtk_paned_add2(GTK_PANED(right_paned), right_panel);
    
    // Replace the original hpaned
    gtk_container_remove(GTK_CONTAINER(vbox), hpaned);
    gtk_box_pack_start(GTK_BOX(vbox), right_paned, TRUE, TRUE, 0);
    
    // Create status bar
    GtkWidget* status_bar = ox_editor_create_status_bar(editor);
    gtk_box_pack_start(GTK_BOX(vbox), status_bar, FALSE, FALSE, 0);
    
    // Connect window close signal
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    return window;
}

GtkWidget* ox_editor_create_menu_bar(ox_editor_t* editor) {
    GtkWidget* menu_bar = gtk_menu_bar_new();
    
    // File menu
    GtkWidget* file_menu = gtk_menu_new();
    GtkWidget* file_item = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    
    GtkWidget* new_item = gtk_menu_item_new_with_label("New Scene");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_item);
    g_signal_connect(new_item, "activate", G_CALLBACK(ox_editor_on_file_new), editor);
    
    GtkWidget* open_item = gtk_menu_item_new_with_label("Open Scene");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_item);
    g_signal_connect(open_item, "activate", G_CALLBACK(ox_editor_on_file_open), editor);
    
    GtkWidget* save_item = gtk_menu_item_new_with_label("Save Scene");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);
    g_signal_connect(save_item, "activate", G_CALLBACK(ox_editor_on_file_save), editor);
    
    GtkWidget* save_as_item = gtk_menu_item_new_with_label("Save Scene As...");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_as_item);
    g_signal_connect(save_as_item, "activate", G_CALLBACK(ox_editor_on_file_save_as), editor);
    
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);
    
    // Edit menu
    GtkWidget* edit_menu = gtk_menu_new();
    GtkWidget* edit_item = gtk_menu_item_new_with_label("Edit");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_item), edit_menu);
    
    GtkWidget* undo_item = gtk_menu_item_new_with_label("Undo");
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), undo_item);
    
    GtkWidget* redo_item = gtk_menu_item_new_with_label("Redo");
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), redo_item);
    
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_item);
    
    // View menu
    GtkWidget* view_menu = gtk_menu_new();
    GtkWidget* view_item = gtk_menu_item_new_with_label("View");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_item), view_menu);
    
    GtkWidget* play_item = gtk_menu_item_new_with_label("Play");
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), play_item);
    g_signal_connect(play_item, "activate", G_CALLBACK(ox_editor_on_play), editor);
    
    GtkWidget* stop_item = gtk_menu_item_new_with_label("Stop");
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), stop_item);
    g_signal_connect(stop_item, "activate", G_CALLBACK(ox_editor_on_stop), editor);
    
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), view_item);
    
    return menu_bar;
}

GtkWidget* ox_editor_create_toolbar(ox_editor_t* editor) {
    GtkWidget* toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
    
    // Play button
    GtkToolItem* play_button = gtk_tool_button_new(NULL, "Play");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), play_button, -1);
    g_signal_connect(play_button, "clicked", G_CALLBACK(ox_editor_on_play), editor);
    
    // Stop button
    GtkToolItem* stop_button = gtk_tool_button_new(NULL, "Stop");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), stop_button, -1);
    g_signal_connect(stop_button, "clicked", G_CALLBACK(ox_editor_on_stop), editor);
    
    // Separator
    GtkToolItem* separator = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), separator, -1);
    
    return toolbar;
}

GtkWidget* ox_editor_create_scene_view(ox_editor_t* editor) {
    GtkWidget* frame = gtk_frame_new("Scene View");
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    
    GtkWidget* drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, SCENE_VIEW_WIDTH, SCENE_VIEW_HEIGHT);
    
    // Connect draw signal
    g_signal_connect(drawing_area, "draw", G_CALLBACK(ox_editor_scene_view_draw), editor);
    
    gtk_container_add(GTK_CONTAINER(frame), drawing_area);
    editor->scene_view = drawing_area;
    
    return frame;
}

GtkWidget* ox_editor_create_property_panel(ox_editor_t* editor) {
    GtkWidget* frame = gtk_frame_new("Properties");
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), 
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    // Add some placeholder property widgets
    GtkWidget* label = gtk_label_new("No entity selected");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
    
    gtk_container_add(GTK_CONTAINER(scrolled), vbox);
    gtk_container_add(GTK_CONTAINER(frame), scrolled);
    
    editor->property_panel = vbox;
    
    return frame;
}

GtkWidget* ox_editor_create_hierarchy_panel(ox_editor_t* editor) {
    GtkWidget* frame = gtk_frame_new("Hierarchy");
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    // Add buttons for entity management
    GtkWidget* add_button = gtk_button_new_with_label("Add Entity");
    gtk_box_pack_start(GTK_BOX(vbox), add_button, FALSE, FALSE, 0);
    g_signal_connect(add_button, "clicked", G_CALLBACK(ox_editor_on_entity_selected), editor);
    
    GtkWidget* delete_button = gtk_button_new_with_label("Delete Entity");
    gtk_box_pack_start(GTK_BOX(vbox), delete_button, FALSE, FALSE, 0);
    
    // Add tree view for entities
    GtkWidget* tree_view = gtk_tree_view_new();
    GtkTreeStore* store = gtk_tree_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes("Entities", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    
    gtk_box_pack_start(GTK_BOX(vbox), tree_view, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    
    editor->hierarchy_panel = tree_view;
    
    return frame;
}

GtkWidget* ox_editor_create_status_bar(ox_editor_t* editor) {
    GtkWidget* status_bar = gtk_statusbar_new();
    gtk_statusbar_push(GTK_STATUSBAR(status_bar), 0, "Ready");
    editor->status_bar = status_bar;
    return status_bar;
}

// Scene management functions
void ox_editor_new_scene(ox_editor_t* editor) {
    if (!editor) return;
    
    // Clear current scene
    if (editor->world) {
        ox_scene_clear(editor->world);
    }
    
    editor->selected_entity.value = 0;
    gtk_statusbar_push(GTK_STATUSBAR(editor->status_bar), 0, "New scene created");
}

void ox_editor_save_scene(ox_editor_t* editor, const char* filename) {
    if (!editor || !filename) return;
    
    long result = ox_scene_save(editor->world, filename);
    if (result == OX_SUCCESS) {
        char status_msg[256];
        snprintf(status_msg, sizeof(status_msg), "Scene saved to: %s", filename);
        gtk_statusbar_push(GTK_STATUSBAR(editor->status_bar), 0, status_msg);
    } else {
        gtk_statusbar_push(GTK_STATUSBAR(editor->status_bar), 0, "Failed to save scene");
    }
}

void ox_editor_load_scene(ox_editor_t* editor, const char* filename) {
    if (!editor || !filename) return;
    
    long result = ox_scene_load(editor->world, filename);
    if (result == OX_SUCCESS) {
        char status_msg[256];
        snprintf(status_msg, sizeof(status_msg), "Scene loaded from: %s", filename);
        gtk_statusbar_push(GTK_STATUSBAR(editor->status_bar), 0, status_msg);
    } else {
        gtk_statusbar_push(GTK_STATUSBAR(editor->status_bar), 0, "Failed to load scene");
    }
}

// Entity management functions
ox_entity_id ox_editor_create_entity(ox_editor_t* editor) {
    if (!editor || !editor->world) {
        ox_entity_id empty = {0};
        return empty;
    }
    
    ox_entity_id entity = ox_scene_add_entity(editor->world, "New Entity");
    if (entity.value != 0) {
        gtk_statusbar_push(GTK_STATUSBAR(editor->status_bar), 0, "Entity created");
    }
    return entity;
}

void ox_editor_delete_entity(ox_editor_t* editor, ox_entity_id entity) {
    if (!editor || entity.value == 0) return;
    
    ox_scene_remove_entity(editor->world, entity);
    if (editor->selected_entity.value == entity.value) {
        editor->selected_entity.value = 0;
    }
    gtk_statusbar_push(GTK_STATUSBAR(editor->status_bar), 0, "Entity deleted");
}

void ox_editor_select_entity(ox_editor_t* editor, ox_entity_id entity) {
    if (!editor) return;
    
    editor->selected_entity = entity;
    
    // TODO: Update property panel with entity properties
    gtk_statusbar_push(GTK_STATUSBAR(editor->status_bar), 0, "Entity selected");
}

// Rendering functions
void ox_editor_render_scene(ox_editor_t* editor) {
    if (!editor || !editor->scene_view_initialized) return;
    
    // Render the scene using the scene system
    ox_scene_render(editor->world, editor->scene_texture);
    
    // Add editor overlay
    BeginTextureMode(editor->scene_texture);
    DrawText("OX Game Engine Editor", 10, 10, 20, WHITE);
    DrawText("Scene View", 10, 40, 16, GRAY);
    EndTextureMode();
}

gboolean ox_editor_scene_view_draw(GtkWidget* widget, cairo_t* cr, gpointer user_data) {
    ox_editor_t* editor = (ox_editor_t*)user_data;
    
    if (!editor) return FALSE;
    
    // Initialize scene view if not done yet
    if (!editor->scene_view_initialized) {
        editor->scene_view_initialized = true;
        // TODO: Initialize raylib context for scene view
    }
    
    // Render the scene
    ox_editor_render_scene(editor);
    
    // Draw the rendered texture to the GTK widget
    // TODO: Convert raylib texture to cairo surface and draw it
    
    return TRUE;
}

// Callback functions
void ox_editor_on_file_new(GtkWidget* widget, gpointer user_data) {
    ox_editor_t* editor = (ox_editor_t*)user_data;
    ox_editor_new_scene(editor);
}

void ox_editor_on_file_open(GtkWidget* widget, gpointer user_data) {
    ox_editor_t* editor = (ox_editor_t*)user_data;
    
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Open Scene",
                                                    GTK_WINDOW(editor->main_window),
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    
    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "OX Scene Files (*.oxs)");
    gtk_file_filter_add_pattern(filter, "*.oxs");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        ox_editor_load_scene(editor, filename);
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

void ox_editor_on_file_save(GtkWidget* widget, gpointer user_data) {
    ox_editor_t* editor = (ox_editor_t*)user_data;
    ox_editor_save_scene(editor, "untitled.oxs");
}

void ox_editor_on_file_save_as(GtkWidget* widget, gpointer user_data) {
    ox_editor_t* editor = (ox_editor_t*)user_data;
    
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Save Scene As",
                                                    GTK_WINDOW(editor->main_window),
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Save", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    
    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "OX Scene Files (*.oxs)");
    gtk_file_filter_add_pattern(filter, "*.oxs");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        ox_editor_save_scene(editor, filename);
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

void ox_editor_on_play(GtkWidget* widget, gpointer user_data) {
    ox_editor_t* editor = (ox_editor_t*)user_data;
    editor->is_playing = true;
    gtk_statusbar_push(GTK_STATUSBAR(editor->status_bar), 0, "Playing...");
}

void ox_editor_on_stop(GtkWidget* widget, gpointer user_data) {
    ox_editor_t* editor = (ox_editor_t*)user_data;
    editor->is_playing = false;
    gtk_statusbar_push(GTK_STATUSBAR(editor->status_bar), 0, "Stopped");
}

void ox_editor_on_entity_selected(GtkWidget* widget, gpointer user_data) {
    ox_editor_t* editor = (ox_editor_t*)user_data;
    ox_entity_id entity = ox_editor_create_entity(editor);
    ox_editor_select_entity(editor, entity);
}