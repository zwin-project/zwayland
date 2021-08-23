#define _GNU_SOURCE 1

#include "shm_pool.h"

#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "util.h"
#include "zwc_global.h"

static int create_shared_fd(off_t size)
{
  const char* name = "zwayland";

  int fd = memfd_create(name, MFD_CLOEXEC | MFD_ALLOW_SEALING);
  if (fd < 0) {
    return fd;
  } else {
    unlink(name);
  }

  if (ftruncate(fd, size) < 0) {
    close(fd);
    return -1;
  }

  return fd;
}

bool zwc_shm_pool_resize(struct zwc_shm_pool* shm_pool, int32_t offset, int32_t size)
{
  if (ftruncate(shm_pool->fd, offset + size) < 0) return false;
  wl_shm_pool_resize(shm_pool->pool, offset + size);
  return true;
}

bool zwc_shm_pool_needs_resize(struct zwc_shm_pool* shm_pool, int32_t offset, int32_t size)
{
  return shm_pool->size < (offset + size);
}

struct wl_raw_buffer* zwc_shm_pool_create_wl_raw_buffer(struct zwc_shm_pool* shm_pool, int32_t offset,
                                                        int32_t size)
{
  return wl_shm_pool_create_raw_buffer(shm_pool->pool, offset, size);
  // TODO: need error handling
}

void* zwc_shm_pool_get_buffer(struct zwc_shm_pool* shm_pool, int32_t offset)
{
  return (uint8_t*)shm_pool->shm_data + offset;
}

struct zwc_shm_pool* zwc_shm_pool_create(struct zwc_global* global, off_t size)
{
  struct zwc_shm_pool* shm_pool;

  shm_pool = zalloc(sizeof *shm_pool);
  if (shm_pool == NULL) goto out;

  shm_pool->fd = 0;
  shm_pool->shm_data = NULL;
  shm_pool->size = size;

  int fd = create_shared_fd(size);
  if (fd < 0) goto out_shm_pool;
  shm_pool->fd = fd;

  shm_pool->shm_data = mmap(NULL, shm_pool->size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_pool->fd, 0);
  if (shm_pool->shm_data == MAP_FAILED) goto out_fd;

  shm_pool->pool = wl_shm_create_pool(global->shm, fd, size);

  return shm_pool;

out_fd:
  close(shm_pool->fd);

out_shm_pool:
  free(shm_pool);

out:
  return NULL;
}

void zwc_shm_pool_destroy(struct zwc_shm_pool* shm_pool)
{
  wl_shm_pool_destroy(shm_pool->pool);
  munmap(shm_pool->shm_data, shm_pool->size);
  close(shm_pool->fd);
  free(shm_pool);
}
