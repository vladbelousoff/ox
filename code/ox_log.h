#pragma once

#include <stddef.h>

void ox_timestamp(char* stamp, size_t stamp_size);
const char* ox_filename(const char* filename);

#define OX_FILENAME   ox_filename(__FILE__)
#define OX_LOG_FORMAT "[%-s|%-s] [%-16s:%5u] (%s) "

#define ox_log(lvl, file, line, func, fmt, ...)                                \
  do {                                                                         \
    char timestamp[16];                                                        \
    ox_timestamp(timestamp, sizeof(timestamp));                                \
    (void)fprintf(stdout, OX_LOG_FORMAT fmt "\n", lvl, timestamp, file, line,  \
                  func, ##__VA_ARGS__);                                        \
  } while (0)

#define ox_log_err(_fmt, ...)                                                  \
  ox_log("ERR", OX_FILENAME, __LINE__, __FUNCTION__, _fmt, ##__VA_ARGS__)

#ifdef OX_DEBUG_BUILD
#define ox_log_wrn(_fmt, ...)                                                  \
  ox_log("WRN", OX_FILENAME, __LINE__, __FUNCTION__, _fmt, ##__VA_ARGS__)
#else
#define ox_log_wrn(...)                                                        \
  do {                                                                         \
  } while (0)
#endif

#ifdef OX_DEBUG_BUILD
#define ox_log_dbg(_fmt, ...)                                                  \
  ox_log("DBG", OX_FILENAME, __LINE__, __FUNCTION__, _fmt, ##__VA_ARGS__)
#else
#define ox_log_dbg(...)                                                        \
  do {                                                                         \
  } while (0)
#endif
