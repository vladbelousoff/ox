#include "ox_editor.h"
#include "../code/ox_log.h"
#include "../code/ox_memory.h"

#include <string.h>

// Helper function to create a property row
static GtkWidget* create_property_row(const char* label, GtkWidget* value_widget)
{
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_left(hbox, 10);
    gtk_widget_set_margin_right(hbox, 10);
    gtk_widget_set_margin_top(hbox, 5);
    gtk_widget_set_margin_bottom(hbox, 5);
    
    GtkWidget* label_widget = gtk_label_new(label);
    gtk_widget_set_size_request(label_widget, 100, -1);
    gtk_widget_set_halign(label_widget, GTK_ALIGN_START);
    
    gtk_box_pack_start(GTK_BOX(hbox), label_widget, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), value_widget, TRUE, TRUE, 0);
    
    return hbox;
}

// Helper function to create a section header
static GtkWidget* create_section_header(const char* title)
{
    GtkWidget* label = gtk_label_new(title);
    gtk_widget_set_margin_top(label, 10);
    gtk_widget_set_margin_bottom(label, 5);
    gtk_widget_set_margin_left(label, 5);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    
    // Make it bold
    PangoAttrList* attrs = pango_attr_list_new();
    PangoAttribute* bold = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
    pango_attr_list_insert(attrs, bold);
    gtk_label_set_attributes(GTK_LABEL(label), attrs);
    pango_attr_list_unref(attrs);
    
    return label;
}

// Create transform component UI
static void add_transform_component(ox_editor_properties_t* properties)
{
    // Transform section header
    GtkWidget* transform_header = create_section_header("Transform");
    gtk_box_pack_start(GTK_BOX(properties->properties_box), transform_header, FALSE, FALSE, 0);
    
    // Position
    GtkWidget* pos_x_entry = gtk_entry_new();
    GtkWidget* pos_y_entry = gtk_entry_new();
    GtkWidget* pos_z_entry = gtk_entry_new();
    
    gtk_entry_set_text(GTK_ENTRY(pos_x_entry), "0.0");
    gtk_entry_set_text(GTK_ENTRY(pos_y_entry), "0.0");
    gtk_entry_set_text(GTK_ENTRY(pos_z_entry), "0.0");
    
    GtkWidget* pos_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(pos_box), pos_x_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(pos_box), pos_y_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(pos_box), pos_z_entry, TRUE, TRUE, 0);
    
    GtkWidget* pos_row = create_property_row("Position:", pos_box);
    gtk_box_pack_start(GTK_BOX(properties->properties_box), pos_row, FALSE, FALSE, 0);
    
    // Rotation
    GtkWidget* rot_x_entry = gtk_entry_new();
    GtkWidget* rot_y_entry = gtk_entry_new();
    GtkWidget* rot_z_entry = gtk_entry_new();
    
    gtk_entry_set_text(GTK_ENTRY(rot_x_entry), "0.0");
    gtk_entry_set_text(GTK_ENTRY(rot_y_entry), "0.0");
    gtk_entry_set_text(GTK_ENTRY(rot_z_entry), "0.0");
    
    GtkWidget* rot_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(rot_box), rot_x_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(rot_box), rot_y_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(rot_box), rot_z_entry, TRUE, TRUE, 0);
    
    GtkWidget* rot_row = create_property_row("Rotation:", rot_box);
    gtk_box_pack_start(GTK_BOX(properties->properties_box), rot_row, FALSE, FALSE, 0);
    
    // Scale
    GtkWidget* scale_x_entry = gtk_entry_new();
    GtkWidget* scale_y_entry = gtk_entry_new();
    GtkWidget* scale_z_entry = gtk_entry_new();
    
    gtk_entry_set_text(GTK_ENTRY(scale_x_entry), "1.0");
    gtk_entry_set_text(GTK_ENTRY(scale_y_entry), "1.0");
    gtk_entry_set_text(GTK_ENTRY(scale_z_entry), "1.0");
    
    GtkWidget* scale_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(scale_box), scale_x_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(scale_box), scale_y_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(scale_box), scale_z_entry, TRUE, TRUE, 0);
    
    GtkWidget* scale_row = create_property_row("Scale:", scale_box);
    gtk_box_pack_start(GTK_BOX(properties->properties_box), scale_row, FALSE, FALSE, 0);
}

