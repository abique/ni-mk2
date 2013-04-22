#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "ni-mk2.h"

bool ni_mk2_open(struct ni_mk2 *ctx, const char *hid_path)
{
  memset(ctx, 0, sizeof (*ctx));

  ctx->fd = open(hid_path, O_RDWR, 0644);
  if (ctx->fd < 0)
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
    // copy previous state
    memcpy(ctx->prev_pads, ctx->pads, sizeof (ctx->pads));

    // cleanup data
    for (int i = 0; i < 32; ++i)
      msg->pads[i] = le16toh(msg->pads[i]) & 0x0fff;

    // copy state
    memcpy(ctx->pads, msg->pads, sizeof (ctx->pads));
    break;

  case NI_MK2_MSG_BTS:
    // copy previous state
    memcpy(&ctx->prev_bts, &ctx->bts, sizeof (ctx->bts));

    // cleanup data
    for (int i = 0; i < 8; ++i)
      msg->bts.wheels[i] = le16toh(msg->bts.wheels[i]);

    // copy state
    memcpy(&ctx->bts, &msg->bts, sizeof (ctx->bts));
    break;

  default:
    break;
  }

  return nbytes;
}

ssize_t ni_mk2_pads_set_color(struct ni_mk2 *ctx)
{
  uint8_t buffer[NI_MK2_MSG_PADS_SET_COLOR_SIZE];
  buffer[0] = NI_MK2_MSG_PADS_SET_COLOR;
  memcpy(buffer + 1, ctx->pads_color, sizeof (buffer) - 1);
  return write(ctx->fd, buffer, sizeof (buffer)) == sizeof (buffer);
}
