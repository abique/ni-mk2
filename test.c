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

  if (!ni_mk2_open(&mk2, argv[1])) {
    fprintf(stderr, "%s: %m\n", argv[1]);
    return 1;
  }

  for (int i = 0; i < 16; ++i) {
    mk2.pads_color[i].r = 0x80 / 16 * i;
    mk2.pads_color[i].g = 0x80 / 16 * i;
    mk2.pads_color[i].b = 0x80 / 16 * i;
  }
  ni_mk2_pads_set_color(&mk2);

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
      printf("got %d bytes\n", nbytes);
      dump(&msg, nbytes);
      break;
    }
  }

  ni_mk2_close(&mk2);

  return 0;
}
