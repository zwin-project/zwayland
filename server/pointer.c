#include "pointer.h"

#include <sys/time.h>
#include <wayland-server.h>
#include <zsurface.h>

#include "seat.h"
#include "surface.h"
#include "util.h"

static void zwl_pointer_pending_cursor_surface_destroy_handler(
    struct wl_listener *listener, void *data)
{
  UNUSED(data);
  struct zwl_pointer *pointer;

  pointer = wl_container_of(listener, pointer,
                            pending_cursor_surface_destroy_listener);

  pointer->pending_cursor.surface_resource = NULL;
  wl_list_init(&pointer->pending_cursor_surface_commit_listener.link);
  wl_list_init(&pointer->pending_cursor_surface_destroy_listener.link);
}

static void zwl_pointer_pending_cursor_surface_commit_handler(
    struct wl_listener *listener, void *data)
{
  UNUSED(data);
  struct zwl_pointer *pointer;
  struct zwl_surface *surface;

  pointer = wl_container_of(listener, pointer,
                            pending_cursor_surface_commit_listener);

  surface = wl_resource_get_user_data(pointer->pending_cursor.surface_resource);

  if (surface->pending.buffer_resource != NULL) {
    struct wl_shm_buffer *shm_buffer =
        wl_shm_buffer_get(surface->pending.buffer_resource);
    struct zsurf_display *surface_display =
        surface->compositor->surface_display;
    uint32_t width = wl_shm_buffer_get_width(shm_buffer);
    uint32_t height = wl_shm_buffer_get_height(shm_buffer);
    struct zsurf_color_bgra *buf = wl_shm_buffer_get_data(shm_buffer);

    wl_shm_buffer_begin_access(shm_buffer);
    zsurf_display_set_cursor(surface_display, buf, width, height,
                             pointer->pending_cursor.hotspot_x,
                             pointer->pending_cursor.hotspot_y);
    wl_shm_buffer_end_access(shm_buffer);
    wl_buffer_send_release(surface->pending.buffer_resource);
  } else {
    // TODO: unset cursor
  }
}

static void zwl_pointer_protocol_set_cursor(
    struct wl_client *client, struct wl_resource *resource, uint32_t serial,
    struct wl_resource *surface_resource, int32_t hotspot_x, int32_t hotspot_y)
{
  UNUSED(client);
  UNUSED(serial);
  // skip serial check; clients cannot change the cursor of other client's
  // surface
  struct zwl_pointer *pointer;
  struct zwl_surface *surface;

  pointer = wl_resource_get_user_data(resource);

  pointer->pending_cursor.surface_resource = surface_resource;
  pointer->pending_cursor.hotspot_x = hotspot_x;
  pointer->pending_cursor.hotspot_y = hotspot_y;

  wl_list_remove(&pointer->pending_cursor_surface_commit_listener.link);
  wl_list_remove(&pointer->pending_cursor_surface_destroy_listener.link);

  if (surface_resource) {
    surface = wl_resource_get_user_data(surface_resource);
    wl_signal_add(&surface->commit_signal,
                  &pointer->pending_cursor_surface_commit_listener);
    wl_signal_add(&surface->destroy_signal,
                  &pointer->pending_cursor_surface_destroy_listener);
  } else {
    // TODO: unset cursor
    wl_list_init(&pointer->pending_cursor_surface_commit_listener.link);
    wl_list_init(&pointer->pending_cursor_surface_destroy_listener.link);
  }
}

static void zwl_pointer_protocol_release(struct wl_client *client,
                                         struct wl_resource *resource)
{
  UNUSED(client);
  wl_resource_destroy(resource);
}

static const struct wl_pointer_interface zwl_pointer_interface = {
    .set_cursor = zwl_pointer_protocol_set_cursor,
    .release = zwl_pointer_protocol_release,
};

void zwl_pointer_send_enter(struct zwl_pointer *pointer,
                            struct zwl_surface *surface, wl_fixed_t x,
                            wl_fixed_t y)
{
  struct wl_resource *resource;
  struct wl_display *display = wl_client_get_display(pointer->client);

  // TODO: keep this value for set_cursor()
  uint32_t serial = wl_display_next_serial(display);

  wl_resource_for_each(resource, &pointer->resource_list)
  {
    wl_pointer_send_enter(resource, serial, surface->resource, x, y);
  }
}

