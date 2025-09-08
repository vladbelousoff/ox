#include "ox_render.h"

#include "ox_core.h"

#include "raylib.h"

long ox_render_init(void)
{
  InitWindow(1920, 1080, "OX");
  ClearWindowState(FLAG_VSYNC_HINT);
  // SetTargetFPS(60);

  return OX_SUCCESS;
}

void ox_render_exit(void)
{
  CloseWindow();
}