#include "xdg_toplevel.h"

#include <stdio.h>
#include <string.h>
#include <wayland-server.h>

#include "util.h"
#include "xdg-shell-server-protocol.h"
#include "xdg_surface.h"

struct zwl_xdg_toplevel {
  struct wl_resource *resource;
  struct zwl_xdg_surface *xdg_surface;
  struct wl_listener xdg_surface_destroy_listener;
  char *title;
};

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

struct zwl_xdg_toplevel *zwl_xdg_toplevel_create(struct wl_client *client, uint32_t id,
                                                 struct zwl_xdg_surface *xdg_surface)
{
  struct zwl_xdg_toplevel *xdg_toplevel;
  struct wl_resource *resource;
  struct wl_signal *xdg_surface_destroy_signal;

  xdg_toplevel = zalloc(sizeof *xdg_toplevel);
  if (xdg_toplevel == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  resource = wl_resource_create(client, &xdg_toplevel_interface, 3, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_xdg_toplevel;
  }
  wl_resource_set_implementation(resource, &zwl_xdg_toplevel_interface, xdg_toplevel,
                                 zwl_xdg_toplevel_handle_destroy);

  xdg_toplevel->resource = resource;
  xdg_toplevel->title = strdup("");

  xdg_toplevel->xdg_surface = xdg_surface;
  xdg_toplevel->xdg_surface_destroy_listener.notify = zwl_xdg_toplevel_xdg_surface_destroy_signal_handler;
  xdg_surface_destroy_signal = zwl_xdg_surface_get_destroy_signal(xdg_surface);
  wl_signal_add(xdg_surface_destroy_signal, &xdg_toplevel->xdg_surface_destroy_listener);

  return xdg_toplevel;

out_xdg_toplevel:
  free(xdg_toplevel);

out:
  return NULL;
}

static void zwl_xdg_toplevel_destroy(struct zwl_xdg_toplevel *xdg_toplevel)
{
  wl_list_remove(&xdg_toplevel->xdg_surface_destroy_listener.link);
  free(xdg_toplevel);
}
