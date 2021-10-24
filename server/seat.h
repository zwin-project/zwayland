#ifndef ZWAYLAND_SEAT_H
#define ZWAYLAND_SEAT_H

#include <wayland-server.h>

struct zwl_seat {
  struct wl_list pointer_list;
  struct wl_list resource_list;
};

// nullable
struct zwl_pointer* zwl_seat_get_pointer(struct zwl_seat* seat, struct wl_client* client);

struct zwl_pointer* zwl_seat_ensure_pointer(struct zwl_seat* seat, struct wl_client* client);

void zwl_seat_destroy_pointer(struct zwl_seat* seat, struct wl_client* client);

void zwl_seat_send_capabilities(struct zwl_seat* seat, struct wl_client* client);

struct zwl_seat* zwl_seat_create(struct wl_display* display);

#endif  //  ZWAYLAND_SEAT_H
