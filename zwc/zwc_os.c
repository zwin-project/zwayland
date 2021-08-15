#include "zwc_os.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

static int set_cloexec_or_close(int fd)
{
  long flags;

  if (fd == -1) return -1;

  flags = fcntl(fd, F_GETFD);
  if (flags == -1) goto err;

  if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1) goto err;

  return fd;

err:
  close(fd);
  return -1;
}

int zwc_os_socket_cloexec(int domain, int type, int protocol)
{
  int fd;

  fd = socket(domain, type | SOCK_CLOEXEC, protocol);
  if (fd >= 0) return fd;
  if (errno != EINVAL) return -1;

  fd = socket(domain, type, protocol);
  return set_cloexec_or_close(fd);
}
