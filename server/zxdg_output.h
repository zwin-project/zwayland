#ifndef ZWAYLAND_ZXDG_OUTPUT_H
#define ZWAYLAND_ZXDG_OUTPUT_H

#include <wayland-server.h>

struct zwl_zxdg_output {
  struct wl_resource* resource;
};

struct zwl_zxdg_output* zwl_zxdg_output_create(struct wl_client* client,
                                               uint32_t id);

#endif  //  ZWAYLAND_ZXDG_OUTPUT_H
