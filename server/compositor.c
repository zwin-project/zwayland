#include "compositor.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <wayland-server.h>
#include <zsurface.h>

#include "compositor_global.h"
#include "keyboard.h"
#include "pointer.h"
#include "region.h"
#include "seat.h"
#include "surface.h"
#include "util.h"

static void zwl_compositor_destroy(struct zwl_compositor *compositor);

static void zwl_compositor_handle_destroy(struct wl_resource *resource)
{
  struct zwl_compositor *compositor;

  compositor = wl_resource_get_user_data(resource);

  zwl_compositor_destroy(compositor);
}

static void zwl_compositor_protocol_create_surface(struct wl_client *client,
                                                   struct wl_resource *resource,
                                                   uint32_t id)
{
  struct zwl_compositor *compositor;
  struct zwl_surface *surface;

  compositor = wl_resource_get_user_data(resource);

  surface = zwl_surface_create(client, id, compositor);
  if (surface == NULL) fprintf(stderr, "failed to create a surface\n");
}

static void zwl_compositor_protocol_create_region(struct wl_client *client,
                                                  struct wl_resource *resource,
                                                  uint32_t id)
{
  UNUSED(resource);
  struct zwl_region *region;
  region = zwl_region_create(client, id);
  if (region == NULL) fprintf(stderr, "failed to create a region\n");

  // TODO: implement
}

static const struct wl_compositor_interface zwl_compositor_interface = {
    .create_surface = zwl_compositor_protocol_create_surface,
    .create_region = zwl_compositor_protocol_create_region,
};

static void zwl_compositor_global_flush_handler(struct wl_listener *listener,
                                                void *data)
{
  UNUSED(data);
  struct zwl_compositor *compositor;

  compositor = wl_container_of(listener, compositor, global_flush_listener);

  zsurf_display_flush(compositor->surface_display);
}

static int zwl_compositor_zsurface_dispatch(int fd, uint32_t mask, void *data)
{
  UNUSED(fd);
  UNUSED(mask);
  struct zwl_compositor *compositor = data;

  while (zsurf_display_prepare_read(compositor->surface_display) == -1) {
    if (errno != EAGAIN) {
      wl_resource_post_error(compositor->resource,
                             WL_DISPLAY_ERROR_IMPLEMENTATION,
                             "failed to prepare to read zsurface event");
      return -1;
    }
    if (zsurf_display_dispatch_pending(compositor->surface_display) == -1) {
      wl_resource_post_error(compositor->resource,
                             WL_DISPLAY_ERROR_IMPLEMENTATION,
                             "failed to dispatch zsurface pending event");
      return -1;
    }
  }

  while (zsurf_display_flush(compositor->surface_display) == -1) {
    if (errno != EAGAIN) {
      zsurf_display_cancel_read(compositor->surface_display);
      wl_resource_post_error(compositor->resource,
                             WL_DISPLAY_ERROR_IMPLEMENTATION,
                             "failed to flush zsurface request");
      return -1;
    }
  }

  if (zsurf_display_read_events(compositor->surface_display) == -1) {
    wl_resource_post_error(compositor->resource,
                           WL_DISPLAY_ERROR_IMPLEMENTATION,
                           "failed to read zsurface event");
    return -1;
  }

  if (zsurf_display_dispatch_pending(compositor->surface_display) == -1) {
    wl_resource_post_error(compositor->resource,
                           WL_DISPLAY_ERROR_IMPLEMENTATION,
                           "failed to dispatch zsurface pending event");
    return -1;
  }

  return 0;
}

