#ifndef ZWAYLAND_POINTER_H
#define ZWAYLAND_POINTER_H

#include <wayland-server.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
struct zwl_pointer {};
#pragma GCC diagnostic pop

struct zwl_pointer *zwl_pointer_create(struct wl_client *client, uint32_t id);

#endif  //  ZWAYLAND_POINTER_H
