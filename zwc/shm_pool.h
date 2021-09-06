#ifndef ZWC_SHM_POOL_H
#define ZWC_SHM_POOL_H

#include <stdbool.h>
#include <sys/mman.h>
#include <wayland-client.h>

#include "zwc_global.h"

struct zwc_shm_pool {
  int fd;
  void* shm_data;
  off_t size;  // bytes
  struct wl_shm_pool* pool;
};

bool zwc_shm_pool_resize(struct zwc_shm_pool* shm_pool, int32_t offset, int32_t size);

bool zwc_shm_pool_needs_resize(struct zwc_shm_pool* shm_pool, int32_t offset, int32_t size);

struct wl_raw_buffer* zwc_shm_pool_create_wl_raw_buffer(struct zwc_shm_pool* shm_pool, int32_t offset,
                                                        int32_t size);

// this buffer is safe until the next resize call
void* zwc_shm_pool_get_buffer(struct zwc_shm_pool* shm_pool, int32_t offset);

struct zwc_shm_pool* zwc_shm_pool_create(struct zwc_global* global, off_t size);

void zwc_shm_pool_destroy(struct zwc_shm_pool* shm_pool);

#endif  //  ZWC_SHM_POOL_H
