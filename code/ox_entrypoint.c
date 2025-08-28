#include "ox_core.h"
#include "ox_log.h"
#include "ox_memory.h"

#include <stdio.h>

typedef struct {
  long (*init)(void);
  void (*free)(void);
  const char* name;
} ox_system;

static ox_system systems[] = {
  { ox_memory_init, ox_memory_exit, "Memory" },
};

static void systems_exit_starting_from(const int index)
{
  for (int i = index; i >= 0; --i) {
    ox_log_dbg("Exit system '%s'", systems[i].name);
    systems[i].free();
  }
}

static long systems_init(void)
{
  for (int i = 0; i < OX_ARRAY_SIZE(systems); ++i) {
    ox_log_dbg("Init system '%s'", systems[i].name);
    if (systems[i].init() != 0) {
      systems_exit_starting_from(i - 1);
      return 1;
    }
  }

  return 0;
}

static void systems_exit(void)
{
  systems_exit_starting_from(OX_ARRAY_SIZE(systems) - 1);
}

int main(void)
{
  systems_init();
  systems_exit();

  return 0;
}
