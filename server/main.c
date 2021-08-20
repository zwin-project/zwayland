#include <stdio.h>
#include <wayland-server.h>

#include "compositor.h"
#include "compositor_global.h"
#include "util.h"
#include "xdg_wm_base_global.h"

int main(int argc, char const *argv[])
{
  UNUSED(argc);
  UNUSED(argv);
  struct wl_display *display;
  struct zwl_compositor_global *compositor;
  struct zwl_wm_base_global *wm_base;
  const char *socket;

  display = wl_display_create();
  UNUSED(display);

  compositor = zwl_compositor_global_create(display);
  if (compositor == NULL) {
    fprintf(stderr, "failed to create a global compostor\n");
    return EXIT_FAILURE;
  }

  wm_base = zwl_wm_base_global_create(display);
  if (wm_base == NULL) {
    fprintf(stderr, "failed to create a global wm base\n");
    return EXIT_FAILURE;
  }

  if (wl_display_init_shm(display) == -1) {
    fprintf(stderr, "failed to initialize shared memory");
    return EXIT_FAILURE;
  }

  socket = wl_display_add_socket_auto(display);

  if (socket == NULL) {
    fprintf(stderr, "failed to add socket\n");
    return EXIT_FAILURE;
  }

  while (1) {
    wl_display_flush_clients(display);
    wl_event_loop_dispatch(wl_display_get_event_loop(display), 0);
    wl_signal_emit(&compositor->flush_signal, compositor);
  }

  return 0;
}
