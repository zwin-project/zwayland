#ifndef ZWAYLAND_SHELL_SURFACE_H
#define ZWAYLAND_SHELL_SURFACE_H

#include <wayland-server.h>

#include "surface.h"

enum zwl_shell_surface_role {
  none,
  toplevel,
  popup,
};

struct zwl_shell_surface {
  struct wl_resource *resource;
  struct zwl_surface *surface;
  enum zwl_shell_surface_role role;
  struct wl_listener surface_commit_listener;
  struct wl_listener surface_frame_listener;
  struct wl_listener surface_destroy_listener;
};

struct zwl_shell_surface *zwl_shell_surface_create(struct wl_client *client,
                                                   uint32_t id,
                                                   struct zwl_surface *surface);

#endif  //  ZWAYLAND_SHELL_SURFACE_H
