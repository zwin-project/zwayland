source_files = [
  'callback.c',
  'compositor.c',
  'compositor_global.c',
  'keyboard.c',
  'main.c',
  'output.c',
  'pointer.c',
  'region.c',
  'seat.c',
  'shell.c',
  'shell_surface.c',
  'surface.c',
  'util.c',
  'xdg_popup.c',
  'xdg_positioner.c',
  'xdg_surface.c',
  'xdg_toplevel.c',
  'xdg_wm_base.c',
  'xdg_wm_base_global.c',
  'zxdg_output.c',
  'zxdg_output_manager.c',
  xdg_output_protocol_c,
  xdg_output_server_protocol_h,
  xdg_shell_protocol_c,
  xdg_shell_server_protocol_h,
]

dependencies = [
  wayland_server_dep,
  zsurface_dep,
]

executable(
  meson.project_name(),
  source_files,
  install : true,
  dependencies : dependencies,
)
