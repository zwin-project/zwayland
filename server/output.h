#ifndef ZWAYLAND_OUTPUT_H
#define ZWAYLAND_OUTPUT_H

#include <wayland-server.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
struct zwl_output {};
#pragma GCC diagnostic pop

struct zwl_output *zwl_output_create(struct wl_display *display);

#endif  //  ZWAYLAND_OUTPUT_H
