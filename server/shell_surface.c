#include "shell_surface.h"

#include <stdio.h>
#include <wayland-server.h>

#include "callback.h"
#include "surface.h"
#include "util.h"

static void zwl_shell_surface_destroy(struct zwl_shell_surface *shell_surface);

static void zwl_shell_surface_handle_destroy(struct wl_resource *resource)
{
  struct zwl_shell_surface *shell_surface;

  shell_surface = wl_resource_get_user_data(resource);

  zwl_shell_surface_destroy(shell_surface);
}

static void zwl_shell_surface_protocol_pong(struct wl_client *client, struct wl_resource *resource,
                                            uint32_t serial)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(serial);
}

static void zwl_shell_surface_protocol_move(struct wl_client *client, struct wl_resource *resource,
                                            struct wl_resource *seat, uint32_t serial)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(seat);
  UNUSED(serial);
}

static void zwl_shell_surface_protocol_resize(struct wl_client *client, struct wl_resource *resource,
                                              struct wl_resource *seat, uint32_t serial, uint32_t edges)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(seat);
  UNUSED(serial);
  UNUSED(edges);
}

static void zwl_shell_surface_protocol_set_toplevel(struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  struct zwl_shell_surface *shell_surface;

  shell_surface = wl_resource_get_user_data(resource);

  shell_surface->role = toplevel;
}

static void zwl_shell_surface_protocol_set_transient(struct wl_client *client, struct wl_resource *resource,
                                                     struct wl_resource *parent, int32_t x, int32_t y,
                                                     uint32_t flags)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(parent);
  UNUSED(x);
  UNUSED(y);
  UNUSED(flags);
}

static void zwl_shell_surface_protocol_set_fullscreen(struct wl_client *client, struct wl_resource *resource,
                                                      uint32_t method, uint32_t framerate,
                                                      struct wl_resource *output)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(method);
  UNUSED(framerate);
  UNUSED(output);
}

static void zwl_shell_surface_protocol_set_popup(struct wl_client *client, struct wl_resource *resource,
                                                 struct wl_resource *seat, uint32_t serial,
                                                 struct wl_resource *parent, int32_t x, int32_t y,
                                                 uint32_t flags)
{
  UNUSED(client);
  UNUSED(seat);
  UNUSED(serial);
  UNUSED(parent);
  UNUSED(x);
  UNUSED(y);
  UNUSED(flags);
  struct zwl_shell_surface *shell_surface;

  shell_surface = wl_resource_get_user_data(resource);

  shell_surface->role = popup;
}

static void zwl_shell_surface_protocol_set_maximaized(struct wl_client *client, struct wl_resource *resource,
                                                      struct wl_resource *output)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(output);
}

static void zwl_shell_surface_protocol_set_title(struct wl_client *client, struct wl_resource *resource,
                                                 const char *title)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(title);
}

static void zwl_shell_surface_protocol_set_class(struct wl_client *client, struct wl_resource *resource,
                                                 const char *class_)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(class_);
}

static const struct wl_shell_surface_interface zwl_shell_surface_interface = {
    .pong = zwl_shell_surface_protocol_pong,
    .move = zwl_shell_surface_protocol_move,
    .resize = zwl_shell_surface_protocol_resize,
    .set_toplevel = zwl_shell_surface_protocol_set_toplevel,
    .set_transient = zwl_shell_surface_protocol_set_transient,
    .set_fullscreen = zwl_shell_surface_protocol_set_fullscreen,
    .set_popup = zwl_shell_surface_protocol_set_popup,
    .set_maximized = zwl_shell_surface_protocol_set_maximaized,
    .set_title = zwl_shell_surface_protocol_set_title,
    .set_class = zwl_shell_surface_protocol_set_class,
};

