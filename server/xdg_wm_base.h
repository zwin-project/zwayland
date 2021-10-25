#ifndef ZWAYLAND_XDG_WM_BASE
#define ZWAYLAND_XDG_WM_BASE

#include <wayland-server.h>

struct zwl_wm_base;

struct zwl_wm_base *zwl_wm_base_create(struct wl_client *client,
                                       uint32_t version, uint32_t id);

#endif  //  ZWAYLAND_XDG_WM_BASE
