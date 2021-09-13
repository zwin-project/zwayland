#include "zxdg_output.h"

#include <wayland-server.h>
#include <xdg-output-server-protocol.h>

#include "util.h"

static void zwl_zxdg_output_destroy(struct zwl_zxdg_output* zxdg_output);

static void zwl_zxdg_output_handle_destroy(struct wl_resource* resource)
{
  struct zwl_zxdg_output* zxdg_output;

  zxdg_output = wl_resource_get_user_data(resource);

  zwl_zxdg_output_destroy(zxdg_output);
}

static void zwl_zxdg_output_protocol_destroy(struct wl_client* client, struct wl_resource* resource)
{
  UNUSED(client);
  wl_resource_destroy(resource);
}

static const struct zxdg_output_v1_interface zwl_zxdg_output_interface = {
    .destroy = zwl_zxdg_output_protocol_destroy,
};

struct zwl_zxdg_output* zwl_zxdg_output_create(struct wl_client* client, uint32_t id)
{
  struct zwl_zxdg_output* zxdg_output;
  struct wl_resource* resource;

  zxdg_output = zalloc(sizeof *zxdg_output);
  if (zxdg_output == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  resource = wl_resource_create(client, &zxdg_output_v1_interface, 3, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_zxdg_output;
  }

  wl_resource_set_implementation(resource, &zwl_zxdg_output_interface, zxdg_output,
                                 zwl_zxdg_output_handle_destroy);

  zxdg_output->resource = resource;

  return zxdg_output;

out_zxdg_output:
  free(zxdg_output);

out:
  return NULL;
}

static void zwl_zxdg_output_destroy(struct zwl_zxdg_output* zxdg_output) { free(zxdg_output); }
