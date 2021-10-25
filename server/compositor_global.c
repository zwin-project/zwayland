#include "compositor_global.h"

#include <stdio.h>
#include <wayland-server.h>

#include "compositor.h"
#include "util.h"

static void zwl_compositor_global_bind(struct wl_client *client, void *data,
                                       uint32_t version, uint32_t id)
{
  struct zwl_compositor_global *compositor_global = data;
  struct zwl_compositor *compositor;

  compositor = zwl_compositor_create(client, version, id, compositor_global);
  if (compositor == NULL) {
    fprintf(stderr, "fail to create compositor client\n");
  }
}

struct zwl_compositor_global *zwl_compositor_global_create(
    struct wl_display *display)
{
  struct zwl_compositor_global *compositor_global;
  struct wl_global *global;

  compositor_global = zalloc(sizeof *compositor_global);
  if (compositor_global == NULL) goto out;

  global = wl_global_create(display, &wl_compositor_interface, 4,
                            compositor_global, zwl_compositor_global_bind);
  if (global == NULL) goto out_compositor;

  compositor_global->display = display;
  wl_signal_init(&compositor_global->flush_signal);

  return compositor_global;

out_compositor:
  free(compositor_global);

out:
  return NULL;
}
