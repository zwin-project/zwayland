#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <wayland-client.h>
#include <zwc.h>

#include "zwc_global.h"
#include "zwc_os.h"

struct zwc_display {
  struct wl_display* wl_display;
  struct zwc_global* global;
  int fd;
  struct z11_virtual_object* virtual_object;
};

static int connect_to_socket(const char* name)
{
  struct sockaddr_un addr;
  socklen_t size;
  const char* runtime_dir;
  int name_size, fd;
  bool path_is_absolute;

  if (name == NULL) name = getenv("Z11_WAYLAND_DISPLAY");
  if (name == NULL) name = "wayland-0";

  path_is_absolute = name[0] == '/';

  runtime_dir = getenv("Z11_XDG_RUNTIME_DIR");
  if (!runtime_dir && !path_is_absolute) {
    fprintf(stderr, "zwc error: Z11_XDG_RUNTIME_DIR not set in the environment.\n");
    return -1;
  }

  fd = zwc_os_socket_cloexec(PF_LOCAL, SOCK_STREAM, 0);
  if (fd < 0) return -1;

  memset(&addr, 0, sizeof addr);
  addr.sun_family = AF_LOCAL;
  if (!path_is_absolute) {
    name_size = snprintf(addr.sun_path, sizeof addr.sun_path, "%s/%s", runtime_dir, name) + 1;
  } else {
    /* absolute path */
    name_size = snprintf(addr.sun_path, sizeof addr.sun_path, "%s", name) + 1;
  }

  assert(name_size > 0);
  if (name_size > (int)sizeof addr.sun_path) {
    if (!path_is_absolute) {
      fprintf(stderr,
              "zwc error: socket path \"%s/%s\" plus null terminator"
              " exceeds %i bytes\n",
              runtime_dir, name, (int)sizeof(addr.sun_path));
    } else {
      fprintf(stderr,
              "zwc error: socket path \"%s\" plus null terminator"
              " exceeds %i bytes\n",
              name, (int)sizeof(addr.sun_path));
    }
    close(fd);
    /* to prevent programs reporting
     * "failed to add socket: Success" */
    errno = ENAMETOOLONG;
    return -1;
  };

  size = offsetof(struct sockaddr_un, sun_path) + name_size;

  if (connect(fd, (struct sockaddr*)&addr, size) < 0) {
    close(fd);
    return -1;
  }

  return fd;
}

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
  int fd = connect_to_socket(name);
  if (fd < 0) goto fail;

  zwc_display = calloc(1, sizeof *zwc_display);
  if (zwc_display == NULL) goto fail;

  zwc_display->wl_display = wl_display_connect_to_fd(fd);
  if (zwc_display->wl_display == NULL) {
    fprintf(stderr, "zwc error: can't connect to display\n");
    goto out_zwc_display;
  }

  zwc_display->global = zwc_global_create(zwc_display->wl_display);
  if (zwc_display->global == NULL) {
    goto out_wl_display;
  }

  zwc_display->fd = fd;

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

int zwc_display_get_fd(struct zwc_display* display) { return display->fd; }
