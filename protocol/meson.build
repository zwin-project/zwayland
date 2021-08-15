scanner = dependency('wayland-scanner')
scanner_path = scanner.get_variable(pkgconfig : 'wayland_scanner')
z11_xml = files('/usr/local/share/z11/z11.xml')
z11_opengl_xml = files('/usr/local/share/z11/z11_opengl.xml')
xdg_shell_xml = files('/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml')
xdg_output_xml = files('/usr/share/wayland-protocols/unstable/xdg-output/xdg-output-unstable-v1.xml')

z11_client_protocol_h = custom_target(
  'z11-client-protocol',
  output : 'z11-client-protocol.h',
  input : z11_xml,
  command : [scanner_path, 'client-header', '@INPUT@', '@OUTPUT@'],
)

z11_protocol_c = custom_target(
  'z11-protocol',
  output : 'z11-protocol.c',
  input : z11_xml,
  command : [scanner_path, 'public-code', '@INPUT@', '@OUTPUT@'],
)

z11_opengl_client_protocol_h = custom_target(
  'z11-opengl-client-protocol',
  output : 'z11-opengl-client-protocol.h',
  input : z11_opengl_xml,
  command : [scanner_path, 'client-header', '@INPUT@', '@OUTPUT@'],
)

z11_opengl_protocol_c = custom_target(
  'z11-opengl-protocol',
  output : 'z11-opengl-protocol.c',
  input : z11_opengl_xml,
  command : [scanner_path, 'public-code', '@INPUT@', '@OUTPUT@'],
)

xdg_shell_server_protocol_h = custom_target(
  'xdg-shell-server-protocol',
  output : 'xdg-shell-server-protocol.h',
  input : xdg_shell_xml,
  command : [scanner_path, 'server-header', '@INPUT@', '@OUTPUT@'],
)

xdg_shell_protocol_c = custom_target(
  'xdg-shell-protocol',
  output : 'xdg-shell-protocol.c',
  input : xdg_shell_xml,
  command : [scanner_path, 'code', '@INPUT@', '@OUTPUT@'],
)

xdg_output_server_protocol_h = custom_target(
  'xdg-output-server-protocol',
  output : 'xdg-output-server-protocol.h',
  input : xdg_output_xml,
  command : [scanner_path, 'server-header', '@INPUT@', '@OUTPUT@'],
)

xdg_output_protocol_c = custom_target(
  'xdg-output-protocol',
  output : 'xdg-output-protocol.c',
  input : xdg_output_xml,
  command : [scanner_path, 'code', '@INPUT@', '@OUTPUT@'],
)
