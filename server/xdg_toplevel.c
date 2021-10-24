#include "xdg_toplevel.h"

#include <stdio.h>
#include <string.h>
#include <wayland-server.h>
#include <zsurface.h>

#include "callback.h"
#include "surface.h"
#include "util.h"
#include "xdg-shell-server-protocol.h"
#include "xdg_surface.h"

static void zwl_xdg_toplevel_destroy(struct zwl_xdg_toplevel *xdg_toplevel);

static void zwl_xdg_toplevel_handle_destroy(struct wl_resource *resource)
{
  struct zwl_xdg_toplevel *xdg_toplevel;

  xdg_toplevel = wl_resource_get_user_data(resource);

  zwl_xdg_toplevel_destroy(xdg_toplevel);
}

static void zwl_xdg_toplevel_protocol_destroy(struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  wl_resource_destroy(resource);
}

static void zwl_xdg_toplevel_protocol_set_parent(struct wl_client *client, struct wl_resource *resource,
                                                 struct wl_resource *parent)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(parent);
  // TODO: implement
}

static void zwl_xdg_toplevel_protocol_set_title(struct wl_client *client, struct wl_resource *resource,
                                                const char *title)
{
  UNUSED(client);
  char *tmp, *old;
  struct zwl_xdg_toplevel *xdg_toplevel = wl_resource_get_user_data(resource);
  tmp = strdup(title);
  if (tmp == NULL) return;
  old = xdg_toplevel->title;
  xdg_toplevel->title = tmp;
  free(old);
}

static void zwl_xdg_toplevel_protocol_set_app_id(struct wl_client *client, struct wl_resource *resource,
                                                 const char *app_id)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(app_id);
  // TODO: implement
}

static void zwl_xdg_toplevel_protocol_show_window_menu(struct wl_client *client, struct wl_resource *resource,
                                                       struct wl_resource *seat, uint32_t serial, int32_t x,
                                                       int32_t y)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(seat);
  UNUSED(serial);
  UNUSED(x);
  UNUSED(y);
  // TODO: implement
}

static void zwl_xdg_toplevel_protocol_move(struct wl_client *client, struct wl_resource *resource,
                                           struct wl_resource *seat, uint32_t serial)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(seat);
  UNUSED(serial);
  // TODO: implement
}

static void zwl_xdg_toplevel_protocol_resize(struct wl_client *client, struct wl_resource *resource,
                                             struct wl_resource *seat, uint32_t serial, uint32_t edges)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(seat);
  UNUSED(serial);
  UNUSED(edges);
  // TODO: implement
}

static void zwl_xdg_toplevel_protocol_set_max_size(struct wl_client *client, struct wl_resource *resource,
                                                   int32_t width, int32_t height)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(width);
  UNUSED(height);
  // TODO: implement
}

static void zwl_xdg_toplevel_protocol_set_min_size(struct wl_client *client, struct wl_resource *resource,
                                                   int32_t width, int32_t height)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(width);
  UNUSED(height);
  // TODO: implement
}

static void zwl_xdg_toplevel_protocol_set_maximized(struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  UNUSED(resource);
  // TODO: implement
}

static void zwl_xdg_toplevel_protocol_unset_maximized(struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  UNUSED(resource);
  // TODO: implement
}

static void zwl_xdg_toplevel_protocol_set_fullscreen(struct wl_client *client, struct wl_resource *resource,
                                                     struct wl_resource *output)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(output);
  // TODO: implement
}

static void zwl_xdg_toplevel_protocol_unset_fullscreen(struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  UNUSED(resource);
  // TODO: implement
}

static void zwl_xdg_toplevel_protocol_set_minimized(struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  UNUSED(resource);
  // TODO: implement
}

