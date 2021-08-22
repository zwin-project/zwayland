dep_libzwc = [
  dependency('wayland-client'),
]

srcs_libzwc = files([
  'zwc_display.c',
  'zwc_global.c',
  'zwc_virtual_object.c',
]) + [
  z11_protocol_c,
  z11_client_protocol_h,
  z11_opengl_protocol_c,
  z11_opengl_client_protocol_h,
]

lib_zwc = static_library(
  'zwc',
  srcs_libzwc,
  include_directories : public_inc,
  install : false,
  version : zwc_version,
  dependencies : dep_libzwc,
)

libzwc_dep = declare_dependency(
  link_with : lib_zwc,
  include_directories : public_inc,
  dependencies : dep_libzwc,
)
