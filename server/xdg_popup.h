#ifndef WAYLAND_XDG_POPUP_H
#define WAYLAND_XDG_POPUP_H

#include <wayland-server.h>

#include "xdg_surface.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
struct zwl_xdg_popup {};
#pragma GCC diagnostic pop

struct zwl_xdg_popup *zwl_xdg_popup_create(struct wl_client *client, uint32_t id,
                                           struct zwl_xdg_surface *xdg_surface);

#endif  //  WAYLAND_XDG_POPUP_H