// Create render component UI
static void add_render_component(ox_editor_properties_t* properties)
{
    // Render section header
    GtkWidget* render_header = create_section_header("Render");
    gtk_box_pack_start(GTK_BOX(properties->properties_box), render_header, FALSE, FALSE, 0);
    
    // Visible checkbox
    GtkWidget* visible_check = gtk_check_button_new_with_label("Visible");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(visible_check), TRUE);
    GtkWidget* visible_row = create_property_row("", visible_check);
    gtk_box_pack_start(GTK_BOX(properties->properties_box), visible_row, FALSE, FALSE, 0);
    
    // Color picker
    GtkWidget* color_button = gtk_color_button_new();
    GdkRGBA color = {1.0, 1.0, 1.0, 1.0}; // White
    gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(color_button), &color);
    
    GtkWidget* color_row = create_property_row("Color:", color_button);
    gtk_box_pack_start(GTK_BOX(properties->properties_box), color_row, FALSE, FALSE, 0);
}

ox_editor_properties_t* ox_editor_properties_create(ox_editor_window_t* window)
{
    if (!window) {
        return NULL;
    }
    
    ox_editor_properties_t* properties = ox_mem_acquire(sizeof(ox_editor_properties_t), OX_SOURCE_LOCATION);
    if (!properties) {
        OX_LOG_ERR("Failed to allocate properties panel");
        return NULL;
    }
    
    // Create main container
    properties->container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(properties->container, -1, 300);
    
    // Create title label
    GtkWidget* title_label = gtk_label_new("Properties");
    gtk_widget_set_margin_top(title_label, 5);
    gtk_widget_set_margin_bottom(title_label, 5);
    gtk_box_pack_start(GTK_BOX(properties->container), title_label, FALSE, FALSE, 0);
    
    // Create scrolled window
    properties->scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(properties->scrolled_window),
                                  GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(properties->container), properties->scrolled_window, TRUE, TRUE, 0);
    
    // Create properties box (will contain all property widgets)
    properties->properties_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(properties->scrolled_window), properties->properties_box);
    
    // Initialize with no entity selected
    properties->current_entity.value = 0;
    
    // Add placeholder message
    GtkWidget* placeholder_label = gtk_label_new("No entity selected");
    gtk_widget_set_margin_top(placeholder_label, 20);
    gtk_widget_set_sensitive(placeholder_label, FALSE);
    gtk_box_pack_start(GTK_BOX(properties->properties_box), placeholder_label, FALSE, FALSE, 0);
    
    OX_LOG_DBG("Properties panel created");
    return properties;
}

void ox_editor_properties_destroy(ox_editor_properties_t* properties)
{
    if (!properties) {
        return;
    }
    
    // GTK will handle widget cleanup
    ox_mem_release(properties);
    OX_LOG_DBG("Properties panel destroyed");
}

void ox_editor_properties_set_entity(ox_editor_properties_t* properties, ox_entity_id entity)
{
    if (!properties) {
        return;
    }
    
    properties->current_entity = entity;
    
    // Clear existing properties
    GList* children = gtk_container_get_children(GTK_CONTAINER(properties->properties_box));
    for (GList* iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    
    if (entity.value == 0) {
        // No entity selected
        GtkWidget* placeholder_label = gtk_label_new("No entity selected");
        gtk_widget_set_margin_top(placeholder_label, 20);
        gtk_widget_set_sensitive(placeholder_label, FALSE);
        gtk_box_pack_start(GTK_BOX(properties->properties_box), placeholder_label, FALSE, FALSE, 0);
    } else {
        // Entity selected - show properties
        
        // Entity info section
        GtkWidget* entity_header = create_section_header("Entity");
        gtk_box_pack_start(GTK_BOX(properties->properties_box), entity_header, FALSE, FALSE, 0);
        
        // Entity ID (read-only)
        GtkWidget* id_entry = gtk_entry_new();
        char id_text[32];
        snprintf(id_text, sizeof(id_text), "%lu", entity.value);
        gtk_entry_set_text(GTK_ENTRY(id_entry), id_text);
        gtk_widget_set_sensitive(id_entry, FALSE);
        
        GtkWidget* id_row = create_property_row("ID:", id_entry);
        gtk_box_pack_start(GTK_BOX(properties->properties_box), id_row, FALSE, FALSE, 0);
        
        // Entity name
        GtkWidget* name_entry = gtk_entry_new();
        char name_text[64];
        snprintf(name_text, sizeof(name_text), "Entity %lu", entity.value);
        gtk_entry_set_text(GTK_ENTRY(name_entry), name_text);
        
        GtkWidget* name_row = create_property_row("Name:", name_entry);
        gtk_box_pack_start(GTK_BOX(properties->properties_box), name_row, FALSE, FALSE, 0);
        
        // Add default components (for demonstration)
        add_transform_component(properties);
        add_render_component(properties);
    }
    
    // Show all new widgets
    gtk_widget_show_all(properties->properties_box);
    
    OX_LOG_DBG("Properties panel updated for entity %lu", entity.value);
}