static const struct xdg_toplevel_interface zwl_xdg_toplevel_interface = {
    .destroy = zwl_xdg_toplevel_protocol_destroy,
    .set_parent = zwl_xdg_toplevel_protocol_set_parent,
    .set_title = zwl_xdg_toplevel_protocol_set_title,
    .set_app_id = zwl_xdg_toplevel_protocol_set_app_id,
    .show_window_menu = zwl_xdg_toplevel_protocol_show_window_menu,
    .move = zwl_xdg_toplevel_protocol_move,
    .resize = zwl_xdg_toplevel_protocol_resize,
    .set_max_size = zwl_xdg_toplevel_protocol_set_max_size,
    .set_min_size = zwl_xdg_toplevel_protocol_set_min_size,
    .set_maximized = zwl_xdg_toplevel_protocol_set_maximized,
    .unset_maximized = zwl_xdg_toplevel_protocol_unset_maximized,
    .set_fullscreen = zwl_xdg_toplevel_protocol_set_fullscreen,
    .unset_fullscreen = zwl_xdg_toplevel_protocol_unset_fullscreen,
    .set_minimized = zwl_xdg_toplevel_protocol_set_minimized,
};

static void zwl_xdg_toplevel_xdg_surface_destroy_signal_handler(struct wl_listener *listener, void *data)
{
  UNUSED(data);
  struct zwl_xdg_toplevel *xdg_toplevel =
      wl_container_of(listener, xdg_toplevel, xdg_surface_destroy_listener);
  wl_resource_destroy(xdg_toplevel->resource);
}

static void zwl_xdg_toplevel_xdg_surface_configure_handler(struct wl_listener *listener, void *data)
{
  UNUSED(data);
  struct zwl_xdg_toplevel *xdg_toplevel =
      wl_container_of(listener, xdg_toplevel, xdg_surface_configure_listener);
  struct wl_array states;

  wl_array_init(&states);

  // TODO: configure states if needed.

  // TODO: send configure event after beeing notified that the cuboid window has actually resized
  zsurface_toplevel_resize(xdg_toplevel->zsurface_toplevel, (float)xdg_toplevel->config.width / 10,
                           (float)xdg_toplevel->config.height / 10);

  xdg_toplevel_send_configure(xdg_toplevel->resource, xdg_toplevel->config.width, xdg_toplevel->config.height,
                              &states);

  wl_array_release(&states);

  xdg_toplevel->configured = true;
}

static void zwl_xdg_toplevel_xdg_surface_set_window_geometry_handler(struct wl_listener *listener, void *data)
{
  struct zwl_xdg_surface_window_geometry *window_geometry = data;
  struct zwl_xdg_toplevel *xdg_toplevel =
      wl_container_of(listener, xdg_toplevel, xdg_surface_set_window_geometry_listener);

  xdg_toplevel->config.width = window_geometry->width;
  xdg_toplevel->config.height = window_geometry->height;
  xdg_toplevel->configured = false;
}

static void zwl_xdg_toplevel_surface_commit_handler(struct wl_listener *listener, void *data)
{
  UNUSED(data);
  struct zwl_xdg_toplevel *xdg_toplevel = wl_container_of(listener, xdg_toplevel, surface_commit_listener);
  struct zwl_surface *surface = xdg_toplevel->xdg_surface->surface;

  // TODO: apply other state if needed

  if (xdg_toplevel->configured == false) zwl_xdg_surface_configure(xdg_toplevel->xdg_surface);

  // TODO: xdg_surface.configure should be scheduled, not immediate.

  // TODO: config state must be applied after the clinet replies ack_configure.

  if (surface->pending.buffer_resource != NULL) {
    struct wl_shm_buffer *shm_buffer = wl_shm_buffer_get(surface->pending.buffer_resource);
    uint32_t width = wl_shm_buffer_get_width(shm_buffer);
    uint32_t height = wl_shm_buffer_get_height(shm_buffer);

    struct zsurface_view *view = zsurface_toplevel_get_view(xdg_toplevel->zsurface_toplevel);

    wl_shm_buffer_begin_access(shm_buffer);
    struct zsurface_color_bgra *data = wl_shm_buffer_get_data(shm_buffer);
    zsurface_view_set_texture(view, data, width, height);  // TODO: pass format info
    zsurface_view_commit(view);
    wl_shm_buffer_end_access(shm_buffer);
  }

  if (surface->pending.buffer_resource != NULL) wl_buffer_send_release(surface->pending.buffer_resource);
}

static void zwl_xdg_toplevel_zsurface_view_callback_done(void *data, uint32_t callback_time)
{
  struct zwl_callback *callback = data;

  wl_callback_send_done(callback->resource, callback_time);
  wl_resource_destroy(callback->resource);
}

