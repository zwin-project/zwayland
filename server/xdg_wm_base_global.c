#include "xdg_wm_base_global.h"

#include <stdio.h>
#include <wayland-server.h>

#include "util.h"
#include "xdg-shell-server-protocol.h"
#include "xdg_wm_base.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
struct zwl_wm_base_global {};
#pragma GCC diagnostic pop

static void zwl_wm_base_global_bind(struct wl_client *client, void *data,
                                    uint32_t version, uint32_t id)
{
  UNUSED(data);
  struct zwl_wm_base *wm_base;

  wm_base = zwl_wm_base_create(client, version, id);
  if (wm_base == NULL) {
    fprintf(stderr, "failed to create a wm base");
  }
}

struct zwl_wm_base_global *zwl_wm_base_global_create(struct wl_display *display)
{
  struct zwl_wm_base_global *wm_base_global;
  struct wl_global *global;

  wm_base_global = zalloc(sizeof *wm_base_global);
  if (wm_base_global == NULL) goto out;

  global = wl_global_create(display, &xdg_wm_base_interface, 3, wm_base_global,
                            zwl_wm_base_global_bind);
  if (global == NULL) goto out_wm_base;

  return wm_base_global;

out_wm_base:
  free(wm_base_global);

out:
  return NULL;
}
