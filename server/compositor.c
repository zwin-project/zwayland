#include "compositor.h"

#include <errno.h>
#include <stdio.h>
#include <wayland-server.h>
#include <zwc.h>

#include "compositor_global.h"
#include "surface.h"
#include "util.h"

struct zwl_compositor {
  struct zwc_display *zwc_display;
  struct wl_listener global_flush_listener;
  struct wl_event_source *event_source;
};

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
  UNUSED(resource);
  struct zwl_surface *surface = zwl_surface_create(client, id);
  if (surface == NULL) fprintf(stderr, "failed to create a surface\n");
}

static void zwl_compositor_protocol_create_region(struct wl_client *client, struct wl_resource *resource,
                                                  uint32_t id)
{
  UNUSED(client);
  UNUSED(resource);
  UNUSED(id);
  // TODO: implement
}

static const struct wl_compositor_interface zwl_compositor_interface = {
    .create_surface = zwl_compositor_protocol_create_surface,
    .create_region = zwl_compositor_protocol_create_region,
};

static int zwl_compositor_zwc_dispatch(int fd, uint32_t mask, void *data)
{
  UNUSED(fd);
  UNUSED(mask);
  struct zwl_compositor *compositor = data;

  while (zwc_display_prepare_read(compositor->zwc_display) != 0) {
    if (errno != EAGAIN) {
      zwl_compositor_destroy(compositor);
      fprintf(stderr, "Disconnected");
      return 0;
    };
    zwc_display_dispatch_pending(compositor->zwc_display);
  }
  if (zwc_display_flush(compositor->zwc_display) == -1) {
    zwl_compositor_destroy(compositor);
    fprintf(stderr, "Disconnected");
    return 0;
  }
  zwc_display_read_events(compositor->zwc_display);
  zwc_display_dispatch_pending(compositor->zwc_display);
  return 0;
}

static void zwl_compositor_global_flush_handler(struct wl_listener *listener, void *data)
{
  UNUSED(data);
  struct zwl_compositor *compositor;

  compositor = wl_container_of(listener, compositor, global_flush_listener);

  zwc_display_flush(compositor->zwc_display);
}

struct zwl_compositor *zwl_compositor_create(struct wl_client *client, uint32_t version, uint32_t id,
                                             struct zwl_compositor_global *compositor_global)
{
  UNUSED(compositor_global);
  struct zwl_compositor *compositor;
  struct wl_resource *resource;
  int fd;

  compositor = zalloc(sizeof *compositor);
  if (compositor == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  compositor->zwc_display = zwc_display_create(NULL);
  if (compositor->zwc_display == NULL) {
    wl_client_post_no_memory(client);
    goto out_compositor;
  }

  resource = wl_resource_create(client, &wl_compositor_interface, version, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_zwc_display;
  }
  wl_resource_set_implementation(resource, &zwl_compositor_interface, compositor,
                                 zwl_compositor_handle_destroy);

  struct wl_event_loop *loop = wl_display_get_event_loop(compositor_global->display);

  fd = zwc_display_get_fd(compositor->zwc_display);
  compositor->event_source =
      wl_event_loop_add_fd(loop, fd, WL_EVENT_READABLE, zwl_compositor_zwc_dispatch, compositor);

  compositor->global_flush_listener.notify = zwl_compositor_global_flush_handler;
  wl_signal_add(&compositor_global->flush_signal, &compositor->global_flush_listener);

  return compositor;

out_zwc_display:
  zwc_display_destroy(compositor->zwc_display);

out_compositor:
  free(compositor);

out:
  return NULL;
}

static void zwl_compositor_destroy(struct zwl_compositor *compositor)
{
  zwc_display_destroy(compositor->zwc_display);
  wl_list_remove(&compositor->global_flush_listener.link);
  wl_event_source_remove(compositor->event_source);
  free(compositor);
}
