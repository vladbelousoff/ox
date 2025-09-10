#include "ox_editor.h"
#include "../code/ox_log.h"
#include "../code/ox_memory.h"
#include "../code/ox_render.h"

#include <stdlib.h>

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

int main(int argc, char** argv)
{
    // Initialize engine subsystems
    const long ret_code = systems_init();
    if (ret_code != OX_SUCCESS) {
        return (int)ret_code;
    }

    // Initialize and run the editor
    ox_editor_t editor;
    if (ox_editor_init(&editor, argc, argv) != OX_SUCCESS) {
        systems_exit();
        return 1;
    }

    ox_editor_run(&editor);
    ox_editor_shutdown(&editor);

    systems_exit();
    return 0;
}