#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define _POSIX_C_SOURCE

#include <alsa/asoundlib.h>
#include <alsa/input.h>
#include <alsa/output.h>
#include <alsa/conf.h>
#include <alsa/seq.h>

#include "ni-mk2.h"

struct alsa_kb {
  struct ni_mk2 mk2;
  snd_seq_t *seq;
  int seq_port;
  snd_seq_event_t seq_ev;
  int note_offset;
};

static const uint8_t pad_note_map[16] = {
  12, 13, 14, 15,
  8, 9, 10, 11,
  4, 5, 6, 7,
  0, 1, 2, 3
};

int is_black_note(int note)
{
  switch (note % 12) {
  case 1:
  case 3:
  case 6:
  case 8:
  case 10:
    return 1;
  default:
    return 0;
  }
}

void set_piano_lights(struct alsa_kb *kb)
{
  for (int i = 0; i < 16; ++i) {
    int note = kb->note_offset + pad_note_map[i];

    if (is_black_note(note)) {
      kb->mk2.pads_lights[i].r = 0;
      kb->mk2.pads_lights[i].g = 0x20;
      kb->mk2.pads_lights[i].b = 0x20;
    } else {
      kb->mk2.pads_lights[i].r = 0x40;
      kb->mk2.pads_lights[i].g = 0x40;
      kb->mk2.pads_lights[i].b = 0x40;
    }

    if (kb->mk2.pads[i] > 128) {
      kb->mk2.pads_lights[i].r *= 2;
      kb->mk2.pads_lights[i].g *= 2;
      kb->mk2.pads_lights[i].b *= 2;
    }
  }
  ni_mk2_pads_set_lights(&kb->mk2);
}

#if 0
int pads_vel(int pressure)
{
  int vel;

  assert(0 <= pressure && pressure < 0x1000);
  vel = 127 * (1 - expf((float)(-pressure * 7) / (float)0x1000));
  assert(0 <= vel && vel < 128);
  return vel;
}
#else
int pads_vel(int pressure)
{
  return pressure > 128 ? 127 : 0;
}
#endif

void process_pads(struct alsa_kb *kb)
{
  int vel, vel_prev;

  for (int i = 0; i < 16; ++i) {
    vel      = pads_vel(kb->mk2.pads[i]);
    vel_prev = pads_vel(kb->mk2.prev_pads[i]);

    assert(vel < 128);
    assert(vel_prev < 128);

    if (vel == vel_prev)
      continue;

    if (!vel_prev)
      snd_seq_ev_set_noteon(&kb->seq_ev, 1, pad_note_map[i] + kb->note_offset, vel);
    else if (vel)
      snd_seq_ev_set_keypress(&kb->seq_ev, 1, pad_note_map[i] + kb->note_offset, vel);
    else
      snd_seq_ev_set_noteoff(&kb->seq_ev, 1, pad_note_map[i] + kb->note_offset, vel);
    snd_seq_event_output(kb->seq, &kb->seq_ev);
    snd_seq_drain_output(kb->seq);
  }
  set_piano_lights(kb);
}

bool process_input(struct alsa_kb *kb)
{
  struct ni_mk2_msg msg;
  ssize_t nbytes = ni_mk2_read(&kb->mk2, &msg);

  if (nbytes < 0)
    return false;

  switch (msg.type) {
  case NI_MK2_MSG_PADS:
    process_pads(kb);
    return true;

  default:
    return true;
  }
}

int main(int argc, char **argv)
{
  struct alsa_kb kb;

  memset(&kb, 0, sizeof (kb));

  kb.note_offset = 48;

  if (argc != 2) {
    fprintf(stderr, "usage: %s /dev/hidrawX\n", argv[0]);
    return 2;
  }

  if (!ni_mk2_open(&kb.mk2, argv[1])) {
    fprintf(stderr, "%s: %m\n", argv[1]);
    return 1;
  }

  if (snd_seq_open(&kb.seq, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
    fprintf(stderr, "failed to open alsa snd seq\n");
    return 1;
  }
  snd_seq_set_client_name(kb.seq, "NI Maschine MK2");

  kb.seq_port = snd_seq_create_simple_port(
    kb.seq, "NI Maschine MK2 (Out)",
    SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
    SND_SEQ_PORT_TYPE_MIDI_GENERIC);

  snd_seq_ev_set_direct(&kb.seq_ev);
  snd_seq_ev_set_source(&kb.seq_ev, kb.seq_port);
  snd_seq_ev_set_subs(&kb.seq_ev);

  while (true) {
    process_input(&kb);
  }

  snd_seq_delete_simple_port(kb.seq, kb.seq_port);
  snd_seq_close(kb.seq);

  ni_mk2_close(&kb.mk2);

  return 0;
}
