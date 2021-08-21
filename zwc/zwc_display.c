#include <stdio.h>
#include <stdlib.h>
#include <wayland-client.h>
#include <zwc.h>

#include "zwc_global.h"

struct zwc_display {
  struct wl_display* wl_display;
  struct zwc_global* global;
  struct z11_virtual_object* virtual_object;
};

static void callback_done(void* data, struct wl_callback* callback, uint32_t callback_data);

static const struct wl_callback_listener zwc_callback_listener = {
    .done = callback_done,
};

static void callback_done(void* data, struct wl_callback* callback, uint32_t callback_data)
{
  (void)callback_data;
  struct zwc_display* zwc_display = data;
  struct wl_callback* cb;
  wl_callback_destroy(callback);
  cb = z11_virtual_object_frame(zwc_display->virtual_object);
  wl_callback_add_listener(cb, &zwc_callback_listener, zwc_display);
  z11_virtual_object_commit(zwc_display->virtual_object);
}

struct zwc_display* zwc_display_create(const char* name)
{
  struct zwc_display* zwc_display;
  struct z11_virtual_object* virtual_object;
  struct wl_callback* cb;

  if (name == NULL) name = "z11-0";

  zwc_display = calloc(1, sizeof *zwc_display);
  if (zwc_display == NULL) goto fail;

  zwc_display->wl_display = wl_display_connect(name);
  if (zwc_display->wl_display == NULL) {
    fprintf(stderr, "zwc error: can't connect to display\n");
    goto out_zwc_display;
  }

  zwc_display->global = zwc_global_create(zwc_display->wl_display);
  if (zwc_display->global == NULL) {
    goto out_wl_display;
  }

  virtual_object = z11_compositor_create_virtual_object(zwc_display->global->compositor);

  zwc_display->virtual_object = virtual_object;

  cb = z11_virtual_object_frame(virtual_object);
  wl_callback_add_listener(cb, &zwc_callback_listener, zwc_display);
  z11_virtual_object_commit(virtual_object);
  wl_display_flush(zwc_display->wl_display);

  return zwc_display;

out_wl_display:
  wl_display_disconnect(zwc_display->wl_display);

out_zwc_display:
  free(zwc_display);

fail:
  return NULL;
}

void zwc_display_destroy(struct zwc_display* zwc_display)
{
  zwc_global_destroy(zwc_display->global);
  wl_display_disconnect(zwc_display->wl_display);
  free(zwc_display);
}

int zwc_display_prepare_read(struct zwc_display* display)
{
  return wl_display_prepare_read(display->wl_display);
}

int zwc_display_dispatch_pending(struct zwc_display* display)
{
  return wl_display_dispatch_pending(display->wl_display);
}

int zwc_display_flush(struct zwc_display* display) { return wl_display_flush(display->wl_display); }

int zwc_display_read_events(struct zwc_display* display)
{
  return wl_display_read_events(display->wl_display);
}

int zwc_display_get_fd(struct zwc_display* display) { return wl_display_get_fd(display->wl_display); }
