#ifndef ZWAYLAND_XDG_TOPLEVEL_H
#define ZWAYLAND_XDG_TOPLEVEL_H

#include <wayland-server.h>
#include <zsurface.h>

#include "xdg_surface.h"

struct zwl_xdg_toplevel_config {
  uint32_t width;
  uint32_t height;
};

struct zwl_xdg_toplevel {
  struct wl_resource *resource;  // Non null
  struct zwl_xdg_toplevel_config config;
  bool configured;
  struct wl_listener xdg_surface_configure_listener;
  struct zwl_xdg_surface *xdg_surface;  // Non null
  struct wl_listener xdg_surface_destroy_listener;
  struct wl_listener xdg_surface_set_window_geometry_listener;
  struct wl_listener surface_commit_listener;
  struct wl_listener surface_frame_listener;
  char *title;

  struct zsurf_toplevel *zsurface_toplevel;
};

struct zwl_xdg_toplevel *zwl_xdg_toplevel_create(
    struct wl_client *client, uint32_t id, struct zwl_xdg_surface *xdg_surface);

#endif  //  ZWAYLAND_XDG_TOPLEVEL_H
