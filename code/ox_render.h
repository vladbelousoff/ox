#pragma once

#include "raylib.h"

long ox_render_init(void);
void ox_render_exit(void);

// Font management functions
Font ox_render_get_default_font(void);
Font ox_render_get_current_font(void);
void ox_render_set_font(Font font);

// Text drawing functions with font support
void ox_render_draw_text(const char* text, int posX, int posY, int fontSize, Color color);
void ox_render_draw_text_ex(Font font, const char* text, Vector2 position, float fontSize, float spacing, Color tint);