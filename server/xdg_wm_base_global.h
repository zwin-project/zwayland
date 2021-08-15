#ifndef ZWAYLAND_XDG_WM_BASE_GLOBAL_H
#define ZWAYLAND_XDG_WM_BASE_GLOBAL_H

#include <wayland-server.h>

struct zwl_wm_base_global;

struct zwl_wm_base_global *zwl_wm_base_global_create(struct wl_display *display);

#endif  //  ZWAYLAND_XDG_WM_BASE_GLOBAL_H
