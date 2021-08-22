#include "zwc_virtual_object.h"

#include <wayland-server.h>
#include <zwc.h>

#include "util.h"
#include "z11-client-protocol.h"
#include "zwc_display.h"
#include "zwc_global.h"

struct zwc_virtual_object {
  struct z11_virtual_object *z11_virtual_object;
};

struct zwc_virtual_object *zwc_virtual_object_create(struct zwc_display *display)
{
  UNUSED(display);
  struct zwc_virtual_object *virtual_object;
  struct z11_virtual_object *z11_virtual_object;

  virtual_object = zalloc(sizeof *virtual_object);
  if (virtual_object == NULL) {
    goto out;
  }

  z11_virtual_object = z11_compositor_create_virtual_object(display->global->compositor);
  if (virtual_object == NULL) {
    goto out_virtual_object;
  }

  virtual_object->z11_virtual_object = z11_virtual_object;

  return virtual_object;

out_virtual_object:
  free(virtual_object);

out:
  return NULL;
}

void zwc_virtual_object_destroy(struct zwc_virtual_object *virtual_object)
{
  z11_virtual_object_destroy(virtual_object->z11_virtual_object);
  free(virtual_object);
}

void zwc_virtual_object_commit(struct zwc_virtual_object *virtual_object)
{
  z11_virtual_object_commit(virtual_object->z11_virtual_object);
}

void zwc_virtual_object_attach_texture() {}

struct zwc_virtual_object_callback_data {
  zwc_virtual_object_frame_callback_done_func_t func;
  void *data;
};

static void zwc_virtual_object_callback_done(void *data, struct wl_callback *callback, uint32_t callback_time)
{
  struct zwc_virtual_object_callback_data *callback_data = data;

  callback_data->func(callback_data->data, callback_time);

  wl_callback_destroy(callback);
  free(callback_data);
}

static const struct wl_callback_listener zwc_virtual_object_callback_listener = {
    .done = zwc_virtual_object_callback_done,
};

void zwc_virtual_object_add_frame_callback(struct zwc_virtual_object *virtual_object,
                                           zwc_virtual_object_frame_callback_done_func_t done_func,
                                           void *data)
{
  struct wl_callback *cb;
  struct zwc_virtual_object_callback_data *callback_data;

  cb = z11_virtual_object_frame(virtual_object->z11_virtual_object);

  callback_data = zalloc(sizeof *callback_data);  // zwc_virtual_object_callback_done will free this
  callback_data->data = data;
  callback_data->func = done_func;

  wl_callback_add_listener(cb, &zwc_virtual_object_callback_listener, callback_data);
}
