#include "ox_editor.h"
#include "../code/ox_log.h"
#include "../code/ox_memory.h"

#include <string.h>

// Tree view columns
enum {
    COLUMN_NAME = 0,
    COLUMN_ENTITY_ID,
    N_COLUMNS
};

// Forward declaration to access editor window
extern ox_editor_window_t* get_current_editor_window(void);

// Selection changed callback
static void on_selection_changed(GtkTreeSelection* selection, gpointer user_data)
{
    ox_editor_hierarchy_t* hierarchy = (ox_editor_hierarchy_t*)user_data;
    GtkTreeModel* model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        guint64 entity_value;
        gtk_tree_model_get(model, &iter, COLUMN_ENTITY_ID, &entity_value, -1);
        
        ox_entity_id entity;
        entity.value = entity_value;
        hierarchy->selected_entity = entity;
        
        OX_LOG_DBG("Selected entity: %lu", entity.value);
        
        // Update properties panel with selected entity
        ox_editor_window_t* window = get_current_editor_window();
        if (window && window->properties) {
            ox_editor_properties_set_entity(window->properties, entity);
        }
    } else {
        // No selection
        hierarchy->selected_entity.value = 0;
        
        // Clear properties panel
        ox_editor_window_t* window = get_current_editor_window();
        if (window && window->properties) {
            ox_entity_id empty_entity = {0};
            ox_editor_properties_set_entity(window->properties, empty_entity);
        }
    }
}

// Toolbar button callbacks
static void on_create_entity_clicked(GtkButton* button, gpointer user_data)
{
    ox_editor_hierarchy_t* hierarchy = (ox_editor_hierarchy_t*)user_data;
    OX_LOG_DBG("Create entity button clicked");
    
    // TODO: Create new entity and add to tree
    // For now, add a placeholder entity
    GtkTreeIter iter;
    gtk_tree_store_append(hierarchy->tree_store, &iter, NULL);
    
    static int entity_counter = 1;
    char entity_name[64];
    snprintf(entity_name, sizeof(entity_name), "Entity %d", entity_counter++);
    
    ox_entity_id dummy_entity;
    dummy_entity.value = entity_counter;
    
    gtk_tree_store_set(hierarchy->tree_store, &iter,
                      COLUMN_NAME, entity_name,
                      COLUMN_ENTITY_ID, dummy_entity.value,
                      -1);
}

static void on_delete_entity_clicked(GtkButton* button, gpointer user_data)
{
    ox_editor_hierarchy_t* hierarchy = (ox_editor_hierarchy_t*)user_data;
    GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(hierarchy->tree_view));
    GtkTreeModel* model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        OX_LOG_DBG("Delete entity button clicked");
        gtk_tree_store_remove(hierarchy->tree_store, &iter);
        hierarchy->selected_entity.value = 0;
    }
}

// Create hierarchy toolbar
static GtkWidget* create_hierarchy_toolbar(ox_editor_hierarchy_t* hierarchy)
{
    GtkWidget* toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
    gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_SMALL_TOOLBAR);
    
    // Create entity button
    GtkToolItem* create_button = gtk_tool_button_new(NULL, "Create");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(create_button), "list-add");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), create_button, -1);
    
    // Delete entity button
    GtkToolItem* delete_button = gtk_tool_button_new(NULL, "Delete");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(delete_button), "list-remove");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), delete_button, -1);
    
    g_signal_connect(create_button, "clicked", G_CALLBACK(on_create_entity_clicked), hierarchy);
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_entity_clicked), hierarchy);
    
    return toolbar;
}

ox_editor_hierarchy_t* ox_editor_hierarchy_create(ox_editor_window_t* window)
{
    if (!window) {
        return NULL;
    }
    
    ox_editor_hierarchy_t* hierarchy = ox_mem_acquire(sizeof(ox_editor_hierarchy_t), OX_SOURCE_LOCATION);
    if (!hierarchy) {
        OX_LOG_ERR("Failed to allocate hierarchy panel");
        return NULL;
    }
    
    // Create main container
    hierarchy->container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(hierarchy->container, 250, -1);
    
    // Create title label
    GtkWidget* title_label = gtk_label_new("Scene Hierarchy");
    gtk_widget_set_margin_top(title_label, 5);
    gtk_widget_set_margin_bottom(title_label, 5);
    gtk_box_pack_start(GTK_BOX(hierarchy->container), title_label, FALSE, FALSE, 0);
    
    // Create toolbar
    hierarchy->toolbar = create_hierarchy_toolbar(hierarchy);
    gtk_box_pack_start(GTK_BOX(hierarchy->container), hierarchy->toolbar, FALSE, FALSE, 0);
    
    // Create tree store
    hierarchy->tree_store = gtk_tree_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_UINT64);
    
    // Create tree view
    hierarchy->tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(hierarchy->tree_store));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(hierarchy->tree_view), FALSE);
    
    // Create column
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes("Entity", renderer,
                                                                        "text", COLUMN_NAME,
                                                                        NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(hierarchy->tree_view), column);
    
    // Set up selection
    GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(hierarchy->tree_view));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    g_signal_connect(selection, "changed", G_CALLBACK(on_selection_changed), hierarchy);
    
    // Create scrolled window for tree view
    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), hierarchy->tree_view);
    
    gtk_box_pack_start(GTK_BOX(hierarchy->container), scrolled_window, TRUE, TRUE, 0);
    
    // Initialize selected entity
    hierarchy->selected_entity.value = 0;
    
    OX_LOG_DBG("Scene hierarchy panel created");
    return hierarchy;
}

void ox_editor_hierarchy_destroy(ox_editor_hierarchy_t* hierarchy)
{
    if (!hierarchy) {
        return;
    }
    
    // GTK will handle widget cleanup
    if (hierarchy->tree_store) {
        g_object_unref(hierarchy->tree_store);
    }
    
    ox_mem_release(hierarchy);
    OX_LOG_DBG("Scene hierarchy panel destroyed");
}

void ox_editor_hierarchy_refresh(ox_editor_hierarchy_t* hierarchy, ox_world_t* world)
{
    if (!hierarchy || !world) {
        return;
    }
    
    // Clear existing entries
    gtk_tree_store_clear(hierarchy->tree_store);
    
    // Add entities from world
    for (size_t i = 0; i < world->entities_count; ++i) {
        GtkTreeIter iter;
        gtk_tree_store_append(hierarchy->tree_store, &iter, NULL);
        
        char entity_name[64];
        snprintf(entity_name, sizeof(entity_name), "Entity %lu", world->entities[i].value);
        
        gtk_tree_store_set(hierarchy->tree_store, &iter,
                          COLUMN_NAME, entity_name,
                          COLUMN_ENTITY_ID, world->entities[i].value,
                          -1);
    }
    
    OX_LOG_DBG("Scene hierarchy refreshed with %zu entities", world->entities_count);
}