#include "ox_core.h"
#include "ox_log.h"
#include "ox_mem.h"
#include "ox_render.h"
#include "raylib.h"

#include <stdio.h>

typedef struct {
  long (*init)(void);
  void (*free)(void);
  const char* name;
} ox_subsystem_t;

static ox_subsystem_t subsystems[] = {
  { ox_mem_init, ox_mem_exit, "Memory" },
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

int main(void)
{
  const long ret_code = systems_init();
  if (ret_code != OX_SUCCESS) {
    return (int)ret_code;
  }

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Hello, OX!", 190, 200, 20, LIGHTGRAY);
    EndDrawing();
  }

  systems_exit();
  return 0;
}
