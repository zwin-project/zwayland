#include <stdio.h>
#include <wayland-server.h>

#include "compositor.h"
#include "compositor_global.h"
#include "output.h"
#include "seat.h"
#include "shell.h"
#include "util.h"
#include "xdg_wm_base_global.h"
#include "zxdg_output_manager.h"

int main(int argc, char const *argv[])
{
  UNUSED(argc);
  UNUSED(argv);
  struct wl_display *display;
  struct zwl_compositor_global *compositor;
  struct zwl_wm_base_global *wm_base;
  struct zwl_seat *seat;
  struct zwl_shell *shell;
  struct zwl_zxdg_output_manager *output_manager;
  struct zwl_output *output;
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

  seat = zwl_seat_create(display);
  if (seat == NULL) {
    fprintf(stderr, "failed to create a seat\n");
    return EXIT_FAILURE;
  }
  compositor->seat = seat;

  shell = zwl_shell_create(display);
  if (shell == NULL) {
    fprintf(stderr, "failed to create a shell\n");
    return EXIT_FAILURE;
  }

  output_manager = zwl_zxdg_output_manager_create(display);
  if (output_manager == NULL) {
    fprintf(stderr, "failed to create a output manager");
    return EXIT_FAILURE;
  }

  output = zwl_output_create(display);
  if (output == NULL) {
    fprintf(stderr, "failed to create a output");
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
