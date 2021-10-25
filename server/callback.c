#include "callback.h"

#include <wayland-server.h>

#include "util.h"

static void zwl_callback_destroy(struct zwl_callback* callback);

static void zwl_callback_handle_destroy(struct wl_resource* resource)
{
  struct zwl_callback* callback;

  callback = wl_resource_get_user_data(resource);

  zwl_callback_destroy(callback);
}

struct zwl_callback* zwl_callback_create(struct wl_client* client, uint32_t id)
{
  struct zwl_callback* callback;
  struct wl_resource* resource;

  callback = zalloc(sizeof *callback);
  if (callback == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  resource = wl_resource_create(client, &wl_callback_interface, 1, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_callback;
  }

  callback->resource = resource;

  wl_resource_set_implementation(resource, NULL, callback,
                                 zwl_callback_handle_destroy);

  return callback;

out_callback:
  free(callback);

out:
  return NULL;
}

static void zwl_callback_destroy(struct zwl_callback* callback)
{
  free(callback);
}
