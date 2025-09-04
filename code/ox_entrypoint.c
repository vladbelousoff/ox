#include "ox_core.h"
#include "ox_log.h"
#include "ox_memory.h"
#include "ox_render.h"
#include "raylib.h"

#include <math.h>
#include <stdio.h>

typedef struct {
  long (*init)(void);
  void (*free)(void);
  const char* name;
} ox_subsystem_t;

static ox_subsystem_t subsystems[] = {
  { ox_memory_init, ox_memory_exit, "Memory" },
  { ox_render_init, ox_render_exit, "Render" },
};

static void systems_exit_starting_from(const int index)
{
  for (int i = index; i >= 0; --i) {
    OX_LOG_DBG("Exit system '%s'", subsystems[i].name);
    subsystems[i].free();
  }
}

static long systems_init(void)
{
  for (int i = 0; i < OX_ARRAY_SIZE(subsystems); ++i) {
    OX_LOG_DBG("Init system '%s'", subsystems[i].name);
    if (subsystems[i].init() != OX_SUCCESS) {
      systems_exit_starting_from(i - 1);
      return 1;
    }
  }

  return 0;
}

static void systems_exit(void)
{
  systems_exit_starting_from(OX_ARRAY_SIZE(subsystems) - 1);
}

void wrap_position(Vector2* position, const float width, const float height)
{
  position->x = fmodf(position->x, width);
  if (position->x < 0.0f)
    position->x += width;

  position->y = fmodf(position->y, height);
  if (position->y < 0.0f)
    position->y += height;
}

int main(void)
{
  const long ret_code = systems_init();
  if (ret_code != OX_SUCCESS) {
    return (int)ret_code;
  }

  const int number_of_balls = 1000;
  Vector2* ball_positions =
    ox_mem_acquire(sizeof(Vector2) * number_of_balls, OX_SOURCE_LOCATION);
  Vector2* ball_directions =
    ox_mem_acquire(sizeof(Vector2) * number_of_balls, OX_SOURCE_LOCATION);
  Color* ball_colors =
    ox_mem_acquire(sizeof(Color) * number_of_balls, OX_SOURCE_LOCATION);

  for (int i = 0; i < number_of_balls; ++i) {
    ball_positions[i].x = (float)GetRandomValue(0, GetScreenWidth());
    ball_positions[i].y = (float)GetRandomValue(0, GetScreenHeight());
    ball_directions[i].x = (float)GetRandomValue(-150, 150);
    ball_directions[i].y = (float)GetRandomValue(-150, 150);
    ball_colors[i].a = 255;
    ball_colors[i].r = GetRandomValue(100, 255);
    ball_colors[i].g = GetRandomValue(100, 255);
    ball_colors[i].b = GetRandomValue(100, 255);
  }

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    for (int i = 0; i < number_of_balls; ++i) {
      ball_positions[i].x += ball_directions[i].x * GetFrameTime();
      ball_positions[i].y += ball_directions[i].y * GetFrameTime();
      wrap_position(&ball_positions[i], (float)GetScreenWidth(),
                    (float)GetScreenHeight());

      DrawCircle((int)ball_positions[i].x, (int)ball_positions[i].y, 10,
                 ball_colors[i]);
    }
    EndDrawing();
  }

  ox_mem_release(ball_positions);
  ox_mem_release(ball_directions);
  ox_mem_release(ball_colors);

  systems_exit();
  return 0;
}