static void zwl_compositor_handle_seat_capabilities(void *data,
                                                    uint32_t capabilities)
{
  struct zwl_compositor *compositor = data;
  struct zwl_seat *seat = compositor->compositor_global->seat;
  struct wl_client *client = wl_resource_get_client(compositor->resource);

  if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
    struct zwl_pointer *pointer;
    pointer = zwl_seat_ensure_pointer(seat, client);
    if (pointer == NULL)
      wl_resource_post_error(compositor->resource, WL_DISPLAY_ERROR_NO_MEMORY,
                             "failed to create a pointer");
  } else {
    zwl_seat_destroy_pointer(seat, client);
  }

  if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
    struct zwl_keyboard *keyboard;
    keyboard = zwl_seat_ensure_keyboard(seat, client);
    if (keyboard == NULL)
      wl_resource_post_error(compositor->resource, WL_DISPLAY_ERROR_NO_MEMORY,
                             "failed to create a keyboard");
  } else {
    zwl_seat_destroy_keyboard(seat, client);
  }

  zwl_seat_send_capabilities(seat, client);
}

static void zwl_compositor_handle_pointer_enter(void *data,
                                                struct zsurf_view *view,
                                                float x, float y)
{
  struct zwl_compositor *compositor = data;
  struct zwl_seat *seat = compositor->compositor_global->seat;
  struct zwl_pointer *pointer =
      zwl_seat_get_pointer(seat, wl_resource_get_client(compositor->resource));
  struct zwl_surface *surface = zsurf_view_get_user_data(view);
  if (surface == NULL || pointer == NULL) return;

  zwl_pointer_send_enter(pointer, surface, wl_fixed_from_double(x),
                         wl_fixed_from_double(y));
}

static void zwl_compositor_handle_pointer_motion(void *data, uint32_t time,
                                                 float x, float y)
{
  struct zwl_compositor *compositor = data;
  struct zwl_seat *seat = compositor->compositor_global->seat;
  struct zwl_pointer *pointer =
      zwl_seat_get_pointer(seat, wl_resource_get_client(compositor->resource));
  if (pointer == NULL) return;

  zwl_pointer_send_motion(pointer, time, wl_fixed_from_double(x),
                          wl_fixed_from_double(y));
}

static void zwl_compositor_handle_pointer_leave(void *data,
                                                struct zsurf_view *view)
{
  struct zwl_compositor *compositor = data;
  struct zwl_seat *seat = compositor->compositor_global->seat;
  struct zwl_pointer *pointer =
      zwl_seat_get_pointer(seat, wl_resource_get_client(compositor->resource));
  struct zwl_surface *surface = zsurf_view_get_user_data(view);
  if (surface == NULL || pointer == NULL) return;

  zwl_pointer_send_leave(pointer, surface);
}

static void zwl_compositor_handle_pointer_button(void *data, uint32_t serial,
                                                 uint32_t time, uint32_t button,
                                                 uint32_t state)
{
  struct zwl_compositor *compositor = data;
  struct zwl_seat *seat = compositor->compositor_global->seat;
  struct zwl_pointer *pointer =
      zwl_seat_get_pointer(seat, wl_resource_get_client(compositor->resource));
  if (pointer == NULL) return;

  zwl_pointer_send_button(pointer, serial, time, button, state);
}

static void zwl_compositor_handle_keyboard_keymap(void *data, uint32_t format,
                                                  int fd, uint32_t size)
{
  struct zwl_compositor *compositor = data;
  struct zwl_seat *seat = compositor->compositor_global->seat;
  struct zwl_keyboard *keyboard =
      zwl_seat_get_keyboard(seat, wl_resource_get_client(compositor->resource));
  if (keyboard == NULL) return;

  keyboard->keymap_info.format = format;
  keyboard->keymap_info.fd = fd;
  keyboard->keymap_info.size = size;
}

static void zwl_compositor_handle_keyboard_enter(void *data, uint32_t serial,
                                                 struct zsurf_view *view,
                                                 uint32_t *keys,
                                                 uint32_t key_count)
{
  struct zwl_compositor *compositor = data;
  struct zwl_seat *seat = compositor->compositor_global->seat;
  struct zwl_keyboard *keyboard =
      zwl_seat_get_keyboard(seat, wl_resource_get_client(compositor->resource));
  struct wl_array keys_array;
  struct zwl_surface *surface = zsurf_view_get_user_data(view);
  if (surface == NULL || keyboard == NULL) return;

  wl_array_init(&keys_array);
  wl_array_add(&keys_array, sizeof(uint32_t) * key_count);
  memcpy(keys_array.data, keys, sizeof(uint32_t) * key_count);

  zwl_keyboard_send_enter(keyboard, serial, surface, &keys_array);

  wl_array_release(&keys_array);
}

