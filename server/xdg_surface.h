#ifndef ZWAYLAND_XDG_SURFACE_H
#define ZWAYLAND_XDG_SURFACE_H

#include <wayland-server.h>

#include "surface.h"

struct zwl_xdg_surface;

struct zwl_xdg_surface *zwl_xdg_surface_create(struct wl_client *client, uint32_t id,
                                               struct zwl_surface *surface);

struct wl_signal *zwl_xdg_surface_get_destroy_signal(struct zwl_xdg_surface *xdg_surface);

#endif  //  ZWAYLAND_XDG_SURFACE_H
