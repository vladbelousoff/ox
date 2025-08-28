#include "ox_log.h"

#include <string.h>
#include <time.h>

void ox_timestamp(char* stamp, const size_t stamp_size)
{
  const time_t now = time(NULL);
  const struct tm* tm_info = localtime(&now);
  strftime(stamp, stamp_size, "%H:%M:%S", tm_info);
}

const char* ox_filename(const char* filename)
{
#ifdef _WIN32
  const char* p = strrchr(filename, '\\');
#else
  const char* p = strrchr(filename, '/');
#endif
  if (p) {
    return p + 1;
  }

#ifdef _WIN32
  p = strrchr(filename, '/');
  if (p) {
    return p + 1;
  }
#endif

  return filename;
}