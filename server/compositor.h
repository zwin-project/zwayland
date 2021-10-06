#ifndef ZWAYLAND_COMPOSITOR_H
#define ZWAYLAND_COMPOSITOR_H

#include <wayland-server.h>

#include "compositor_global.h"

struct zwl_compositor {
  struct wl_display *display;
  struct wl_listener global_flush_listener;
  struct wl_signal destroy_signal;
};

struct zwl_compositor *zwl_compositor_create(struct wl_client *client, uint32_t version, uint32_t id,
                                             struct zwl_compositor_global *compositor_global);

#endif  //  ZWAYLAND_COMPOSITOR_H
