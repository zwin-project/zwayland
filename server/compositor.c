#include "compositor.h"

#include <errno.h>
#include <stdio.h>
#include <wayland-server.h>

#include "compositor_global.h"
#include "region.h"
#include "surface.h"
#include "util.h"

static void zwl_compositor_destroy(struct zwl_compositor *compositor);

static void zwl_compositor_handle_destroy(struct wl_resource *resource)
{
  struct zwl_compositor *compositor;

  compositor = wl_resource_get_user_data(resource);

  zwl_compositor_destroy(compositor);
}

static void zwl_compositor_protocol_create_surface(struct wl_client *client, struct wl_resource *resource,
                                                   uint32_t id)
{
  struct zwl_compositor *compositor;
  struct zwl_surface *surface;

  compositor = wl_resource_get_user_data(resource);

  surface = zwl_surface_create(client, id, compositor);
  if (surface == NULL) fprintf(stderr, "failed to create a surface\n");
}

static void zwl_compositor_protocol_create_region(struct wl_client *client, struct wl_resource *resource,
                                                  uint32_t id)
{
  UNUSED(resource);
  struct zwl_region *region;
  region = zwl_region_create(client, id);
  if (region == NULL) fprintf(stderr, "failed to create a region\n");

  // TODO: implement
}

static const struct wl_compositor_interface zwl_compositor_interface = {
    .create_surface = zwl_compositor_protocol_create_surface,
    .create_region = zwl_compositor_protocol_create_region,
};

static void zwl_compositor_global_flush_handler(struct wl_listener *listener, void *data)
{
  UNUSED(data);
  struct zwl_compositor *compositor;

  compositor = wl_container_of(listener, compositor, global_flush_listener);

  // TODO: implement
}

struct zwl_compositor *zwl_compositor_create(struct wl_client *client, uint32_t version, uint32_t id,
                                             struct zwl_compositor_global *compositor_global)
{
  UNUSED(compositor_global);
  struct zwl_compositor *compositor;
  struct wl_resource *resource;

  compositor = zalloc(sizeof *compositor);
  if (compositor == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  resource = wl_resource_create(client, &wl_compositor_interface, version, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_compositor;
  }
  wl_resource_set_implementation(resource, &zwl_compositor_interface, compositor,
                                 zwl_compositor_handle_destroy);

  compositor->display = compositor_global->display;

  compositor->global_flush_listener.notify = zwl_compositor_global_flush_handler;
  wl_signal_add(&compositor_global->flush_signal, &compositor->global_flush_listener);

  wl_signal_init(&compositor->destroy_signal);

  return compositor;

out_compositor:
  free(compositor);

out:
  return NULL;
}

static void zwl_compositor_destroy(struct zwl_compositor *compositor)
{
  wl_signal_emit(&compositor->destroy_signal, compositor);
  wl_list_remove(&compositor->global_flush_listener.link);
  free(compositor);
}
