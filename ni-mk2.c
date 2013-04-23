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

bool ni_mk2_pads_set_lights(struct ni_mk2 *ctx)
{
  uint8_t buffer[NI_MK2_MSG_PADS_SET_LIGHTS_SIZE];
  buffer[0] = NI_MK2_MSG_PADS_SET_LIGHTS;
  memcpy(buffer + 1, ctx->pads_lights, sizeof (ctx->pads_lights));
  return write(ctx->fd, buffer, sizeof (buffer)) == sizeof (buffer);
}

bool ni_mk2_groups_transport_set_lights(struct ni_mk2 *ctx)
{
  uint8_t buffer[NI_MK2_MSG_GRP_TP_SET_LIGHTS_SIZE];
  buffer[0] = NI_MK2_MSG_GRP_TP_SET_LIGHTS;
  memcpy(buffer + 1, ctx->groups_lights, sizeof (ctx->groups_lights));
  memcpy(buffer + 1 + sizeof (ctx->groups_lights), &ctx->transport_lights,
         sizeof (ctx->transport_lights));
  return write(ctx->fd, buffer, sizeof (buffer)) == sizeof (buffer);
}

bool ni_mk2_buttons_set_lights(struct ni_mk2 *ctx)
{
  uint8_t buffer[NI_MK2_MSG_BTS_SET_LIGHTS_SIZE];
  buffer[0] = NI_MK2_MSG_BTS_SET_LIGHTS;
  memcpy(buffer + 1, &ctx->buttons_lights, sizeof (ctx->buttons_lights));
  return write(ctx->fd, buffer, sizeof (buffer)) == sizeof (buffer);
}

bool ni_mk2_screens_draw(struct ni_mk2 *ctx)
{
  uint8_t buffer[NI_MK2_MSG_SCREEN_DRAW_SIZE];

  memset(buffer + 1, 0, 8);
  buffer[5] = 0x20;
  buffer[7] = 0x08;

  for (int i = 0; i < 64 / 8; ++i) {
    buffer[3] = i * 8;

    buffer[0] = NI_MK2_MSG_SCREEN_LEFT_DRAW;
    memcpy(buffer + 1 + 8, ctx->screen_left + 256 * i, 256);
    write(ctx->fd, buffer, sizeof (buffer)) == sizeof (buffer);

    buffer[0] = NI_MK2_MSG_SCREEN_RIGHT_DRAW;
    memcpy(buffer + 1 + 8, ctx->screen_right + 256 * i, 256);
    write(ctx->fd, buffer, sizeof (buffer)) == sizeof (buffer);
  }
  return true;
}
