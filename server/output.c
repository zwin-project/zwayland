#include "output.h"

#include <wayland-server.h>

#include "util.h"

static void zwl_output_protocol_release(struct wl_client *client,
                                        struct wl_resource *resource)
{
  UNUSED(client);
  UNUSED(resource);
}

static const struct wl_output_interface zwl_output_interface = {
    .release = zwl_output_protocol_release,
};

static void zwl_output_bind(struct wl_client *client, void *data,
                            uint32_t version, uint32_t id)
{
  struct zwl_output *output = data;
  struct wl_resource *resource;

  resource = wl_resource_create(client, &wl_output_interface, version, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    return;
  }

  const int width = 1920;
  const int height = 1080;
  wl_resource_set_implementation(resource, &zwl_output_interface, output, NULL);

  wl_output_send_geometry(resource, 0, 0, width, height,
                          WL_OUTPUT_SUBPIXEL_UNKNOWN, "z11", "zwayland",
                          WL_OUTPUT_TRANSFORM_NORMAL);
  wl_output_send_scale(resource, 1);
  wl_output_send_mode(resource,
                      WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED, width,
                      height, 60000);
  wl_output_send_done(resource);
}

struct zwl_output *zwl_output_create(struct wl_display *display)
{
  struct zwl_output *output;
  struct wl_global *global;

  output = zalloc(sizeof *output);
  if (output == NULL) goto out;

  global = wl_global_create(display, &wl_output_interface, 3, output,
                            zwl_output_bind);
  if (global == NULL) goto out_output;

  return output;

out_output:
  free(output);

out:
  return NULL;
}
