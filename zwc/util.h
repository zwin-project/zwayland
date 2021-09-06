#ifndef ZWC_UTIL_H
#define ZWC_UTIL_H

#include <stdlib.h>

#define UNUSED(x) ((void)x)

static inline void* zalloc(size_t size) { return calloc(1, size); }

void zwc_log(const char* fmt, ...);

#endif  //  ZWC_UTIL_H
