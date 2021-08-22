#include "util.h"

#include <stdio.h>

void zwc_log(const char* fmt, ...)
{
  va_list argp;

  va_start(argp, fmt);
  vfprintf(stderr, fmt, argp);
  va_end(argp);
}
