#ifndef ZWAYLAND_SURFACE_H
#define ZWAYLAND_SURFACE_H

#include <wayland-server.h>

struct zwl_surface;

struct zwl_surface *zwl_surface_create(struct wl_client *client, uint32_t id);

struct wl_signal *zwl_surface_get_destroy_signal(struct zwl_surface *surface);

#endif  // ZWAYLAND_SURFACE_H
