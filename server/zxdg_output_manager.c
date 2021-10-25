#include "zxdg_output_manager.h"

#include <stdio.h>
#include <xdg-output-server-protocol.h>

#include "util.h"
#include "zxdg_output.h"

static void zwl_zxdg_output_manager_protocol_destroy(
    struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  UNUSED(resource);
  // nothing to do
}

static void zwl_zxdg_output_manager_protocol_get_xdg_output(
    struct wl_client *client, struct wl_resource *resource, uint32_t id,
    struct wl_resource *output)
{
  UNUSED(resource);
  UNUSED(output);
  struct zwl_zxdg_output *zxdg_output;

  zxdg_output = zwl_zxdg_output_create(client, id);
  if (zxdg_output == NULL) {
    fprintf(stderr, "failed to create a zxdg output");
  }

  zxdg_output_v1_send_logical_position(zxdg_output->resource, 0, 0);
  zxdg_output_v1_send_logical_size(zxdg_output->resource, 1920, 1080);
  zxdg_output_v1_send_done(zxdg_output->resource);
}

static const struct zxdg_output_manager_v1_interface
    zwl_output_manager_interface = {
        .destroy = zwl_zxdg_output_manager_protocol_destroy,
        .get_xdg_output = zwl_zxdg_output_manager_protocol_get_xdg_output,
};

static void zwl_zxdg_output_manager_bind(struct wl_client *client, void *data,
                                         uint32_t version, uint32_t id)
{
  struct zwl_zxdg_output_manager *output_manager = data;
  struct wl_resource *resource;

  resource = wl_resource_create(client, &zxdg_output_manager_v1_interface,
                                version, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    return;
  }

  wl_resource_set_implementation(resource, &zwl_output_manager_interface,
                                 output_manager, NULL);
}

struct zwl_zxdg_output_manager *zwl_zxdg_output_manager_create(
    struct wl_display *display)
{
  struct zwl_zxdg_output_manager *output_manager;
  struct wl_global *global;

  output_manager = zalloc(sizeof *output_manager);
  if (output_manager == NULL) goto out;

  global = wl_global_create(display, &zxdg_output_manager_v1_interface, 3,
                            output_manager, zwl_zxdg_output_manager_bind);
  if (global == NULL) goto out_output_manager;

  return output_manager;

out_output_manager:
  free(output_manager);

out:
  return NULL;
}
