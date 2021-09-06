source_files = [
  'callback.c',
  'compositor.c',
  'compositor_global.c',
  'main.c',
  'region.c',
  'surface.c',
  'util.c',
  'xdg_surface.c',
  'xdg_toplevel.c',
  'xdg_wm_base.c',
  'xdg_wm_base_global.c',
  xdg_output_protocol_c,
  xdg_output_server_protocol_h,
  xdg_shell_protocol_c,
  xdg_shell_server_protocol_h,
]

dependencies = [
  dependency('wayland-server'),
  libzwc_dep,
]

executable(
  meson.project_name(),
  source_files,
  install : true,
  dependencies : dependencies,
)
