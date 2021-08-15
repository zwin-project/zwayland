#define _GNU_SOURCE 1

#include "zwc_global.h"

#include <stdlib.h>
#include <string.h>

#include "z11-client-protocol.h"
#include "z11-opengl-client-protocol.h"

void shm_format(void *data, struct wl_shm *shm, uint32_t format)
{
  (void)data;
  (void)shm;
  (void)format;
  // TODO: handle this if needed
}

struct wl_shm_listener shm_listener = {
    shm_format,
};

static void global_registry_handler(void *data, struct wl_registry *registry, uint32_t id,
                                    const char *interface, uint32_t version)
{
  struct zwc_global *global = data;

  if (strcmp(interface, "z11_compositor") == 0) {
    global->compositor = wl_registry_bind(registry, id, &z11_compositor_interface, version);
  } else if (strcmp(interface, "wl_shm") == 0) {
    global->shm = wl_registry_bind(registry, id, &wl_shm_interface, version);
    wl_shm_add_listener(global->shm, &shm_listener, NULL);
  } else if (strcmp(interface, "z11_opengl") == 0) {
    global->gl = wl_registry_bind(registry, id, &z11_opengl_interface, version);
  } else if (strcmp(interface, "z11_opengl_render_component_manager") == 0) {
    global->render_component_manager =
        wl_registry_bind(registry, id, &z11_opengl_render_component_manager_interface, version);
  }
}

static void global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
  (void)data;
  (void)registry;
  (void)id;
}

static const struct wl_registry_listener registry_listener = {
    global_registry_handler,
    global_registry_remover,
};

struct zwc_global *zwc_global_create(struct wl_display *display)
{
  struct zwc_global *global;
  struct wl_registry *registry;

  global = malloc(sizeof *global);
  if (global == NULL) goto out;

  global->compositor = NULL;
  global->shm = NULL;
  global->gl = NULL;
  global->render_component_manager = NULL;

  registry = wl_display_get_registry(display);
  if (registry == NULL) goto out_global;

  global->registry = registry;

  wl_registry_add_listener(registry, &registry_listener, global);

  wl_display_roundtrip(display);
  wl_display_dispatch(display);

  if (!(global->compositor && global->gl && global->shm && global->render_component_manager)) {
    if (global->compositor != NULL) z11_compositor_destroy(global->compositor);
    if (global->gl != NULL) z11_opengl_destroy(global->gl);
    if (global->shm != NULL) wl_shm_destroy(global->shm);
    if (global->render_component_manager != NULL)
      z11_opengl_render_component_manager_destroy(global->render_component_manager);
    goto out_registry;
  }

  return global;

out_registry:
  wl_registry_destroy(registry);

out_global:
  free(global);

out:
  return NULL;
}

void zwc_global_destroy(struct zwc_global *global)
{
  z11_compositor_destroy(global->compositor);
  z11_opengl_destroy(global->gl);
  wl_shm_destroy(global->shm);
  z11_opengl_render_component_manager_destroy(global->render_component_manager);
  wl_registry_destroy(global->registry);
  free(global);
}
