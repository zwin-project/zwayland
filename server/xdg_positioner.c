#include "xdg_positioner.h"

#include <wayland-server.h>
#include <xdg-shell-server-protocol.h>

#include "util.h"

static void zwl_xdg_positioner_destroy(struct zwl_xdg_positioner *positioner);

static void zwl_xdg_positioner_handle_destroy(struct wl_resource *resource)
{
  struct zwl_xdg_positioner *positioner;

  positioner = wl_resource_get_user_data(resource);

  zwl_xdg_positioner_destroy(positioner);
}

static void zwl_xdg_positioner_protocol_destroy(struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  wl_resource_destroy(resource);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static void zwl_xdg_positioner_protocol_set_size(struct wl_client *client, struct wl_resource *resource,
                                                 int32_t width, int32_t height)
{}
static void zwl_xdg_positioner_protocol_set_anchor_rect(struct wl_client *client,
                                                        struct wl_resource *resource, int32_t x, int32_t y,
                                                        int32_t width, int32_t height)
{}
static void zwl_xdg_positioner_protocol_set_anchor(struct wl_client *client, struct wl_resource *resource,
                                                   uint32_t anchor)
{}
static void zwl_xdg_positioner_protocol_set_gravity(struct wl_client *client, struct wl_resource *resource,
                                                    uint32_t gravity)
{}
static void zwl_xdg_positioner_protocol_set_constraint_adjustment(struct wl_client *client,
                                                                  struct wl_resource *resource,
                                                                  uint32_t constraint_adjustment)
{}
static void zwl_xdg_positioner_protocol_set_offset(struct wl_client *client, struct wl_resource *resource,
                                                   int32_t x, int32_t y)
{}
static void zwl_xdg_positioner_protocol_set_reactive(struct wl_client *client, struct wl_resource *resource)
{}
static void zwl_xdg_positioner_protocol_set_parent_size(struct wl_client *client,
                                                        struct wl_resource *resource, int32_t parent_width,
                                                        int32_t parent_height)
{}
static void zwl_xdg_positioner_protocol_set_parent_configure(struct wl_client *client,
                                                             struct wl_resource *resource, uint32_t serial)
{}
#pragma GCC diagnostic pop

static const struct xdg_positioner_interface zwl_xdg_positioner_interface = {
    .destroy = zwl_xdg_positioner_protocol_destroy,
    .set_size = zwl_xdg_positioner_protocol_set_size,
    .set_anchor_rect = zwl_xdg_positioner_protocol_set_anchor_rect,
    .set_anchor = zwl_xdg_positioner_protocol_set_anchor,
    .set_gravity = zwl_xdg_positioner_protocol_set_gravity,
    .set_constraint_adjustment = zwl_xdg_positioner_protocol_set_constraint_adjustment,
    .set_offset = zwl_xdg_positioner_protocol_set_offset,
    .set_reactive = zwl_xdg_positioner_protocol_set_reactive,
    .set_parent_size = zwl_xdg_positioner_protocol_set_parent_size,
    .set_parent_configure = zwl_xdg_positioner_protocol_set_parent_configure,
};

struct zwl_xdg_positioner *zwl_xdg_positioner_create(struct wl_client *client, uint32_t id)
{
  struct zwl_xdg_positioner *positioner;
  struct wl_resource *resource;

  positioner = zalloc(sizeof *positioner);
  if (positioner == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  resource = wl_resource_create(client, &xdg_positioner_interface, 3, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_positioner;
  }

  wl_resource_set_implementation(resource, &zwl_xdg_positioner_interface, positioner,
                                 zwl_xdg_positioner_handle_destroy);

  return positioner;

out_positioner:
  free(positioner);

out:
  return NULL;
}

static void zwl_xdg_positioner_destroy(struct zwl_xdg_positioner *positioner) { free(positioner); }
