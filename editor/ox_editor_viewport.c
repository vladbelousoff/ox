#include "ox_editor.h"
#include "../code/ox_log.h"
#include "../code/ox_memory.h"

#include <raylib.h>
#include <math.h>

// Viewport toolbar callbacks
static void on_viewport_play_clicked(GtkButton* button, gpointer user_data)
{
    OX_LOG_DBG("Viewport play button clicked");
    // TODO: Start game simulation
}

static void on_viewport_stop_clicked(GtkButton* button, gpointer user_data)
{
    OX_LOG_DBG("Viewport stop button clicked");
    // TODO: Stop game simulation
}

static void on_viewport_reset_clicked(GtkButton* button, gpointer user_data)
{
    OX_LOG_DBG("Viewport reset button clicked");
    // TODO: Reset viewport camera
}

// Create viewport toolbar
static GtkWidget* create_viewport_toolbar(ox_editor_viewport_t* viewport)
{
    GtkWidget* toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
    gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_SMALL_TOOLBAR);
    
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
    
    // Reset view button
    GtkToolItem* reset_button = gtk_tool_button_new(NULL, "Reset View");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(reset_button), "view-refresh");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), reset_button, -1);
    
    g_signal_connect(play_button, "clicked", G_CALLBACK(on_viewport_play_clicked), viewport);
    g_signal_connect(stop_button, "clicked", G_CALLBACK(on_viewport_stop_clicked), viewport);
    g_signal_connect(reset_button, "clicked", G_CALLBACK(on_viewport_reset_clicked), viewport);
    
    return toolbar;
}

// Drawing area realize callback - initialize Raylib
static void on_drawing_area_realize(GtkWidget* widget, gpointer user_data)
{
    ox_editor_viewport_t* viewport = (ox_editor_viewport_t*)user_data;
    
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);
    
    viewport->width = allocation.width;
    viewport->height = allocation.height;
    
    // Note: In a real implementation, we would need to properly integrate
    // Raylib with GTK's OpenGL context. For now, we'll simulate this.
    viewport->is_initialized = true;
    
    OX_LOG_DBG("Viewport drawing area realized: %dx%d", viewport->width, viewport->height);
}

// Drawing area draw callback
static gboolean on_drawing_area_draw(GtkWidget* widget, cairo_t* cr, gpointer user_data)
{
    ox_editor_viewport_t* viewport = (ox_editor_viewport_t*)user_data;
    
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);
    
    // Update dimensions if changed
    if (allocation.width != viewport->width || allocation.height != viewport->height) {
        viewport->width = allocation.width;
        viewport->height = allocation.height;
    }
    
    // Clear background with dark color (editor-like)
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
    cairo_rectangle(cr, 0, 0, allocation.width, allocation.height);
    cairo_fill(cr);
    
    // Draw grid
    cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
    cairo_set_line_width(cr, 1.0);
    
    int grid_size = 50;
    for (int x = 0; x < allocation.width; x += grid_size) {
        cairo_move_to(cr, x, 0);
        cairo_line_to(cr, x, allocation.height);
        cairo_stroke(cr);
    }
    
    for (int y = 0; y < allocation.height; y += grid_size) {
        cairo_move_to(cr, 0, y);
        cairo_line_to(cr, allocation.width, y);
        cairo_stroke(cr);
    }
    
    // Draw center axes
    cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);
    cairo_set_line_width(cr, 2.0);
    
    int center_x = allocation.width / 2;
    int center_y = allocation.height / 2;
    
    // X axis (red)
    cairo_set_source_rgb(cr, 1.0, 0.3, 0.3);
    cairo_move_to(cr, center_x, center_y);
    cairo_line_to(cr, center_x + 50, center_y);
    cairo_stroke(cr);
    
    // Y axis (green)
    cairo_set_source_rgb(cr, 0.3, 1.0, 0.3);
    cairo_move_to(cr, center_x, center_y);
    cairo_line_to(cr, center_x, center_y - 50);
    cairo_stroke(cr);
    
    // Draw some sample entities (circles)
    cairo_set_source_rgb(cr, 0.8, 0.8, 0.2);
    
    static float time = 0.0f;
    time += 0.016f; // Simulate 60 FPS
    
    for (int i = 0; i < 5; ++i) {
        float angle = time + i * 1.2f;
        float x = center_x + cos(angle) * (100 + i * 20);
        float y = center_y + sin(angle) * (50 + i * 10);
        
        cairo_arc(cr, x, y, 10, 0, 2 * M_PI);
        cairo_fill(cr);
    }
    
    // Draw viewport info
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 12);
    
    char info_text[128];
    snprintf(info_text, sizeof(info_text), "Viewport: %dx%d", allocation.width, allocation.height);
    cairo_move_to(cr, 10, 20);
    cairo_show_text(cr, info_text);
    
    // Request redraw for animation
    gtk_widget_queue_draw(widget);
    
    return TRUE;
}

