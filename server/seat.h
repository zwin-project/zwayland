#ifndef ZWAYLAND_SEAT_H
#define ZWAYLAND_SEAT_H

#include <wayland-server.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
struct zwl_seat {};
#pragma GCC diagnostic pop

struct zwl_seat *zwl_seat_create(struct wl_display *display);

#endif  //  ZWAYLAND_SEAT_H
