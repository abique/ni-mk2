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

  mk2.pads_lights[0].r = 0x80;
  mk2.pads_lights[0].g = 0x00;
  mk2.pads_lights[0].b = 0x00;
  mk2.pads_lights[1].r = 0x00;
  mk2.pads_lights[1].g = 0x80;
  mk2.pads_lights[1].b = 0x00;
  mk2.pads_lights[2].r = 0x00;
  mk2.pads_lights[2].g = 0x00;
  mk2.pads_lights[2].b = 0x80;
  mk2.pads_lights[3].r = 0x80;
  mk2.pads_lights[3].g = 0x80;
  mk2.pads_lights[3].b = 0x00;
  mk2.pads_lights[4].r = 0x80;
  mk2.pads_lights[4].g = 0x00;
  mk2.pads_lights[4].b = 0x80;
  mk2.pads_lights[5].r = 0x00;
  mk2.pads_lights[5].g = 0x80;
  mk2.pads_lights[5].b = 0x80;
  mk2.pads_lights[6].r = 0x80;
  mk2.pads_lights[6].g = 0x80;
  mk2.pads_lights[6].b = 0x80;
  mk2.pads_lights[7].r = 0x40;
  mk2.pads_lights[7].g = 0x80;
  mk2.pads_lights[7].b = 0x80;
  mk2.pads_lights[8].r = 0x80;
  mk2.pads_lights[8].g = 0x40;
  mk2.pads_lights[8].b = 0x80;
  mk2.pads_lights[9].r = 0x80;
  mk2.pads_lights[9].g = 0x80;
  mk2.pads_lights[9].b = 0x40;
  mk2.pads_lights[10].r = 0x40;
  mk2.pads_lights[10].g = 0x40;
  mk2.pads_lights[10].b = 0x80;
  mk2.pads_lights[11].r = 0x40;
  mk2.pads_lights[11].g = 0x80;
  mk2.pads_lights[11].b = 0x40;
  mk2.pads_lights[12].r = 0x80;
  mk2.pads_lights[12].g = 0x40;
  mk2.pads_lights[12].b = 0x40;
  mk2.pads_lights[13].r = 0x20;
  mk2.pads_lights[13].g = 0x40;
  mk2.pads_lights[13].b = 0x80;
  mk2.pads_lights[14].r = 0x40;
  mk2.pads_lights[14].g = 0x20;
  mk2.pads_lights[14].b = 0x80;
  mk2.pads_lights[15].r = 0x80;
  mk2.pads_lights[15].g = 0x20;
  mk2.pads_lights[15].b = 0x40;
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

  mk2.transport_lights.restart = 0x20;
  mk2.transport_lights.left = 0x20;
  mk2.transport_lights.right = 0x20;
  mk2.transport_lights.grid = 0x20;
  mk2.transport_lights.play = 0x20;
  mk2.transport_lights.rec = 0x20;
  mk2.transport_lights.erase = 0x20;
  mk2.transport_lights.shift = 0x20;

  ni_mk2_groups_transport_set_lights(&mk2);

  mk2.buttons_lights.control = 0x20;
  mk2.buttons_lights.step = 0x20;
  mk2.buttons_lights.browse = 0x20;
  mk2.buttons_lights.sampling = 0x20;
  mk2.buttons_lights.browse_left = 0x20;
  mk2.buttons_lights.browse_right = 0x20;
  mk2.buttons_lights.all = 0x20;
  mk2.buttons_lights.auto_wr = 0x20;

  mk2.buttons_lights.top0 = 0x20;
  mk2.buttons_lights.top1 = 0x20;
  mk2.buttons_lights.top2 = 0x20;
  mk2.buttons_lights.top3 = 0x20;
  mk2.buttons_lights.top4 = 0x20;
  mk2.buttons_lights.top5 = 0x20;
  mk2.buttons_lights.top6 = 0x20;
  mk2.buttons_lights.top7 = 0x20;

  mk2.buttons_lights.scene = 0x20;
  mk2.buttons_lights.pattern = 0x20;
  mk2.buttons_lights.pad_mode = 0x20;
  mk2.buttons_lights.navigate = 0x20;
  mk2.buttons_lights.duplicate = 0x20;
  mk2.buttons_lights.select = 0x20;
  mk2.buttons_lights.solo = 0x20;
  mk2.buttons_lights.mute = 0x20;

  mk2.buttons_lights.volume = 0x20;
  mk2.buttons_lights.swing = 0x20;
  mk2.buttons_lights.tempo = 0x20;
  mk2.buttons_lights.master_left = 0x20;
  mk2.buttons_lights.master_right = 0x20;
  mk2.buttons_lights.enter = 0x20;
  mk2.buttons_lights.note_repeat = 0x20;

  ni_mk2_buttons_set_lights(&mk2);

  memset(mk2.screen_left, 0xff, sizeof (mk2.screen_left));
  memset(mk2.screen_right, 0xff, sizeof (mk2.screen_right));
  ni_mk2_screens_draw(&mk2);

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
