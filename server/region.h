#ifndef ZWAYLAND_REGION_H
#define ZWAYLAND_REGION_H

#include <wayland-server.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
struct zwl_region {};
#pragma GCC diagnostic pop

struct zwl_region* zwl_region_create(struct wl_client* client, uint32_t id);

#endif  //  ZWAYLAND_REGION_H
