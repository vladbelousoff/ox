#include "ox_core.h"
#include "ox_log.h"
#include "ox_mem.h"

#include <stdio.h>

typedef struct {
  long (*init)(void);
  void (*free)(void);
  const char* name;
} ox_system;

static ox_system systems[] = {
  { ox_mem_init, ox_mem_exit, "Memory" },
};

static void systems_exit_starting_from(const int index)
{
  for (int i = index; i >= 0; --i) {
    OX_LOG_DBG("Exit system '%s'", systems[i].name);
    systems[i].free();
  }
}

static long systems_init(void)
{
  for (int i = 0; i < OX_ARRAY_SIZE(systems); ++i) {
    OX_LOG_DBG("Init system '%s'", systems[i].name);
    if (systems[i].init() != OX_SUCCESS) {
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
  const long ret_code = systems_init();
  if (ret_code != OX_SUCCESS) {
    return (int)ret_code;
  }

  systems_exit();
  return 0;
}
