#ifndef ZWAYLAND_SHELL_H
#define ZWAYLAND_SHELL_H

#include <wayland-server.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
struct zwl_shell {};
#pragma GCC diagnostic pop

struct zwl_shell *zwl_shell_create(struct wl_display *display);

#endif  //  ZWAYLAND_SHELL_H