// Mouse button press callback
static gboolean on_drawing_area_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
    ox_editor_viewport_t* viewport = (ox_editor_viewport_t*)user_data;
    
    if (event->button == 1) { // Left click
        OX_LOG_DBG("Viewport left click at (%.1f, %.1f)", event->x, event->y);
        // TODO: Handle object selection, placement, etc.
    } else if (event->button == 3) { // Right click
        OX_LOG_DBG("Viewport right click at (%.1f, %.1f)", event->x, event->y);
        // TODO: Show context menu
    }
    
    return TRUE;
}

ox_editor_viewport_t* ox_editor_viewport_create(ox_editor_window_t* window)
{
    if (!window) {
        return NULL;
    }
    
    ox_editor_viewport_t* viewport = ox_mem_acquire(sizeof(ox_editor_viewport_t), OX_SOURCE_LOCATION);
    if (!viewport) {
        OX_LOG_ERR("Failed to allocate viewport");
        return NULL;
    }
    
    // Create main container
    viewport->container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    // Create title label
    GtkWidget* title_label = gtk_label_new("Viewport");
    gtk_widget_set_margin_top(title_label, 5);
    gtk_widget_set_margin_bottom(title_label, 5);
    gtk_box_pack_start(GTK_BOX(viewport->container), title_label, FALSE, FALSE, 0);
    
    // Create toolbar
    viewport->toolbar = create_viewport_toolbar(viewport);
    gtk_box_pack_start(GTK_BOX(viewport->container), viewport->toolbar, FALSE, FALSE, 0);
    
    // Create drawing area
    viewport->drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(viewport->drawing_area, 640, 480);
    gtk_widget_set_can_focus(viewport->drawing_area, TRUE);
    
    // Add event masks
    gtk_widget_add_events(viewport->drawing_area, 
                         GDK_BUTTON_PRESS_MASK | 
                         GDK_BUTTON_RELEASE_MASK |
                         GDK_POINTER_MOTION_MASK |
                         GDK_SCROLL_MASK);
    
    // Connect signals
    g_signal_connect(viewport->drawing_area, "realize", G_CALLBACK(on_drawing_area_realize), viewport);
    g_signal_connect(viewport->drawing_area, "draw", G_CALLBACK(on_drawing_area_draw), viewport);
    g_signal_connect(viewport->drawing_area, "button-press-event", G_CALLBACK(on_drawing_area_button_press), viewport);
    
    // Add drawing area to container
    gtk_box_pack_start(GTK_BOX(viewport->container), viewport->drawing_area, TRUE, TRUE, 0);
    
    // Initialize state
    viewport->is_initialized = false;
    viewport->width = 640;
    viewport->height = 480;
    
    OX_LOG_DBG("Viewport created");
    return viewport;
}

void ox_editor_viewport_destroy(ox_editor_viewport_t* viewport)
{
    if (!viewport) {
        return;
    }
    
    // GTK will handle widget cleanup
    ox_mem_release(viewport);
    OX_LOG_DBG("Viewport destroyed");
}

void ox_editor_viewport_render(ox_editor_viewport_t* viewport)
{
    if (!viewport || !viewport->is_initialized) {
        return;
    }
    
    // In a real implementation, this would render the game scene
    // using Raylib or another graphics library
    
    // For now, we trigger a redraw of the GTK drawing area
    if (viewport->drawing_area) {
        gtk_widget_queue_draw(viewport->drawing_area);
    }
}