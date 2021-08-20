#ifndef ZWAYLAND_XDG_TOPLEVEL_H
#define ZWAYLAND_XDG_TOPLEVEL_H

#include <wayland-server.h>

#include "xdg_surface.h"

struct zwl_xdg_toplevel;

struct zwl_xdg_toplevel *zwl_xdg_toplevel_create(struct wl_client *client, uint32_t id,
                                                 struct zwl_xdg_surface *xdg_surface);

#endif  //  ZWAYLAND_XDG_TOPLEVEL_H
