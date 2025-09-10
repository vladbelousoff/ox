#include "ox_editor.h"
#include "../code/ox_log.h"
#include "../code/ox_memory.h"

// Global reference to current editor window for inter-panel communication
static ox_editor_window_t* g_current_window = NULL;

ox_editor_window_t* get_current_editor_window(void)
{
    return g_current_window;
}

// Menu callbacks
static void on_file_new(GtkMenuItem* menuitem, gpointer user_data)
{
    OX_LOG_DBG("File -> New clicked");
    // TODO: Implement new scene functionality
}

static void on_file_open(GtkMenuItem* menuitem, gpointer user_data)
{
    OX_LOG_DBG("File -> Open clicked");
    // TODO: Implement open scene functionality
}

static void on_file_save(GtkMenuItem* menuitem, gpointer user_data)
{
    OX_LOG_DBG("File -> Save clicked");
    // TODO: Implement save scene functionality
}

static void on_file_quit(GtkMenuItem* menuitem, gpointer user_data)
{
    ox_editor_t* editor = (ox_editor_t*)user_data;
    OX_LOG_DBG("File -> Quit clicked");
    g_application_quit(G_APPLICATION(editor->app));
}

static void on_entity_create(GtkMenuItem* menuitem, gpointer user_data)
{
    OX_LOG_DBG("Entity -> Create clicked");
    // TODO: Implement entity creation
}

static void on_entity_delete(GtkMenuItem* menuitem, gpointer user_data)
{
    OX_LOG_DBG("Entity -> Delete clicked");
    // TODO: Implement entity deletion
}

// Create menu bar
static GtkWidget* create_menu_bar(ox_editor_t* editor)
{
    GtkWidget* menubar = gtk_menu_bar_new();
    
    // File menu
    GtkWidget* file_menu = gtk_menu_new();
    GtkWidget* file_item = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_item);
    
    GtkWidget* new_item = gtk_menu_item_new_with_label("New");
    GtkWidget* open_item = gtk_menu_item_new_with_label("Open");
    GtkWidget* save_item = gtk_menu_item_new_with_label("Save");
    GtkWidget* separator = gtk_separator_menu_item_new();
    GtkWidget* quit_item = gtk_menu_item_new_with_label("Quit");
    
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), separator);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);
    
    g_signal_connect(new_item, "activate", G_CALLBACK(on_file_new), editor);
    g_signal_connect(open_item, "activate", G_CALLBACK(on_file_open), editor);
    g_signal_connect(save_item, "activate", G_CALLBACK(on_file_save), editor);
    g_signal_connect(quit_item, "activate", G_CALLBACK(on_file_quit), editor);
    
    // Entity menu
    GtkWidget* entity_menu = gtk_menu_new();
    GtkWidget* entity_item = gtk_menu_item_new_with_label("Entity");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(entity_item), entity_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), entity_item);
    
    GtkWidget* create_entity_item = gtk_menu_item_new_with_label("Create Entity");
    GtkWidget* delete_entity_item = gtk_menu_item_new_with_label("Delete Entity");
    
    gtk_menu_shell_append(GTK_MENU_SHELL(entity_menu), create_entity_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(entity_menu), delete_entity_item);
    
    g_signal_connect(create_entity_item, "activate", G_CALLBACK(on_entity_create), editor);
    g_signal_connect(delete_entity_item, "activate", G_CALLBACK(on_entity_delete), editor);
    
    return menubar;
}

// Create toolbar
static GtkWidget* create_toolbar(ox_editor_t* editor)
{
    GtkWidget* toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
    
    // Play button
    GtkToolItem* play_button = gtk_tool_button_new(NULL, "Play");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(play_button), "media-playback-start");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), play_button, -1);
    
    // Stop button  
    GtkToolItem* stop_button = gtk_tool_button_new(NULL, "Stop");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(stop_button), "media-playback-stop");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), stop_button, -1);
    
    // Separator
    GtkToolItem* separator = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), separator, -1);
    
    // Create Entity button
    GtkToolItem* create_button = gtk_tool_button_new(NULL, "Create Entity");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(create_button), "list-add");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), create_button, -1);
    
    g_signal_connect(create_button, "clicked", G_CALLBACK(on_entity_create), editor);
    
    return toolbar;
}

