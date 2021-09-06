#include "keyboard.h"

#include <wayland-server.h>

#include "util.h"

static void zwl_keyboard_destroy(struct zwl_keyboard *keyboard);

static void zwl_keyboard_handle_destroy(struct wl_resource *resource)
{
  struct zwl_keyboard *keyboard;

  keyboard = wl_resource_get_user_data(resource);

  zwl_keyboard_destroy(keyboard);
}

static void zwl_keyboard_protocol_release(struct wl_client *client, struct wl_resource *resource)
{
  UNUSED(client);
  UNUSED(resource);
}

static const struct wl_keyboard_interface zwl_keyboard_interface = {
    .release = zwl_keyboard_protocol_release,
};

struct zwl_keyboard *zwl_keyboard_create(struct wl_client *client, uint32_t id)
{
  struct zwl_keyboard *keyboard;
  struct wl_resource *resource;

  keyboard = zalloc(sizeof *keyboard);
  if (keyboard == NULL) {
    wl_client_post_no_memory(client);
    goto out;
  }

  resource = wl_resource_create(client, &wl_keyboard_interface, 7, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto out_keyboard;
  }

  wl_resource_set_implementation(resource, &zwl_keyboard_interface, keyboard, zwl_keyboard_handle_destroy);

  return keyboard;

out_keyboard:
  free(keyboard);

out:
  return NULL;
}

static void zwl_keyboard_destroy(struct zwl_keyboard *keyboard) { free(keyboard); }
