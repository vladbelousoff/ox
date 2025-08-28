#pragma once

#include <stddef.h>

void ox_timestamp(char* stamp, size_t stamp_size);
const char* ox_filename(const char* filename);

#define OX_FILENAME   ox_filename(__FILE__)
#define OX_LOG_FORMAT "[%-s|%-s] [%-16s:%5u] (%s) "

#define OX_LOG(lvl, file, line, func, fmt, ...)                                \
  do {                                                                         \
    char timestamp[16];                                                        \
    ox_timestamp(timestamp, sizeof(timestamp));                                \
    (void)fprintf(stdout, OX_LOG_FORMAT fmt "\n", lvl, timestamp, file, line,  \
                  func, ##__VA_ARGS__);                                        \
  } while (0)

#define OX_LOG_ERR(_fmt, ...)                                                  \
  OX_LOG("ERR", OX_FILENAME, __LINE__, __FUNCTION__, _fmt, ##__VA_ARGS__)

#ifdef OX_DEBUG_BUILD
#define OX_LOG_WRN(_fmt, ...)                                                  \
  OX_LOG("WRN", OX_FILENAME, __LINE__, __FUNCTION__, _fmt, ##__VA_ARGS__)
#else
#define OX_LOG_WRN(...)                                                        \
  do {                                                                         \
  } while (0)
#endif

#ifdef OX_DEBUG_BUILD
#define OX_LOG_DBG(_fmt, ...)                                                  \
  OX_LOG("DBG", OX_FILENAME, __LINE__, __FUNCTION__, _fmt, ##__VA_ARGS__)
#else
#define OX_LOG_DBG(...)                                                        \
  do {                                                                         \
  } while (0)
#endif
