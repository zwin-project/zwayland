project(
  'zwayland',
  'c',
  version : '0.0.1',
  license : 'Apache-2.0',
  default_options : ['warning_level=3', 'werror=true', 'buildtype=debug'],
  meson_version : '>=0.57.0',
)

public_inc = include_directories('include')

subdir('protocol')
subdir('server')
