scanner = dependency('wayland-scanner')
scanner_path = scanner.get_variable(pkgconfig : 'wayland_scanner')
xdg_shell_xml = files('/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml')
xdg_output_xml = files('/usr/share/wayland-protocols/unstable/xdg-output/xdg-output-unstable-v1.xml')

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
