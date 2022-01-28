#ifndef ZWAYLAND_KEYBOARD_H
#define ZWAYLAND_KEYBOARD_H

#include <wayland-server.h>
#include <wayland-util.h>

#include "seat.h"
#include "surface.h"

struct zwl_keyboard {
  struct wl_list link;
  struct wl_client *client;
  struct wl_list resource_list;
  struct wl_listener client_destroy_listener;

  struct {
    uint32_t format;
    int fd;  // -1 if an keymap doesn't exist
    uint32_t size;
  } keymap_info;
};

struct zwl_keyboard *zwl_keyboard_create(struct wl_client *client,
                                         struct zwl_seat *seat);

void zwl_keyboard_destroy(struct zwl_keyboard *keyboard);

struct wl_resource *zwl_keyboard_add_resource(struct zwl_keyboard *keyboard,
                                              struct wl_client *client,
                                              uint32_t id);

void zwl_keyboard_send_keymap(struct zwl_keyboard *keyboard,
                              struct wl_resource *resource);

void zwl_keyboard_send_enter(struct zwl_keyboard *keyboard, uint32_t serial,
                             struct zwl_surface *surface,
                             struct wl_array *keys);

void zwl_keyboard_send_leave(struct zwl_keyboard *keyboard, uint32_t serial,
                             struct zwl_surface *surface);

void zwl_keyboard_send_key(struct zwl_keyboard *keyboard, uint32_t serial,
                           uint32_t time, uint32_t key, uint32_t state);

void zwl_keyboard_send_modifiers(struct zwl_keyboard *keyboard, uint32_t serial,
                                 uint32_t mods_depressed, uint32_t mods_latched,
                                 uint32_t mods_locked, uint32_t group);

#endif  //  ZWAYLAND_KEYBOARD_H