void zwl_pointer_send_motion(struct zwl_pointer *pointer, uint32_t time,
                             wl_fixed_t x, wl_fixed_t y)
{
  struct wl_resource *resource;

  wl_resource_for_each(resource, &pointer->resource_list)
  {
    wl_pointer_send_motion(resource, time, x, y);
  }
}

void zwl_pointer_send_leave(struct zwl_pointer *pointer,
                            struct zwl_surface *surface)
{
  struct wl_resource *resource;
  struct wl_display *display = wl_client_get_display(pointer->client);
  uint32_t serial = wl_display_next_serial(display);

  wl_resource_for_each(resource, &pointer->resource_list)
  {
    wl_pointer_send_leave(resource, serial, surface->resource);
  }
}

void zwl_pointer_send_button(struct zwl_pointer *pointer, uint32_t serial,
                             uint32_t time, uint32_t button, uint32_t state)
{
  struct wl_resource *resource;

  wl_resource_for_each(resource, &pointer->resource_list)
  {
    wl_pointer_send_button(resource, serial, time, button, state);
  }
}

static void zwl_pointer_handle_destroy(struct wl_resource *resource)
{
  wl_list_remove(wl_resource_get_link(resource));
}

struct wl_resource *zwl_pointer_add_resource(struct zwl_pointer *pointer,
                                             struct wl_client *client,
                                             uint32_t id)
{
  struct wl_resource *resource;

  resource = wl_resource_create(client, &wl_pointer_interface, 7, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    return NULL;
  }

  wl_list_insert(&pointer->resource_list, wl_resource_get_link(resource));

  wl_resource_set_implementation(resource, &zwl_pointer_interface, pointer,
                                 zwl_pointer_handle_destroy);

  return resource;
}

static void zwl_pointer_client_destroy_handler(struct wl_listener *listener,
                                               void *data)
{
  UNUSED(data);
  struct zwl_pointer *pointer;

  pointer = wl_container_of(listener, pointer, client_destroy_listener);

  zwl_pointer_destroy(pointer);
}

struct zwl_pointer *zwl_pointer_create(struct wl_client *client,
                                       struct zwl_seat *seat)
{
  struct zwl_pointer *pointer;

  pointer = zalloc(sizeof *pointer);
  if (pointer == NULL) goto out;

  pointer->client = client;
  pointer->client_destroy_listener.notify = zwl_pointer_client_destroy_handler;
  wl_client_add_destroy_listener(client, &pointer->client_destroy_listener);

  wl_list_init(&pointer->resource_list);
  wl_list_insert(&seat->pointer_list, &pointer->link);

  pointer->pending_cursor.surface_resource = NULL;

  pointer->pending_cursor_surface_commit_listener.notify =
      zwl_pointer_pending_cursor_surface_commit_handler;
  wl_list_init(&pointer->pending_cursor_surface_commit_listener.link);

  pointer->pending_cursor_surface_destroy_listener.notify =
      zwl_pointer_pending_cursor_surface_destroy_handler;
  wl_list_init(&pointer->pending_cursor_surface_destroy_listener.link);

  return pointer;

out:
  return NULL;
}

void zwl_pointer_destroy(struct zwl_pointer *pointer)
{
  struct wl_resource *resource, *tmp;
  wl_list_remove(&pointer->link);
  wl_list_remove(&pointer->client_destroy_listener.link);
  wl_list_remove(&pointer->pending_cursor_surface_commit_listener.link);
  wl_list_remove(&pointer->pending_cursor_surface_destroy_listener.link);
  wl_resource_for_each_safe(resource, tmp, &pointer->resource_list)
  {
    wl_resource_set_destructor(resource, NULL);
    wl_resource_set_user_data(resource, NULL);
    wl_list_remove(wl_resource_get_link(resource));
  }
  free(pointer);
}