static void zwl_xdg_toplevel_surface_frame_handler(struct wl_listener *listener, void *data)
{
  struct zwl_callback *callback = data;
  struct zwl_xdg_toplevel *xdg_toplevel = wl_container_of(listener, xdg_toplevel, surface_frame_listener);
  struct zsurface_view *view = zsurface_toplevel_get_view(xdg_toplevel->zsurface_toplevel);

  zsurface_view_add_frame_callback(view, zwl_xdg_toplevel_zsurface_view_callback_done, callback);
}

struct zwl_xdg_toplevel *zwl_xdg_toplevel_create(struct wl_client *client, uint32_t id,
                                                 struct zwl_xdg_surface *xdg_surface)
{
  struct zwl_xdg_toplevel *xdg_toplevel;
  struct wl_resource *resource;

  xdg_toplevel = zalloc(sizeof *xdg_toplevel);
  if (xdg_toplevel == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  xdg_toplevel->zsurface_toplevel = zsurface_create_toplevel_view(xdg_surface->surface->compositor->zsurface);
  if (xdg_toplevel->zsurface_toplevel == NULL) {
    wl_client_post_no_memory(client);
    goto out_xdg_toplevel;
  }

  {
    struct zsurface_view *view = zsurface_toplevel_get_view(xdg_toplevel->zsurface_toplevel);
    zsurface_view_set_user_data(view, xdg_surface->surface);
  }

  resource = wl_resource_create(client, &xdg_toplevel_interface, 3, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_zsurface_toplevel;
  }
  wl_resource_set_implementation(resource, &zwl_xdg_toplevel_interface, xdg_toplevel,
                                 zwl_xdg_toplevel_handle_destroy);

  xdg_toplevel->resource = resource;

  /* zalloc set xdg_toplevel->config to {0,0}*/
  xdg_toplevel->configured = false;

  xdg_toplevel->xdg_surface_configure_listener.notify = zwl_xdg_toplevel_xdg_surface_configure_handler;
  wl_signal_add(&xdg_surface->configure_signal, &xdg_toplevel->xdg_surface_configure_listener);

  xdg_toplevel->xdg_surface = xdg_surface;
  xdg_toplevel->xdg_surface_destroy_listener.notify = zwl_xdg_toplevel_xdg_surface_destroy_signal_handler;
  wl_signal_add(&xdg_surface->destroy_signal, &xdg_toplevel->xdg_surface_destroy_listener);

  xdg_toplevel->xdg_surface_set_window_geometry_listener.notify =
      zwl_xdg_toplevel_xdg_surface_set_window_geometry_handler;
  wl_signal_add(&xdg_surface->set_window_geometry_signal,
                &xdg_toplevel->xdg_surface_set_window_geometry_listener);

  xdg_toplevel->surface_commit_listener.notify = zwl_xdg_toplevel_surface_commit_handler;
  wl_signal_add(&xdg_surface->surface->commit_signal, &xdg_toplevel->surface_commit_listener);

  xdg_toplevel->surface_frame_listener.notify = zwl_xdg_toplevel_surface_frame_handler;
  wl_signal_add(&xdg_surface->surface->frame_signal, &xdg_toplevel->surface_frame_listener);

  xdg_toplevel->title = strdup("");

  return xdg_toplevel;

out_zsurface_toplevel:
  zsurface_destroy_toplevel_view(xdg_surface->surface->compositor->zsurface, xdg_toplevel->zsurface_toplevel);

out_xdg_toplevel:
  free(xdg_toplevel);

out:
  return NULL;
}

static void zwl_xdg_toplevel_destroy(struct zwl_xdg_toplevel *xdg_toplevel)
{
  wl_list_remove(&xdg_toplevel->surface_commit_listener.link);
  wl_list_remove(&xdg_toplevel->surface_frame_listener.link);
  wl_list_remove(&xdg_toplevel->xdg_surface_configure_listener.link);
  wl_list_remove(&xdg_toplevel->xdg_surface_set_window_geometry_listener.link);
  wl_list_remove(&xdg_toplevel->xdg_surface_destroy_listener.link);
  zsurface_destroy_toplevel_view(xdg_toplevel->xdg_surface->surface->compositor->zsurface,
                                 xdg_toplevel->zsurface_toplevel);
  free(xdg_toplevel->title);
  free(xdg_toplevel);
}