static void zwl_compositor_handle_keyboard_leave(void *data, uint32_t serial,
                                                 struct zsurf_view *view)
{
  struct zwl_compositor *compositor = data;
  struct zwl_seat *seat = compositor->compositor_global->seat;
  struct zwl_keyboard *keyboard =
      zwl_seat_get_keyboard(seat, wl_resource_get_client(compositor->resource));
  struct zwl_surface *surface = zsurf_view_get_user_data(view);
  if (surface == NULL || keyboard == NULL) return;

  zwl_keyboard_send_leave(keyboard, serial, surface);
}

static void zwl_compositor_handle_keyboard_key(void *data, uint32_t serial,
                                               uint32_t time, uint32_t key,
                                               uint32_t state)
{
  struct zwl_compositor *compositor = data;
  struct zwl_seat *seat = compositor->compositor_global->seat;
  struct zwl_keyboard *keyboard =
      zwl_seat_get_keyboard(seat, wl_resource_get_client(compositor->resource));
  if (keyboard == NULL) return;

  zwl_keyboard_send_key(keyboard, serial, time, key, state);
}

static const struct zsurf_display_interface surface_display_interface = {
    .seat_capabilities = zwl_compositor_handle_seat_capabilities,
    .pointer_enter = zwl_compositor_handle_pointer_enter,
    .pointer_motion = zwl_compositor_handle_pointer_motion,
    .pointer_leave = zwl_compositor_handle_pointer_leave,
    .pointer_button = zwl_compositor_handle_pointer_button,
    .keyboard_keymap = zwl_compositor_handle_keyboard_keymap,
    .keyboard_enter = zwl_compositor_handle_keyboard_enter,
    .keyboard_leave = zwl_compositor_handle_keyboard_leave,
    .keyboard_key = zwl_compositor_handle_keyboard_key,
};

struct zwl_compositor *zwl_compositor_create(
    struct wl_client *client, uint32_t version, uint32_t id,
    struct zwl_compositor_global *compositor_global)
{
  UNUSED(compositor_global);
  struct zwl_compositor *compositor;
  struct wl_resource *resource;
  struct wl_event_loop *loop;
  int fd;

  compositor = zalloc(sizeof *compositor);
  if (compositor == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  compositor->surface_display =
      zsurf_display_create("zigen-0", &surface_display_interface, compositor);
  if (compositor->surface_display == NULL) {
    wl_client_post_no_memory(client);
    goto out_compositor;
  }

  resource = wl_resource_create(client, &wl_compositor_interface, version, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_zsurf_display;
  }

  wl_resource_set_implementation(resource, &zwl_compositor_interface,
                                 compositor, zwl_compositor_handle_destroy);

  compositor->resource = resource;
  compositor->display = compositor_global->display;
  compositor->compositor_global = compositor_global;

  loop = wl_display_get_event_loop(compositor->display);
  fd = zsurf_display_get_fd(compositor->surface_display);

  compositor->event_source =
      wl_event_loop_add_fd(loop, fd, WL_EVENT_READABLE,
                           zwl_compositor_zsurface_dispatch, compositor);

  compositor->global_flush_listener.notify =
      zwl_compositor_global_flush_handler;
  wl_signal_add(&compositor_global->flush_signal,
                &compositor->global_flush_listener);

  wl_signal_init(&compositor->destroy_signal);

  return compositor;

out_zsurf_display:
  zsurf_display_destroy(compositor->surface_display);

out_compositor:
  free(compositor);

out:
  return NULL;
}

static void zwl_compositor_destroy(struct zwl_compositor *compositor)
{
  wl_event_source_remove(compositor->event_source);
  wl_signal_emit(&compositor->destroy_signal, compositor);
  wl_list_remove(&compositor->global_flush_listener.link);
  zsurf_display_destroy(compositor->surface_display);
  free(compositor);
}
