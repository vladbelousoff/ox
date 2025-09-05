#pragma once

// Error codes
#define OX_SUCCESS 0
#define OX_FAILURE 1

#define OX_ARRAY_SIZE(x)                                                       \
  _Generic(&(x), __typeof__(&(x)): (sizeof(x) / sizeof((x)[0])))

#define OX_DECLARE_ID(name)                                                    \
  typedef struct {                                                             \
    int value;                                                                 \
  } name

#define OX_SIZEOF_IN_BITS(x) (sizeof(x) * CHAR_BIT)
