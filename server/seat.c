#include "seat.h"

#include <stdio.h>
#include <wayland-server.h>

#include "keyboard.h"
#include "pointer.h"
#include "util.h"

static void zwl_seat_protocol_get_pointer(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
  UNUSED(resource);
  struct zwl_pointer *pointer;

  pointer = zwl_pointer_create(client, id);
  if (pointer == NULL) {
    fprintf(stderr, "failed to create a pointer\n");
  }
}

static void zwl_seat_protocol_get_keyboard(struct wl_client *client, struct wl_resource *resource,
                                           uint32_t id)
{
  UNUSED(resource);
  struct zwl_keyboard *keyboard;

  keyboard = zwl_keyboard_create(client, id);
  if (keyboard == NULL) {
    fprintf(stderr, "failed to create a keyboard\n");
  }
}

static void zwl_seat_protocol_get_touch(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(id);
}

static void zwl_seat_protocol_release(struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  UNUSED(resource);
}

static const struct wl_seat_interface zwl_seat_interface = {
    .get_pointer = zwl_seat_protocol_get_pointer,
    .get_keyboard = zwl_seat_protocol_get_keyboard,
    .get_touch = zwl_seat_protocol_get_touch,
    .release = zwl_seat_protocol_release,
};

static void zwl_seat_bind(struct wl_client *client, void *data, uint32_t version, uint32_t id)
{
  struct zwl_seat *seat = data;
  struct wl_resource *resource;

  resource = wl_resource_create(client, &wl_seat_interface, version, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    return;
  }

  wl_resource_set_implementation(resource, &zwl_seat_interface, seat, NULL);

  // FIXME: check capability & name
  wl_seat_send_capabilities(resource, WL_SEAT_CAPABILITY_POINTER | WL_SEAT_CAPABILITY_KEYBOARD);
  wl_seat_send_name(resource, "default");
}

struct zwl_seat *zwl_seat_create(struct wl_display *display)
{
  struct zwl_seat *seat;
  struct wl_global *global;

  seat = zalloc(sizeof *seat);
  if (seat == NULL) goto out;

  global = wl_global_create(display, &wl_seat_interface, 3, seat, zwl_seat_bind);
  if (global == NULL) goto out_seat;

  return seat;

out_seat:
  free(seat);

out:
  return NULL;
}
