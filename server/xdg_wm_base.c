#include "xdg_wm_base.h"

#include <stdio.h>
#include <wayland-server.h>

#include "surface.h"
#include "util.h"
#include "xdg-shell-server-protocol.h"
#include "xdg_positioner.h"
#include "xdg_surface.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
struct zwl_wm_base {};
#pragma GCC diagnostic pop

static void zwl_wm_base_destroy(struct zwl_wm_base *wm_base);

static void zwl_wm_base_handle_destroy(struct wl_resource *resource)
{
  struct zwl_wm_base *wm_base;

  wm_base = wl_resource_get_user_data(resource);

  zwl_wm_base_destroy(wm_base);
}

static void zwl_wm_base_protocol_destroy(struct wl_client *client,
                                         struct wl_resource *resource)
{
  UNUSED(client);
  wl_resource_destroy(resource);
}

static void zwl_wm_base_protocol_create_positioner(struct wl_client *client,
                                                   struct wl_resource *resource,
                                                   uint32_t id)
{
  UNUSED(resource);
  struct zwl_xdg_positioner *positioner;

  positioner = zwl_xdg_positioner_create(client, id);
  if (positioner == NULL) {
    fprintf(stderr, "failed to create a xdg positioner\n");
  }
}

static void zwl_wm_base_protocol_get_xdg_surface(
    struct wl_client *client, struct wl_resource *resource, uint32_t id,
    struct wl_resource *surface_resource)
{
  UNUSED(resource);
  struct zwl_xdg_surface *xdg_furface;
  struct zwl_surface *surface;

  surface = wl_resource_get_user_data(surface_resource);

  xdg_furface = zwl_xdg_surface_create(client, id, surface);
  if (xdg_furface == NULL) {
    fprintf(stderr, "failed to create a xdg surface\n");
  }
}

static void zwl_wm_base_protocol_pong(struct wl_client *client,
                                      struct wl_resource *resource,
                                      uint32_t serial)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(serial);
  // TODO: implementation
}

static const struct xdg_wm_base_interface zwl_wm_base_interface = {
    .destroy = zwl_wm_base_protocol_destroy,
    .create_positioner = zwl_wm_base_protocol_create_positioner,
    .get_xdg_surface = zwl_wm_base_protocol_get_xdg_surface,
    .pong = zwl_wm_base_protocol_pong,
};

struct zwl_wm_base *zwl_wm_base_create(struct wl_client *client,
                                       uint32_t version, uint32_t id)
{
  UNUSED(version);
  struct zwl_wm_base *wm_base;
  struct wl_resource *resource;

  wm_base = zalloc(sizeof *wm_base);
  if (wm_base == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  resource = wl_resource_create(client, &xdg_wm_base_interface, 3, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_wm_base;
  }
  wl_resource_set_implementation(resource, &zwl_wm_base_interface, wm_base,
                                 zwl_wm_base_handle_destroy);

  return wm_base;

out_wm_base:
  free(wm_base);

out:
  return NULL;
}

static void zwl_wm_base_destroy(struct zwl_wm_base *wm_base) { free(wm_base); }
