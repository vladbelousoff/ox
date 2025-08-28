#pragma once

#define OX_ARRAY_SIZE(x)                                                       \
  _Generic(&(x), __typeof__(&(x)): (sizeof(x) / sizeof((x)[0])))