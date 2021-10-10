#ifndef ZWAYLAND_COMPOSITOR_GLOBAL_H
#define ZWAYLAND_COMPOSITOR_GLOBAL_H

#include <wayland-server.h>

/* compositor */

struct zwl_compositor_global {
  struct wl_display *display;
  struct wl_signal flush_signal;
  struct zwl_seat *seat;
};

struct zwl_compositor_global *zwl_compositor_global_create(struct wl_display *display);

#endif  //  ZWAYLAND_COMPOSITOR_GLOBAL_H
