#include "region.h"

#include <wayland-server.h>

#include "util.h"

static void zwl_region_destroy(struct zwl_region* region);

static void zwl_region_handle_destroy(struct wl_resource* resource)
{
  struct zwl_region* region;

  region = wl_resource_get_user_data(resource);

  zwl_region_destroy(region);
}

static void zwl_region_protocol_destroy(struct wl_client* client, struct wl_resource* resource)
{
  UNUSED(client);
  wl_resource_destroy(resource);
}

static void zwl_region_protocol_add(struct wl_client* client, struct wl_resource* resource, int32_t x,
                                    int32_t y, int32_t width, int32_t height)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(x);
  UNUSED(y);
  UNUSED(width);
  UNUSED(height);
}

static void zwl_region_protocol_subtract(struct wl_client* client, struct wl_resource* resource, int32_t x,
                                         int32_t y, int32_t width, int32_t height)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(x);
  UNUSED(y);
  UNUSED(width);
  UNUSED(height);
}

static const struct wl_region_interface zwl_region_interface = {
    .destroy = zwl_region_protocol_destroy,
    .add = zwl_region_protocol_add,
    .subtract = zwl_region_protocol_subtract,
};

struct zwl_region* zwl_region_create(struct wl_client* client, uint32_t id)
{
  struct zwl_region* region;
  struct wl_resource* resource;

  region = zalloc(sizeof *region);
  if (region == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  resource = wl_resource_create(client, &wl_region_interface, 1, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_region;
  }

  wl_resource_set_implementation(resource, &zwl_region_interface, region, zwl_region_handle_destroy);

  return region;

out_region:
  free(region);

out:
  return NULL;
}

static void zwl_region_destroy(struct zwl_region* region) { free(region); }
