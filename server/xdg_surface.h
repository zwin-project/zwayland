#ifndef ZWAYLAND_XDG_SURFACE_H
#define ZWAYLAND_XDG_SURFACE_H

#include <wayland-server.h>

#include "surface.h"

struct zwl_xdg_surface_window_geometry {
  int32_t x, y, width, height;
};

struct zwl_xdg_surface {
  struct wl_resource *resource;  // Non null
  struct wl_signal destroy_signal;
  struct zwl_surface *surface;  // Non null
  struct wl_listener surface_destroy_listener;
  struct wl_signal configure_signal;
  struct wl_signal set_window_geometry_signal;
};

struct zwl_xdg_surface *zwl_xdg_surface_create(struct wl_client *client,
                                               uint32_t id,
                                               struct zwl_surface *surface);

void zwl_xdg_surface_configure(struct zwl_xdg_surface *xdg_surface);

#endif  //  ZWAYLAND_XDG_SURFACE_H
