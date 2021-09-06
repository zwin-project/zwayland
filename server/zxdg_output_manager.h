#ifndef ZWAYLAND_ZXDG_OUTPUT_MANAGER_H
#define ZWAYLAND_ZXDG_OUTPUT_MANAGER_H

#include <wayland-server.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
struct zwl_zxdg_output_manager {};
#pragma GCC diagnostic pop

struct zwl_zxdg_output_manager *zwl_zxdg_output_manager_create(struct wl_display *display);

#endif  //  ZWAYLAND_ZXDG_OUTPUT_MANAGER_H
