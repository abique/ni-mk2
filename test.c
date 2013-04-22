#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "ni-mk2.h"

void dump(const void *data, ssize_t nbytes)
{
  const uint8_t *d = data;

  for (int i = 0; i < nbytes; ++i)
    printf(" %02x", d[i]);
  printf("\n");
}

void dump_pads(const struct ni_mk2_msg *msg)
{
  for (int i = 0; i < 16; ++i)
    if (msg->pads[i] > 0)
      printf("%d: %d, ", i, msg->pads[i]);
  printf("\n");
}

void dump_bts(const struct ni_mk2_msg *msg)
{
#define P(X)                                    \
  if (msg->bts.X)                               \
    printf(#X ", ")

  P(top0);
  P(top1);
  P(top2);
  P(top3);
  P(top4);
  P(top5);
  P(top6);
  P(top7);
  P(control);
  P(step);
  P(browse);
  P(sampling);
  P(browse_left);
  P(browse_right);
  P(all);
  P(auto_wr);
  P(volume);
  P(swing);
  P(tempo);
  P(master_left);
  P(master_right);
  P(enter);
  P(note_repeat);
  P(master_wheel);
  P(a);
  P(b);
  P(c);
  P(d);
  P(e);
  P(f);
  P(g);
  P(h);
  P(restart);
  P(transport_left);
  P(transport_right);
  P(grid);
  P(play);
  P(rec);
  P(erase);
  P(shift);
  P(scene);
  P(pattern);
  P(pad_mode);
  P(navigate);
  P(duplicate);
  P(select);
  P(solo);
  P(mute);

  printf("master wheel: %d, wheels: [", msg->bts.master_wheel_pos);
  for (int i = 0; i < 8; ++i)
    printf("%d, ", msg->bts.wheels[i]);
  printf("]\n");

#undef P
}

int main(int argc, char **argv)
{
  struct ni_mk2 mk2;

  if (argc != 2) {
    fprintf(stderr, "usage: %s /dev/hidrawX\n", argv[0]);
    return 2;
  }

  if (!ni_mk2_open(&mk2, argv[1])) {
    fprintf(stderr, "%s: %m\n", argv[1]);
    return 1;
  }

  for (int i = 0; i < 16; ++i) {
    mk2.pads_lights[i].r = 0x80 / 16 * i;
    mk2.pads_lights[i].g = 0x80 / 16 * i;
    mk2.pads_lights[i].b = 0x80 / 16 * i;
  }
  ni_mk2_pads_set_lights(&mk2);

  mk2.groups_lights[0].left.r = 0x80;
  mk2.groups_lights[0].left.g = 0x00;
  mk2.groups_lights[0].left.b = 0x00;
  mk2.groups_lights[0].right.r = 0x00;
  mk2.groups_lights[0].right.g = 0x80;
  mk2.groups_lights[0].right.b = 0x00;
  mk2.groups_lights[1].left.r = 0x00;
  mk2.groups_lights[1].left.g = 0x00;
  mk2.groups_lights[1].left.b = 0x80;
  mk2.groups_lights[1].right.r = 0x80;
  mk2.groups_lights[1].right.g = 0x00;
  mk2.groups_lights[1].right.b = 0x00;
  mk2.groups_lights[2].left.r = 0x00;
  mk2.groups_lights[2].left.g = 0x80;
  mk2.groups_lights[2].left.b = 0x00;
  mk2.groups_lights[2].right.r = 0x00;
  mk2.groups_lights[2].right.g = 0x00;
  mk2.groups_lights[2].right.b = 0x80;
  mk2.groups_lights[3].left.r = 0x80;
  mk2.groups_lights[3].left.g = 0x00;
  mk2.groups_lights[3].left.b = 0x00;
  mk2.groups_lights[3].right.r = 0x00;
  mk2.groups_lights[3].right.g = 0x80;
  mk2.groups_lights[3].right.b = 0x00;
  mk2.groups_lights[4].left.r = 0x80;
  mk2.groups_lights[4].left.g = 0x00;
  mk2.groups_lights[4].left.b = 0x00;
  mk2.groups_lights[4].right.r = 0x00;
  mk2.groups_lights[4].right.g = 0x80;
  mk2.groups_lights[4].right.b = 0x00;
  mk2.groups_lights[5].left.r = 0x00;
  mk2.groups_lights[5].left.g = 0x00;
  mk2.groups_lights[5].left.b = 0x80;
  mk2.groups_lights[5].right.r = 0x80;
  mk2.groups_lights[5].right.g = 0x00;
  mk2.groups_lights[5].right.b = 0x00;
  mk2.groups_lights[6].left.r = 0x00;
  mk2.groups_lights[6].left.g = 0x80;
  mk2.groups_lights[6].left.b = 0x00;
  mk2.groups_lights[6].right.r = 0x00;
  mk2.groups_lights[6].right.g = 0x00;
  mk2.groups_lights[6].right.b = 0x80;
  mk2.groups_lights[7].left.r = 0x80;
  mk2.groups_lights[7].left.g = 0x00;
  mk2.groups_lights[7].left.b = 0x00;
  mk2.groups_lights[7].right.r = 0x00;
  mk2.groups_lights[7].right.g = 0x80;
  mk2.groups_lights[7].right.b = 0x00;

  mk2.transport_lights.restart = 0x10;
  mk2.transport_lights.left = 0x20;
  mk2.transport_lights.right = 0x30;
  mk2.transport_lights.grid = 0x40;
  mk2.transport_lights.play = 0x50;
  mk2.transport_lights.rec = 0x60;
  mk2.transport_lights.erase = 0x70;
  mk2.transport_lights.shift = 0x80;

  ni_mk2_groups_transport_set_lights(&mk2);

  while (true) {
    struct ni_mk2_msg msg;
    ssize_t nbytes = ni_mk2_read(&mk2, &msg);

    switch (msg.type) {
    case NI_MK2_MSG_PADS:
      assert(nbytes == NI_MK2_MSG_PADS_SIZE);
      //dump_pads(&msg);
      break;

    case NI_MK2_MSG_BTS:
      assert(nbytes == NI_MK2_MSG_BTS_SIZE);
      //dump(&msg, nbytes);
      dump_bts(&msg);
      break;

    default:
      printf("got %d bytes\n", (int)nbytes);
      dump(&msg, nbytes);
      break;
    }
  }

  ni_mk2_close(&mk2);

  return 0;
}
