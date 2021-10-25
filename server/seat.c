#include "seat.h"

#include <stdio.h>
#include <wayland-server.h>

#include "keyboard.h"
#include "pointer.h"
#include "util.h"

struct zwl_pointer *zwl_seat_get_pointer(struct zwl_seat *seat,
                                         struct wl_client *client)
{
  struct zwl_pointer *pointer;

  wl_list_for_each(pointer, &seat->pointer_list, link)
  {
    if (pointer->client == client) return pointer;
  }

  return NULL;
}

struct zwl_pointer *zwl_seat_ensure_pointer(struct zwl_seat *seat,
                                            struct wl_client *client)
{
  struct zwl_pointer *pointer;

  pointer = zwl_seat_get_pointer(seat, client);
  if (pointer) return pointer;

  pointer = zwl_pointer_create(client, seat);

  return pointer;
}

void zwl_seat_destroy_pointer(struct zwl_seat *seat, struct wl_client *client)
{
  struct zwl_pointer *pointer;

  pointer = zwl_seat_get_pointer(seat, client);
  if (pointer == NULL) return;

  zwl_pointer_destroy(pointer);
}

void zwl_seat_send_capabilities(struct zwl_seat *seat, struct wl_client *client)
{
  struct wl_resource *resource;
  uint32_t capabilities = 0;
  if (zwl_seat_get_pointer(seat, client))
    capabilities |= WL_SEAT_CAPABILITY_POINTER;

  wl_resource_for_each(resource, &seat->resource_list)
  {
    if (wl_resource_get_client(resource) == client)
      wl_seat_send_capabilities(resource, capabilities);
  }
}

static void zwl_seat_protocol_get_pointer(struct wl_client *client,
                                          struct wl_resource *resource,
                                          uint32_t id)
{
  struct zwl_seat *seat = wl_resource_get_user_data(resource);
  struct zwl_pointer *pointer = zwl_seat_get_pointer(seat, client);
  struct wl_resource *pointer_resource;

  if (pointer == NULL) {
    pointer_resource = wl_resource_create(client, NULL, 7, id);
    if (pointer_resource == NULL) wl_client_post_no_memory(client);
    fprintf(stderr, "called get_pointer but no pointer capability\n");
  } else {
    pointer_resource = zwl_pointer_add_resource(pointer, client, id);
    if (pointer_resource == NULL)
      fprintf(stderr, "failed to create a pointer resource\n");
  }
}

static void zwl_seat_protocol_get_keyboard(struct wl_client *client,
                                           struct wl_resource *resource,
                                           uint32_t id)
{
  UNUSED(resource);
  struct zwl_keyboard *keyboard;

  keyboard = zwl_keyboard_create(client, id);
  if (keyboard == NULL) {
    fprintf(stderr, "failed to create a keyboard\n");
  }
}

static void zwl_seat_protocol_get_touch(struct wl_client *client,
                                        struct wl_resource *resource,
                                        uint32_t id)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(id);
}

static void zwl_seat_protocol_release(struct wl_client *client,
                                      struct wl_resource *resource)
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

static void zwl_seat_handle_destroy(struct wl_resource *resource)
{
  wl_list_remove(wl_resource_get_link(resource));
}

static void zwl_seat_bind(struct wl_client *client, void *data,
                          uint32_t version, uint32_t id)
{
  struct zwl_seat *seat = data;
  struct wl_resource *resource;

  resource = wl_resource_create(client, &wl_seat_interface, version, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    return;
  }

  wl_resource_set_implementation(resource, &zwl_seat_interface, seat,
                                 zwl_seat_handle_destroy);

  wl_list_insert(&seat->resource_list, wl_resource_get_link(resource));

  zwl_seat_send_capabilities(seat, client);
  wl_seat_send_name(resource, "default");
}

struct zwl_seat *zwl_seat_create(struct wl_display *display)
{
  struct zwl_seat *seat;
  struct wl_global *global;

  seat = zalloc(sizeof *seat);
  if (seat == NULL) goto out;

  global =
      wl_global_create(display, &wl_seat_interface, 3, seat, zwl_seat_bind);
  if (global == NULL) goto out_seat;

  wl_list_init(&seat->pointer_list);
  wl_list_init(&seat->resource_list);

  return seat;

out_seat:
  free(seat);

out:
  return NULL;
}
