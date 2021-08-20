#ifndef ZWC_GLOBAL_H
#define ZWC_GLOBAL_H

#include "z11-client-protocol.h"

struct zwc_global {
  struct wl_registry *registry;
  struct z11_compositor *compositor;
  struct wl_shm *shm;
  struct z11_opengl *gl;
  struct z11_opengl_render_component_manager *render_component_manager;
};

struct zwc_global *zwc_global_create(struct wl_display *display);

void zwc_global_destroy(struct zwc_global *global);

#endif  //  ZWC_GLOBAL_H
