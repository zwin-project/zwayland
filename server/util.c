#include "util.h"

#include <stdio.h>
#include <wayland-server.h>

static void zwl_week_pointer_handle_destroy_signal_handler(
    struct wl_listener* listener, void* data)
{
  UNUSED(data);
  struct zwl_week_ref* ref;

  ref = wl_container_of(listener, ref, destroy_listener);
  if (ref->destroy_func) ref->destroy_func(ref->data);

  wl_list_remove(&ref->destroy_listener.link);
  wl_list_init(&ref->destroy_listener.link);
  ref->data = NULL;
  ref->destroy_func = NULL;
}

void zwl_week_ref_init(struct zwl_week_ref* ref)
{
  wl_list_init(&ref->destroy_listener.link);
  ref->destroy_listener.notify = zwl_week_pointer_handle_destroy_signal_handler;
  ref->data = NULL;
  ref->destroy_func = NULL;
}

void zwl_week_ref_destroy(struct zwl_week_ref* ref)
{
  wl_list_remove(&ref->destroy_listener.link);
}

void zwl_week_ref_set_data(struct zwl_week_ref* ref, void* data,
                           struct wl_signal* destroy_signal,
                           zwl_week_ref_destroy_func_t on_destroy)
{
  wl_list_remove(&ref->destroy_listener.link);
  wl_signal_add(destroy_signal, &ref->destroy_listener);
  ref->data = data;
  ref->destroy_func = on_destroy;
}
