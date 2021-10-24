#include "xdg_popup.h"

#include <xdg-shell-server-protocol.h>

#include "util.h"

static void zwl_xdg_popup_destroy(struct zwl_xdg_popup *popup);

static void zwl_xdg_popup_handle_destroy(struct wl_resource *resource)
{
  struct zwl_xdg_popup *popup;

  popup = wl_resource_get_user_data(resource);

  zwl_xdg_popup_destroy(popup);
}

static void zwl_xdg_popup_protocol_destroy(struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  wl_resource_destroy(resource);
}

static void zwl_xdg_popup_protocol_grab(struct wl_client *client, struct wl_resource *resource,
                                        struct wl_resource *seat, uint32_t serial)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(seat);
  UNUSED(serial);
}

static void zwl_xdg_popup_protocol_reposition(struct wl_client *client, struct wl_resource *resource,
                                              struct wl_resource *positioner, uint32_t token)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(positioner);
  UNUSED(token);
}

static const struct xdg_popup_interface zwl_xdg_popup_interface = {
    .destroy = zwl_xdg_popup_protocol_destroy,
    .grab = zwl_xdg_popup_protocol_grab,
    .reposition = zwl_xdg_popup_protocol_reposition,
};

struct zwl_xdg_popup *zwl_xdg_popup_create(struct wl_client *client, uint32_t id,
                                           struct zwl_xdg_surface *xdg_surface)
{
  UNUSED(xdg_surface);
  struct zwl_xdg_popup *popup;
  struct wl_resource *resource;

  popup = zalloc(sizeof *popup);
  if (popup == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  resource = wl_resource_create(client, &xdg_popup_interface, 3, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_popup;
  }

  wl_resource_set_implementation(resource, &zwl_xdg_popup_interface, popup, zwl_xdg_popup_handle_destroy);

  return popup;

out_popup:
  free(popup);

out:
  return NULL;
}

static void zwl_xdg_popup_destroy(struct zwl_xdg_popup *popup) { free(popup); }
