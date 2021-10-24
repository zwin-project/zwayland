#ifndef ZWAYLAND_XDG_POSITIONER_H
#define ZWAYLAND_XDG_POSITIONER_H

#include <wayland-server.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
struct zwl_xdg_positioner {};
#pragma GCC diagnostic pop

struct zwl_xdg_positioner *zwl_xdg_positioner_create(struct wl_client *client, uint32_t id);

#endif  //  ZWAYLAND_XDG_POSITIONER_H
