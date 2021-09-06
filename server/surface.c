#include "surface.h"

#include <wayland-server.h>

#include "callback.h"
#include "compositor.h"
#include "stdio.h"
#include "util.h"

static void zwl_surface_destroy(struct zwl_surface *surface);

static void zwl_surface_handle_destroy(struct wl_resource *resource)
{
  struct zwl_surface *surface;

  surface = wl_resource_get_user_data(resource);

  zwl_surface_destroy(surface);
}

static void zwl_surface_protocol_destory(struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  wl_resource_destroy(resource);
}
static void zwl_surface_protocol_attach(struct wl_client *client, struct wl_resource *resource,
                                        struct wl_resource *buffer_resource, int32_t x, int32_t y)
{
  UNUSED(client);
  UNUSED(x);
  UNUSED(y);

  struct zwl_surface *surface;

  surface = wl_resource_get_user_data(resource);

  surface->pending.buffer_resource = buffer_resource;

  // TODO: implement more
}

static void zwl_surface_protocol_damage(struct wl_client *client, struct wl_resource *resource, int32_t x,
                                        int32_t y, int32_t width, int32_t height)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(x);
  UNUSED(y);
  UNUSED(width);
  UNUSED(height);
  // TODO: implement
}

static void zwl_surface_protocol_frame(struct wl_client *client, struct wl_resource *resource,
                                       uint32_t callback_id)
{
  struct zwl_surface *surface = wl_resource_get_user_data(resource);
  struct zwl_callback *callback;
  callback = zwl_callback_create(client, callback_id);
  if (callback == NULL) {
    fprintf(stderr, "failed to create frame callback\n");
  }

  wl_signal_emit(&surface->frame_signal, callback);
}

static void zwl_surface_protocol_set_opaque_region(struct wl_client *client, struct wl_resource *resource,
                                                   struct wl_resource *region)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(region);
  // TODO: implement
}

static void zwl_surface_protocol_set_input_region(struct wl_client *client, struct wl_resource *resource,
                                                  struct wl_resource *region)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(region);
  // TODO: implement
}

static void zwl_surface_protocol_commit(struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  struct zwl_surface *surface;

  surface = wl_resource_get_user_data(resource);

  wl_signal_emit(&surface->commit_signal, NULL);
}

static void zwl_surface_protocol_set_buffer_transform(struct wl_client *client, struct wl_resource *resource,
                                                      int32_t transform)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(transform);
  // TODO: implement
}

static void zwl_surface_protocol_set_buffer_scale(struct wl_client *client, struct wl_resource *resource,
                                                  int32_t scale)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(scale);
  // TODO: implement
}

static void zwl_surface_protocol_damage_buffer(struct wl_client *client, struct wl_resource *resource,
                                               int32_t x, int32_t y, int32_t width, int32_t height)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(x);
  UNUSED(y);
  UNUSED(width);
  UNUSED(height);
  // TODO: implement
}

static const struct wl_surface_interface zwl_surface_interface = {
    .destroy = zwl_surface_protocol_destory,
    .attach = zwl_surface_protocol_attach,
    .damage = zwl_surface_protocol_damage,
    .frame = zwl_surface_protocol_frame,
    .set_opaque_region = zwl_surface_protocol_set_opaque_region,
    .set_input_region = zwl_surface_protocol_set_input_region,
    .commit = zwl_surface_protocol_commit,
    .set_buffer_transform = zwl_surface_protocol_set_buffer_transform,
    .set_buffer_scale = zwl_surface_protocol_set_buffer_scale,
    .damage_buffer = zwl_surface_protocol_damage_buffer,
};

static void zwl_surface_compositor_destroy_handler(struct wl_listener *listener, void *data)
{
  UNUSED(data);
  struct zwl_surface *surface;

  surface = wl_container_of(listener, surface, compositor_destroy_listener);

  wl_resource_destroy(surface->resource);
}

struct zwl_surface *zwl_surface_create(struct wl_client *client, uint32_t id,
                                       struct zwl_compositor *compositor)
{
  struct zwl_surface *surface;
  struct wl_resource *resource;

  surface = zalloc(sizeof *surface);
  if (surface == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  resource = wl_resource_create(client, &wl_surface_interface, 4, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_surface;
  }
  wl_resource_set_implementation(resource, &zwl_surface_interface, surface, zwl_surface_handle_destroy);

  surface->resource = resource;

  wl_signal_init(&surface->commit_signal);
  wl_signal_init(&surface->frame_signal);
  wl_signal_init(&surface->destroy_signal);

  surface->compositor = compositor;
  surface->compositor_destroy_listener.notify = zwl_surface_compositor_destroy_handler;
  wl_signal_add(&compositor->destroy_signal, &surface->compositor_destroy_listener);

  surface->pending.buffer_resource = NULL;
  // TODO: listen buffer resource destroy signal

  return surface;

out_surface:
  free(surface);

out:
  return NULL;
}

static void zwl_surface_destroy(struct zwl_surface *surface)
{
  wl_signal_emit(&surface->destroy_signal, surface);
  wl_list_remove(&surface->compositor_destroy_listener.link);
  free(surface);
}

struct wl_signal *zwl_surface_get_destroy_signal(struct zwl_surface *surface)
{
  return &surface->destroy_signal;
}
