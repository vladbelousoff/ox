#include "ox_render.h"

#include "raylib.h"

long ox_render_init(void)
{
  InitWindow(800, 600, "OX");
  SetTargetFPS(60);

  return 0;
}

void ox_render_exit(void)
{
  CloseWindow();
}