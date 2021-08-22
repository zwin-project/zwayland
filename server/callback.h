#ifndef ZWAYLAND_CALLBACK_H
#define ZWAYLAND_CALLBACK_H

#include <wayland-server.h>

struct zwl_callback {
  struct wl_resource* resource;
};

struct zwl_callback* zwl_callback_create(struct wl_client* client, uint32_t id);

#endif  //  ZWAYLAND_CALLBACK_H
