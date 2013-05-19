#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>

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
};

void process_pads(struct alsa_kb *kb)
{
  static const uint8_t pad_note_map[16] = {
    60, 61, 62, 63,
    56, 57, 58, 59,
    52, 53, 54, 55,
    48, 49, 50, 51
  };
  int vel, vel_prev;

  for (int i = 0; i < 16; ++i) {
    vel      = kb->mk2.pads[i] >> 5;
    vel_prev = kb->mk2.prev_pads[i] >> 5;

    assert(vel < 128);
    assert(vel_prev < 128);

    if (vel == vel_prev)
      continue;

    if (!vel_prev)
      snd_seq_ev_set_noteon(&kb->seq_ev, 1, pad_note_map[i], vel);
    else if (vel)
      snd_seq_ev_set_keypress(&kb->seq_ev, 1, pad_note_map[i], vel);
    else
      snd_seq_ev_set_noteoff(&kb->seq_ev, 1, pad_note_map[i], vel);
    snd_seq_event_output(kb->seq, &kb->seq_ev);
    snd_seq_drain_output(kb->seq);
  }
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
