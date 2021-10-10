#include "pointer.h"

#include <sys/time.h>
#include <wayland-server.h>

#include "seat.h"
#include "surface.h"
#include "util.h"

static void zwl_pointer_protocol_set_cursor(struct wl_client *client, struct wl_resource *resource,
                                            uint32_t serial, struct wl_resource *surface, int32_t hotspot_x,
                                            int32_t hotspot_y)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(serial);
  UNUSED(surface);
  UNUSED(hotspot_x);
  UNUSED(hotspot_y);
}

static void zwl_pointer_protocol_release(struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  wl_resource_destroy(resource);
}

static const struct wl_pointer_interface zwl_pointer_interface = {
    .set_cursor = zwl_pointer_protocol_set_cursor,
    .release = zwl_pointer_protocol_release,
};

void zwl_pointer_send_enter(struct zwl_pointer *pointer, struct zwl_surface *surface, uint32_t x, uint32_t y)
{
  struct wl_resource *resource;
  struct wl_display *display = wl_client_get_display(pointer->client);
  uint32_t serial = wl_display_next_serial(display);

  wl_resource_for_each(resource, &pointer->resource_list)
  {
    wl_pointer_send_enter(resource, serial, surface->resource, wl_fixed_from_int(x), wl_fixed_from_int(y));
  }
}

void zwl_pointer_send_motion(struct zwl_pointer *pointer, uint32_t x, uint32_t y)
{
  struct wl_resource *resource;
  struct timeval tv;
  uint32_t current_time_in_milles;

  gettimeofday(&tv, NULL);
  current_time_in_milles = tv.tv_sec * 1000 + tv.tv_usec / 1000;

  wl_resource_for_each(resource, &pointer->resource_list)
  {
    wl_pointer_send_motion(resource, current_time_in_milles, wl_fixed_from_int(x), wl_fixed_from_int(y));
  }
}

void zwl_pointer_send_leave(struct zwl_pointer *pointer, struct zwl_surface *surface)
{
  struct wl_resource *resource;
  struct wl_display *display = wl_client_get_display(pointer->client);
  uint32_t serial = wl_display_next_serial(display);

  wl_resource_for_each(resource, &pointer->resource_list)
  {
    wl_pointer_send_leave(resource, serial, surface->resource);
  }
}

static void zwl_pointer_handle_destroy(struct wl_resource *resource)
{
  wl_list_remove(wl_resource_get_link(resource));
}

struct wl_resource *zwl_pointer_add_resource(struct zwl_pointer *pointer, struct wl_client *client,
                                             uint32_t id)
{
  struct wl_resource *resource;

  resource = wl_resource_create(client, &wl_pointer_interface, 7, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    return NULL;
  }

  wl_list_insert(&pointer->resource_list, wl_resource_get_link(resource));

  wl_resource_set_implementation(resource, &zwl_pointer_interface, pointer, zwl_pointer_handle_destroy);

  return resource;
}

static void zwl_pointer_client_destroy_handler(struct wl_listener *listener, void *data)
{
  UNUSED(data);
  struct zwl_pointer *pointer;

  pointer = wl_container_of(listener, pointer, client_destroy_listener);

  zwl_pointer_destroy(pointer);
}

struct zwl_pointer *zwl_pointer_create(struct wl_client *client, struct zwl_seat *seat)
{
  struct zwl_pointer *pointer;

  pointer = zalloc(sizeof *pointer);
  if (pointer == NULL) goto out;

  pointer->client = client;
  pointer->client_destroy_listener.notify = zwl_pointer_client_destroy_handler;
  wl_client_add_destroy_listener(client, &pointer->client_destroy_listener);

  wl_list_init(&pointer->resource_list);
  wl_list_insert(&seat->pointer_list, &pointer->link);

  return pointer;

out:
  return NULL;
}

void zwl_pointer_destroy(struct zwl_pointer *pointer)
{
  struct wl_resource *resource, *tmp;
  wl_list_remove(&pointer->link);
  wl_list_remove(&pointer->client_destroy_listener.link);
  wl_resource_for_each_safe(resource, tmp, &pointer->resource_list)
  {
    wl_resource_set_destructor(resource, NULL);
    wl_resource_set_user_data(resource, NULL);
    wl_list_remove(wl_resource_get_link(resource));
  }
  free(pointer);
}
