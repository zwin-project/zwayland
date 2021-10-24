#include "xdg_surface.h"

#include <stdio.h>
#include <wayland-server.h>

#include "surface.h"
#include "util.h"
#include "xdg-shell-server-protocol.h"
#include "xdg_popup.h"
#include "xdg_surface.h"
#include "xdg_toplevel.h"

static void zwl_xdg_surface_destroy(struct zwl_xdg_surface* xdg_surface);

static void zwl_xdg_surface_handle_destroy(struct wl_resource* resource)
{
  struct zwl_xdg_surface* xdg_surface;

  xdg_surface = wl_resource_get_user_data(resource);

  zwl_xdg_surface_destroy(xdg_surface);
}

static void zwl_xdg_surface_protocol_destroy(struct wl_client* client, struct wl_resource* resource)
{
  UNUSED(client);
  wl_resource_destroy(resource);
}

static void zwl_xdg_surface_protocol_get_toplevel(struct wl_client* client, struct wl_resource* resource,
                                                  uint32_t id)
{
  struct zwl_xdg_surface* xdg_surface = wl_resource_get_user_data(resource);
  struct zwl_xdg_toplevel* xdg_toplevel;

  xdg_toplevel = zwl_xdg_toplevel_create(client, id, xdg_surface);
  if (xdg_toplevel == NULL) {
    fprintf(stderr, "failed to create a xdg toplevel surface\n");
  }
}

static void zwl_xdg_surface_protocol_get_popup(struct wl_client* client, struct wl_resource* resource,
                                               uint32_t id, struct wl_resource* parent,
                                               struct wl_resource* positioner)
{
  UNUSED(parent);
  UNUSED(positioner);
  struct zwl_xdg_surface* xdg_surface = wl_resource_get_user_data(resource);
  struct zwl_xdg_popup* popup;
  // TODO: implement more
  popup = zwl_xdg_popup_create(client, id, xdg_surface);
  if (popup == NULL) {
    fprintf(stderr, "failed to create xdg popup\n");
  }
}

static void zwl_xdg_surface_protocol_set_window_geometry(struct wl_client* client,
                                                         struct wl_resource* resource, int32_t x, int32_t y,
                                                         int32_t width, int32_t height)
{
  UNUSED(client);
  UNUSED(resource);
  struct zwl_xdg_surface* xdg_surface;
  struct zwl_xdg_surface_window_geometry window_geometry;

  xdg_surface = wl_resource_get_user_data(resource);

  window_geometry.x = x;
  window_geometry.x = y;
  window_geometry.width = width;
  window_geometry.height = height;

  wl_signal_emit(&xdg_surface->set_window_geometry_signal, &window_geometry);
}

static void zwl_xdg_surface_protocol_ack_configure(struct wl_client* client, struct wl_resource* resource,
                                                   uint32_t serial)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(serial);
  // TODO: implement
}

static const struct xdg_surface_interface zwl_xdg_surface_interface = {
    .destroy = zwl_xdg_surface_protocol_destroy,
    .get_toplevel = zwl_xdg_surface_protocol_get_toplevel,
    .get_popup = zwl_xdg_surface_protocol_get_popup,
    .set_window_geometry = zwl_xdg_surface_protocol_set_window_geometry,
    .ack_configure = zwl_xdg_surface_protocol_ack_configure,
};

static void zwl_xdg_surface_surface_destroy_signal_handler(struct wl_listener* listener, void* data)
{
  UNUSED(data);
  struct zwl_xdg_surface* xdg_surface = wl_container_of(listener, xdg_surface, surface_destroy_listener);
  wl_resource_destroy(xdg_surface->resource);
}

struct zwl_xdg_surface* zwl_xdg_surface_create(struct wl_client* client, uint32_t id,
                                               struct zwl_surface* surface)
{
  struct zwl_xdg_surface* xdg_surface;
  struct wl_resource* resource;
  struct wl_signal* surface_destroy_signal;

  xdg_surface = zalloc(sizeof *xdg_surface);
  if (xdg_surface == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  resource = wl_resource_create(client, &xdg_surface_interface, 3, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_xdg_surface;
  }
  wl_resource_set_implementation(resource, &zwl_xdg_surface_interface, xdg_surface,
                                 zwl_xdg_surface_handle_destroy);

  xdg_surface->resource = resource;
  wl_signal_init(&xdg_surface->destroy_signal);

  xdg_surface->surface = surface;
  xdg_surface->surface_destroy_listener.notify = zwl_xdg_surface_surface_destroy_signal_handler;
  surface_destroy_signal = zwl_surface_get_destroy_signal(surface);
  wl_signal_add(surface_destroy_signal, &xdg_surface->surface_destroy_listener);

  wl_signal_init(&xdg_surface->configure_signal);
  wl_signal_init(&xdg_surface->set_window_geometry_signal);

  return xdg_surface;

out_xdg_surface:
  free(xdg_surface);

out:
  return NULL;
}

static void zwl_xdg_surface_destroy(struct zwl_xdg_surface* xdg_surface)
{
  wl_signal_emit(&xdg_surface->destroy_signal, xdg_surface);
  wl_list_remove(&xdg_surface->surface_destroy_listener.link);
  free(xdg_surface);
}

void zwl_xdg_surface_configure(struct zwl_xdg_surface* xdg_surface)
{
  uint32_t serial;

  serial = wl_display_next_serial(xdg_surface->surface->compositor->display);
  wl_signal_emit(&xdg_surface->configure_signal, NULL);  // second args will be "serial", maybe

  xdg_surface_send_configure(xdg_surface->resource, serial);
}
