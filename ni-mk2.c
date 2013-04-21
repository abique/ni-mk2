#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "ni-mk2.h"

bool ni_mk2_open(struct ni_mk2 *ctx, const char *hid_path)
{
  ctx->fd = open(hid_path, O_RDWR, 0644);
  if (!ctx->fd < 0)
    return false;
  return true;
}

void ni_mk2_close(struct ni_mk2 *ctx)
{
  close(ctx->fd);
  ctx->fd = -1;
}

ssize_t ni_mk2_read(struct ni_mk2 *ctx, struct ni_mk2_msg *msg)
{
  ssize_t nbytes = read(ctx->fd, msg, sizeof (*msg));

  if (nbytes <= 0)
    return nbytes;

  switch (msg->type) {
  case NI_MK2_MSG_PADS:
    for (int i = 0; i < 32; ++i)
      msg->pads[i] = le16toh(msg->pads[i]) & 0x0fff;
    break;

  case NI_MK2_MSG_BTS:
    for (int i = 0; i < 8; ++i)
      msg->bts.wheels[i] = le16toh(msg->bts.wheels[i]);
    break;

  default:
    break;
  }

  return nbytes;
}