static void zwl_shell_surface_surface_commit_handler(struct wl_listener *listener, void *data)
{
  UNUSED(data);
  struct zwl_shell_surface *shell_surface;
  struct zwl_surface *surface;

  shell_surface = wl_container_of(listener, shell_surface, surface_commit_listener);
  if (shell_surface->role != toplevel) {
    return;
  }

  surface = shell_surface->surface;

  if (surface->pending.buffer_resource != NULL) {
    struct wl_shm_buffer *shm_buffer = wl_shm_buffer_get(surface->pending.buffer_resource);
    enum wl_shm_format format = wl_shm_buffer_get_format(shm_buffer);
    uint32_t stride = wl_shm_buffer_get_stride(shm_buffer);
    uint32_t width = wl_shm_buffer_get_width(shm_buffer);
    uint32_t height = wl_shm_buffer_get_height(shm_buffer);
    uint32_t size = stride * height;
    wl_shm_buffer_begin_access(shm_buffer);
    uint8_t *data = wl_shm_buffer_get_data(shm_buffer);
    UNUSED(data);
    UNUSED(width);
    UNUSED(height);
    UNUSED(size);
    UNUSED(format);
    wl_shm_buffer_end_access(shm_buffer);
  }

  if (surface->pending.buffer_resource != NULL) wl_buffer_send_release(surface->pending.buffer_resource);
}

static void zwl_shell_surface_virtual_object_callback_done(void *data, uint32_t callback_time)
{
  struct zwl_callback *callback = data;

  wl_callback_send_done(callback->resource, callback_time);
  wl_resource_destroy(callback->resource);
}

static void zwl_shell_surface_surface_frame_handler(struct wl_listener *listener, void *data)
{
  struct zwl_callback *callback = data;
  struct zwl_shell_surface *shell_surface;

  shell_surface = wl_container_of(listener, shell_surface, surface_frame_listener);

  // TODO: implement
  (void)zwl_shell_surface_virtual_object_callback_done;
  (void)callback;
  (void)shell_surface;
}

static void zwl_shell_surface_surface_destroy_handler(struct wl_listener *listener, void *data)
{
  UNUSED(data);
  struct zwl_shell_surface *shell_surface;

  shell_surface = wl_container_of(listener, shell_surface, surface_destroy_listener);

  wl_resource_destroy(shell_surface->resource);
}

struct zwl_shell_surface *zwl_shell_surface_create(struct wl_client *client, uint32_t id,
                                                   struct zwl_surface *surface)
{
  UNUSED(surface);
  struct zwl_shell_surface *shell_surface;
  struct wl_resource *resource;

  shell_surface = zalloc(sizeof *shell_surface);
  if (shell_surface == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  resource = wl_resource_create(client, &wl_shell_surface_interface, 1, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_shell_surface;
  }

  wl_resource_set_implementation(resource, &zwl_shell_surface_interface, shell_surface,
                                 zwl_shell_surface_handle_destroy);

  shell_surface->resource = resource;
  shell_surface->surface = surface;
  shell_surface->role = none;

  shell_surface->surface_commit_listener.notify = zwl_shell_surface_surface_commit_handler;
  wl_signal_add(&shell_surface->surface->commit_signal, &shell_surface->surface_commit_listener);

  shell_surface->surface_frame_listener.notify = zwl_shell_surface_surface_frame_handler;
  wl_signal_add(&shell_surface->surface->frame_signal, &shell_surface->surface_frame_listener);

  shell_surface->surface_destroy_listener.notify = zwl_shell_surface_surface_destroy_handler;
  wl_signal_add(&shell_surface->surface->destroy_signal, &shell_surface->surface_destroy_listener);

  return shell_surface;

out_shell_surface:
  free(shell_surface);

out:
  return NULL;
}

static void zwl_shell_surface_destroy(struct zwl_shell_surface *shell_surface)
{
  wl_list_remove(&shell_surface->surface_commit_listener.link);
  wl_list_remove(&shell_surface->surface_frame_listener.link);
  wl_list_remove(&shell_surface->surface_destroy_listener.link);
  free(shell_surface);
}
