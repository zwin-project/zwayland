#include "pointer.h"

#include <wayland-server.h>

#include "util.h"

static void zwl_pointer_destroy(struct zwl_pointer *pointer);

static void zwl_pointer_handle_destroy(struct wl_resource *resource)
{
  struct zwl_pointer *pointer;

  pointer = wl_resource_get_user_data(resource);

  zwl_pointer_destroy(pointer);
}

static void zwl_pointer_protocol_set_cursor(struct wl_client *client, struct wl_resource *resource,
                                            uint32_t serial, struct wl_resource *surface, int32_t hotspot_x,
                                            int32_t hotspot_y)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(serial);
  UNUSED(surface);
  UNUSED(hotspot_x);
  UNUSED(hotspot_y);
}

static void zwl_pointer_protocol_release(struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  UNUSED(resource);
}

static const struct wl_pointer_interface zwl_pointer_interface = {
    .set_cursor = zwl_pointer_protocol_set_cursor,
    .release = zwl_pointer_protocol_release,
};

struct zwl_pointer *zwl_pointer_create(struct wl_client *client, uint32_t id)
{
  struct zwl_pointer *pointer;
  struct wl_resource *resource;

  pointer = zalloc(sizeof *pointer);
  if (pointer == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  resource = wl_resource_create(client, &wl_pointer_interface, 7, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_pointer;
  }

  wl_resource_set_implementation(resource, &zwl_pointer_interface, pointer, zwl_pointer_handle_destroy);

  return pointer;

out_pointer:
  free(pointer);

out:
  return NULL;
}

static void zwl_pointer_destroy(struct zwl_pointer *pointer) { free(pointer); }