// Window delete event callback
static gboolean on_window_delete_event(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
    ox_editor_t* editor = (ox_editor_t*)user_data;
    g_application_quit(G_APPLICATION(editor->app));
    return FALSE;
}

ox_editor_window_t* ox_editor_window_create(ox_editor_t* editor)
{
    if (!editor) {
        return NULL;
    }
    
    ox_editor_window_t* window = ox_mem_acquire(sizeof(ox_editor_window_t), OX_SOURCE_LOCATION);
    if (!window) {
        OX_LOG_ERR("Failed to allocate editor window");
        return NULL;
    }
    
    // Create main window
    window->window = gtk_application_window_new(editor->app);
    gtk_window_set_title(GTK_WINDOW(window->window), "Ox Game Engine Editor");
    gtk_window_set_default_size(GTK_WINDOW(window->window), 1200, 800);
    gtk_window_set_position(GTK_WINDOW(window->window), GTK_WIN_POS_CENTER);
    
    // Connect window signals
    g_signal_connect(window->window, "delete-event", G_CALLBACK(on_window_delete_event), editor);
    
    // Create main container (vertical box)
    window->main_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window->window), window->main_container);
    
    // Create menu bar
    window->menubar = create_menu_bar(editor);
    gtk_box_pack_start(GTK_BOX(window->main_container), window->menubar, FALSE, FALSE, 0);
    
    // Create toolbar
    window->toolbar = create_toolbar(editor);
    gtk_box_pack_start(GTK_BOX(window->main_container), window->toolbar, FALSE, FALSE, 0);
    
    // Create horizontal paned (main content area)
    window->paned_horizontal = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(window->main_container), window->paned_horizontal, TRUE, TRUE, 0);
    
    // Create vertical paned (right side)
    window->paned_vertical = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    gtk_paned_pack2(GTK_PANED(window->paned_horizontal), window->paned_vertical, TRUE, FALSE);
    
    // Create panels
    window->hierarchy = ox_editor_hierarchy_create(window);
    window->viewport = ox_editor_viewport_create(window);
    window->properties = ox_editor_properties_create(window);
    
    if (!window->hierarchy || !window->viewport || !window->properties) {
        OX_LOG_ERR("Failed to create editor panels");
        ox_editor_window_destroy(window);
        return NULL;
    }
    
    // Layout panels
    gtk_paned_pack1(GTK_PANED(window->paned_horizontal), window->hierarchy->container, FALSE, FALSE);
    gtk_paned_pack1(GTK_PANED(window->paned_vertical), window->viewport->container, TRUE, FALSE);
    gtk_paned_pack2(GTK_PANED(window->paned_vertical), window->properties->container, FALSE, FALSE);
    
    // Set paned positions
    gtk_paned_set_position(GTK_PANED(window->paned_horizontal), 250);
    gtk_paned_set_position(GTK_PANED(window->paned_vertical), 500);
    
    // Set global reference
    g_current_window = window;
    
    OX_LOG_DBG("Editor window created successfully");
    return window;
}

void ox_editor_window_destroy(ox_editor_window_t* window)
{
    if (!window) {
        return;
    }
    
    // Clear global reference
    if (g_current_window == window) {
        g_current_window = NULL;
    }
    
    // Cleanup panels
    if (window->hierarchy) {
        ox_editor_hierarchy_destroy(window->hierarchy);
    }
    if (window->viewport) {
        ox_editor_viewport_destroy(window->viewport);
    }
    if (window->properties) {
        ox_editor_properties_destroy(window->properties);
    }
    
    // Window will be destroyed by GTK
    ox_mem_release(window);
    
    OX_LOG_DBG("Editor window destroyed");
}