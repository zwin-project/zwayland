#ifndef ZWAYLAND_UTIL_H
#define ZWAYLAND_UTIL_H

#include <stdlib.h>
#include <wayland-server.h>
#define UNUSED(x) ((void)x)

static inline void* zalloc(size_t size) { return calloc(1, size); }

/* week ref */

struct zwl_week_ref;

typedef void (*zwl_week_ref_destroy_func_t)(struct zwl_week_ref* ref);

struct zwl_week_ref {
  void* data;  // NULLABLE
  struct wl_listener destroy_listener;
  zwl_week_ref_destroy_func_t destroy_func;
};

void zwl_week_ref_init(struct zwl_week_ref* week_ref);

void zwl_week_ref_destroy(struct zwl_week_ref* ref);

void zwl_week_ref_set_data(struct zwl_week_ref* ref, void* data,
                           struct wl_signal* destroy_signal,
                           zwl_week_ref_destroy_func_t on_destroy);

#endif  // ZWAYLAND_UTIL_H
