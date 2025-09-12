#include "ox_render.h"

#include "ox_core.h"

#include "raylib.h"

static Font current_font;
static bool font_loaded = false;

long ox_render_init(void)
{
  InitWindow(1920, 1080, "OX");
  ClearWindowState(FLAG_VSYNC_HINT);
  // SetTargetFPS(60);

  // Try to load a system font first (common monospace fonts)
  static const char* font_paths[] = {
    "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
    "/usr/share/fonts/TTF/Hack-Regular.ttf",
    "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
    "/System/Library/Fonts/Monaco.ttf", // macOS
    "C:/Windows/Fonts/consola.ttf",     // Windows
    ""                                  // Fallback to default
  };

  current_font = (Font){ 0 };
  for (int i = 0; i < 4; ++i) {
    if (FileExists(font_paths[i])) {
      current_font =
        LoadFontEx(font_paths[i], ox_render_get_font_size(), 0, 250);
      if (current_font.texture.id != 0) {
        break;
      }
    }
  }

  // If no custom font was loaded, use the default font
  if (current_font.texture.id == 0) {
    current_font = GetFontDefault();
  }

  font_loaded = true;

  return OX_SUCCESS;
}

void ox_render_exit(void)
{
  if (font_loaded && current_font.texture.id != GetFontDefault().texture.id) {
    UnloadFont(current_font);
  }
  CloseWindow();
}

Font ox_render_get_default_font(void)
{
  return GetFontDefault();
}

Font ox_render_get_current_font(void)
{
  return font_loaded ? current_font : GetFontDefault();
}

int ox_render_get_font_size(void)
{
  return 24;
}

void ox_render_set_font(const Font font)
{
  if (font_loaded && current_font.texture.id != GetFontDefault().texture.id) {
    UnloadFont(current_font);
  }

  current_font = font;
  font_loaded = true;
}

void ox_render_draw_text(const char* text, const int posX, const int posY,
                         const int fontSize, const Color color)
{
  if (font_loaded) {
    DrawTextEx(current_font, text, (Vector2){ (float)posX, (float)posY },
               (float)fontSize, 1.0f, color);
  } else {
    DrawText(text, posX, posY, fontSize, color);
  }
}

void ox_render_draw_text_ex(const Font font, const char* text,
                            const Vector2 position, const float fontSize,
                            const float spacing, const Color tint)
{
  DrawTextEx(font, text, position, fontSize, spacing, tint);
}