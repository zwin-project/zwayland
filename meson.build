project(
  'zwayland',
  'c',
  version : '0.0.1',
  license : 'Apache-2.0',
  default_options : ['warning_level=3', 'werror=true', 'buildtype=debug'],
  meson_version : '>=0.57.0',
)

wayland_server_dep = dependency('wayland-server')
zsurface_dep = dependency('zsurface')

subdir('protocol')
subdir('server')
