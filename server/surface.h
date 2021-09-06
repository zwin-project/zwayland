#ifndef ZWAYLAND_SURFACE_H
#define ZWAYLAND_SURFACE_H

#include <wayland-server.h>

#include "compositor.h"

struct zwl_surface_state {
  struct wl_resource *buffer_resource;  // Nullable
};

struct zwl_surface {
  struct wl_resource *resource;  // Non null
  struct wl_signal commit_signal;
  struct wl_signal frame_signal;
  struct wl_signal destroy_signal;
  struct zwl_compositor *compositor;  // Non null
  struct wl_listener compositor_destroy_listener;
  struct zwl_surface_state pending;
};

struct zwl_surface *zwl_surface_create(struct wl_client *client, uint32_t id,
                                       struct zwl_compositor *compositor);

struct wl_signal *zwl_surface_get_destroy_signal(struct zwl_surface *surface);

#endif  // ZWAYLAND_SURFACE_H
