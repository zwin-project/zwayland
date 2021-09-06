#ifndef ZWC_VIRTUAL_OBJECT_H
#define ZWC_VIRTUAL_OBJECT_H

#include <wayland-client.h>
#include <z11-opengl-client-protocol.h>

#include "shm_pool.h"

struct point {
  float x, y, z;
};

struct uv_coord {
  float u, v;
};

struct vertex {
  struct point point;
  struct uv_coord uv;
};

struct zwc_virtual_object {
  struct z11_virtual_object *z11_virtual_object;
  int32_t width;
  int32_t height;
  struct zwc_shm_pool *shm_pool;
  struct vertex *vertices_data;  // size is always 6
  struct wl_raw_buffer *vertices_raw_buffer;
  struct z11_opengl_vertex_buffer *vertex_buffer;
  struct z11_opengl_texture_2d *texture;
  struct wl_raw_buffer *texture_raw_buffer;
  struct z11_opengl_shader_program *shader_program;
  struct z11_opengl_render_component *render_component;
  float del_z;
  float del_x;
};

#endif  //  ZWC_VIRTUAL_OBJECT_H
