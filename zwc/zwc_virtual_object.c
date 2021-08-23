#include "zwc_virtual_object.h"

#include <stdio.h>
#include <wayland-client.h>
#include <z11-client-protocol.h>
#include <z11-opengl-client-protocol.h>
#include <zwc.h>

#include "shm_pool.h"
#include "util.h"
#include "zwc_display.h"
#include "zwc_global.h"

const char *vertex_shader;
const char *fragment_shader;

struct zwc_virtual_object *zwc_virtual_object_create(struct zwc_display *display)
{
  UNUSED(display);
  struct zwc_virtual_object *virtual_object;
  struct z11_virtual_object *z11_virtual_object;
  const int32_t vertices_size = sizeof(struct vertex) * 6;

  virtual_object = zalloc(sizeof *virtual_object);
  if (virtual_object == NULL) {
    goto out;
  }

  z11_virtual_object = z11_compositor_create_virtual_object(display->global->compositor);
  if (z11_virtual_object == NULL) {
    goto out_virtual_object;
  }

  virtual_object->z11_virtual_object = z11_virtual_object;

  virtual_object->width = 0;
  virtual_object->height = 0;

  virtual_object->shm_pool = zwc_shm_pool_create(display->global, vertices_size);
  if (virtual_object->shm_pool == NULL) goto out_z11_virtual_object;

  virtual_object->vertices_data = zwc_shm_pool_get_buffer(virtual_object->shm_pool, 0);

  virtual_object->vertices_raw_buffer =
      zwc_shm_pool_create_wl_raw_buffer(virtual_object->shm_pool, 0, vertices_size);

  virtual_object->vertex_buffer = z11_opengl_create_vertex_buffer(display->global->gl);

  virtual_object->texture = z11_opengl_create_texture_2d(display->global->gl);

  virtual_object->shader_program =
      z11_opengl_create_shader_program(display->global->gl, vertex_shader, fragment_shader);

  virtual_object->render_component = z11_opengl_render_component_manager_create_opengl_render_component(
      display->global->render_component_manager, virtual_object->z11_virtual_object);

  z11_opengl_render_component_attach_vertex_buffer(virtual_object->render_component,
                                                   virtual_object->vertex_buffer);
  z11_opengl_render_component_attach_texture_2d(virtual_object->render_component, virtual_object->texture);
  z11_opengl_render_component_attach_shader_program(virtual_object->render_component,
                                                    virtual_object->shader_program);
  z11_opengl_render_component_append_vertex_input_attribute(
      virtual_object->render_component, 0, Z11_OPENGL_VERTEX_INPUT_ATTRIBUTE_FORMAT_FLOAT_VECTOR3,
      offsetof(struct vertex, point));
  z11_opengl_render_component_append_vertex_input_attribute(
      virtual_object->render_component, 1, Z11_OPENGL_VERTEX_INPUT_ATTRIBUTE_FORMAT_FLOAT_VECTOR2,
      offsetof(struct vertex, uv));
  z11_opengl_render_component_set_topology(virtual_object->render_component, Z11_OPENGL_TOPOLOGY_TRIANGLES);

  return virtual_object;

out_z11_virtual_object:
  z11_virtual_object_destroy(z11_virtual_object);

out_virtual_object:
  free(virtual_object);

out:
  return NULL;
}

void zwc_virtual_object_destroy(struct zwc_virtual_object *virtual_object)
{
  z11_opengl_shader_program_destroy(virtual_object->shader_program);
  z11_opengl_texture_2d_destroy(virtual_object->texture);
  z11_opengl_vertex_buffer_destroy(virtual_object->vertex_buffer);
  wl_raw_buffer_destroy(virtual_object->vertices_raw_buffer);
  zwc_shm_pool_destroy(virtual_object->shm_pool);
  z11_opengl_render_component_destroy(virtual_object->render_component);
  z11_virtual_object_destroy(virtual_object->z11_virtual_object);
  free(virtual_object);
}

void zwc_virtual_object_commit(struct zwc_virtual_object *virtual_object)
{
  z11_virtual_object_commit(virtual_object->z11_virtual_object);
}

/* (u, v)
  D (0,0) C (1, 0)

  A (0,1) B (1,1)
*/
void zwc_virtual_object_attach_surface(struct zwc_virtual_object *virtual_object, int32_t width,
                                       int32_t height, uint32_t size, uint32_t format, uint8_t *data)
{
  UNUSED(size);
  UNUSED(data);
  const int z = 300;
  if (format != WL_SHM_FORMAT_ARGB8888 && format != WL_SHM_FORMAT_XRGB8888) return;
  if (virtual_object->width != width || virtual_object->height != height) {
    struct vertex A = {{-width / 2, -height / 2, z}, {0, 1}};
    struct vertex B = {{width / 2, -height / 2, z}, {1, 1}};
    struct vertex C = {{width / 2, height / 2, z}, {1, 0}};
    struct vertex D = {{-width / 2, height / 2, z}, {0, 0}};
    virtual_object->vertices_data[0] = A;
    virtual_object->vertices_data[1] = B;
    virtual_object->vertices_data[2] = C;
    virtual_object->vertices_data[3] = A;
    virtual_object->vertices_data[4] = D;
    virtual_object->vertices_data[5] = C;
    z11_opengl_vertex_buffer_attach(virtual_object->vertex_buffer, virtual_object->vertices_raw_buffer,
                                    sizeof(struct vertex));
  }
}

struct zwc_virtual_object_callback_data {
  zwc_virtual_object_frame_callback_done_func_t func;
  void *data;
};

static void zwc_virtual_object_callback_done(void *data, struct wl_callback *callback, uint32_t callback_time)
{
  struct zwc_virtual_object_callback_data *callback_data = data;

  callback_data->func(callback_data->data, callback_time);

  wl_callback_destroy(callback);
  free(callback_data);
}

static const struct wl_callback_listener zwc_virtual_object_callback_listener = {
    .done = zwc_virtual_object_callback_done,
};

void zwc_virtual_object_add_frame_callback(struct zwc_virtual_object *virtual_object,
                                           zwc_virtual_object_frame_callback_done_func_t done_func,
                                           void *data)
{
  struct wl_callback *cb;
  struct zwc_virtual_object_callback_data *callback_data;

  cb = z11_virtual_object_frame(virtual_object->z11_virtual_object);

  callback_data = zalloc(sizeof *callback_data);  // zwc_virtual_object_callback_done will free this
  callback_data->data = data;
  callback_data->func = done_func;

  wl_callback_add_listener(cb, &zwc_virtual_object_callback_listener, callback_data);
}

const char *vertex_shader =
    "#version 410\n"
    "uniform mat4 matrix;\n"
    "layout(location = 0) in vec4 position;\n"
    "layout(location = 1) in vec2 v2UVcoordsIn;\n"
    "layout(location = 2) in vec3 v3NormalIn;\n"
    "out vec2 v2UVcoords;\n"
    "void main()\n"
    "{\n"
    "  v2UVcoords = v2UVcoordsIn;\n"
    "  gl_Position = matrix * position;\n"
    "}\n";

const char *fragment_shader =
    "#version 410 core\n"
    "uniform sampler2D myTexture;\n"
    "in vec2 v2UVcoords;\n"
    "out vec4 outputColor;\n"
    "void main()\n"
    "{\n"
    // "  outputColor = texture(myTexture, v2UVcoords);\n"
    "  outputColor = vec4(1.0, 0.647, 0.0, 1.0);\n"
    "}\n";
