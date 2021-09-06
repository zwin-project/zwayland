#include "shell.h"

#include <stdio.h>
#include <wayland-server.h>
#include <xdg-output-server-protocol.h>

#include "shell_surface.h"
#include "util.h"

static void zwl_shell_protocol_get_shell_surface(struct wl_client *client, struct wl_resource *resource,
                                                 uint32_t id, struct wl_resource *surface_resource)
{
  UNUSED(resource);
  struct zwl_surface *surface = wl_resource_get_user_data(surface_resource);
  struct zwl_shell_surface *shell_surface;

  shell_surface = zwl_shell_surface_create(client, id, surface);
  if (shell_surface == NULL) {
    fprintf(stderr, "failed to create a shell surface\n");
  }
}

static const struct wl_shell_interface zwl_shell_interface = {
    .get_shell_surface = zwl_shell_protocol_get_shell_surface,
};

static void zwl_shell_bind(struct wl_client *client, void *data, uint32_t version, uint32_t id)
{
  struct zwl_shell *shell = data;
  struct wl_resource *resource;

  resource = wl_resource_create(client, &wl_shell_interface, version, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    return;
  }

  wl_resource_set_implementation(resource, &zwl_shell_interface, shell, NULL);
}

struct zwl_shell *zwl_shell_create(struct wl_display *display)
{
  struct zwl_shell *shell;
  struct wl_global *global;

  shell = zalloc(sizeof *shell);
  if (shell == NULL) goto out;

  global = wl_global_create(display, &wl_shell_interface, 1, shell, zwl_shell_bind);
  if (global == NULL) goto out_shell;

  return shell;

out_shell:
  free(shell);

out:
  return NULL;
}
