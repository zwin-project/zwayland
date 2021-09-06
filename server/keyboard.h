#ifndef ZWAYLAND_KEYBOARD_H
#define ZWAYLAND_KEYBOARD_H

#include <wayland-server.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
struct zwl_keyboard {};
#pragma GCC diagnostic pop

struct zwl_keyboard *zwl_keyboard_create(struct wl_client *client, uint32_t id);

#endif  //  ZWAYLAND_KEYBOARD_H
