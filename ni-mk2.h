#ifndef NI_MK2_H
# define NI_MK2_H

# ifndef _BSD_SOURCE
#  define _BSD_SOURCE // endian.h
# endif

# include <stdint.h>
# include <stdbool.h>
# include <assert.h>

# include <endian.h>

# define NI_MK2_MSG_PADS 0x20
# define NI_MK2_MSG_PADS_SIZE 65
# define NI_MK2_MSG_PAD_PRESSURE_MASK 0x0fff

# define NI_MK2_MSG_BTS 0x01
# define NI_MK2_MSG_BTS_SIZE 25

# define NI_MK2_MSG_PADS_SET_LIGHTS 0x80
# define NI_MK2_MSG_PADS_SET_LIGHTS_SIZE (1 + 16 * 3)

# define NI_MK2_MSG_GRP_TP_SET_LIGHTS 0x81
# define NI_MK2_MSG_GRP_TP_SET_LIGHTS_SIZE (1 + 8 * (3 + 3) + 8)

# define NI_MK2_MSG_BTS_SET_LIGHTS 0x82
# define NI_MK2_MSG_BTS_SET_LIGHTS_SIZE (1 + 8 + 8 + 8 + 7)

# define NI_MK2_MSG_SCREEN_LEFT_DRAW 0xe0
# define NI_MK2_MSG_SCREEN_RIGHT_DRAW 0xe1
# define NI_MK2_MSG_SCREEN_DRAW_SIZE (1 + 8 + 256)


/** the state of buttons: 0 released, 1 pushed */
struct ni_mk2_bts {
  unsigned top0            : 1;
  unsigned top1            : 1;
  unsigned top2            : 1;
  unsigned top3            : 1;
  unsigned top4            : 1;
  unsigned top5            : 1;
  unsigned top6            : 1;
  unsigned top7            : 1;
  unsigned control         : 1;
  unsigned step            : 1;
  unsigned browse          : 1;
  unsigned sampling        : 1;
  unsigned browse_left     : 1;
  unsigned browse_right    : 1;
  unsigned all             : 1;
  unsigned auto_wr         : 1;
  unsigned volume          : 1;
  unsigned swing           : 1;
  unsigned tempo           : 1;
  unsigned master_left     : 1;
  unsigned master_right    : 1;
  unsigned enter           : 1;
  unsigned note_repeat     : 1;
  unsigned master_wheel    : 1;
  unsigned a               : 1;
  unsigned b               : 1;
  unsigned c               : 1;
  unsigned d               : 1;
  unsigned e               : 1;
  unsigned f               : 1;
  unsigned g               : 1;
  unsigned h               : 1;
  unsigned restart         : 1;
  unsigned transport_left  : 1;
  unsigned transport_right : 1;
  unsigned grid            : 1;
  unsigned play            : 1;
  unsigned rec             : 1;
  unsigned erase           : 1;
  unsigned shift           : 1;
  unsigned scene           : 1;
  unsigned pattern         : 1;
  unsigned pad_mode        : 1;
  unsigned navigate        : 1;
  unsigned duplicate       : 1;
  unsigned select          : 1;
  unsigned solo            : 1;
  unsigned mute            : 1;
  uint8_t reserved1;
  uint8_t master_wheel_pos; // from 0 to 15
  uint16_t wheels[8]; // from 0 to 1000
} __attribute__((packed));

// fields varies from 0x00 to 0x80
struct ni_mk2_rgb_light {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} __attribute__((packed));

struct ni_mk2_group_lights {
  struct ni_mk2_rgb_light left;
  struct ni_mk2_rgb_light right;
} __attribute__((packed));

struct ni_mk2_transport_lights {
  uint8_t restart;
  uint8_t left;
  uint8_t right;
  uint8_t grid;
  uint8_t play;
  uint8_t rec;
  uint8_t erase;
  uint8_t shift;
} __attribute__((packed));

struct ni_mk2_buttons_lights {
  uint8_t control;
  uint8_t step;
  uint8_t browse;
  uint8_t sampling;
  uint8_t browse_left;
  uint8_t browse_right;
  uint8_t all;
  uint8_t auto_wr;

  uint8_t top0;
  uint8_t top1;
  uint8_t top2;
  uint8_t top3;
  uint8_t top4;
  uint8_t top5;
  uint8_t top6;
  uint8_t top7;

  uint8_t scene;
  uint8_t pattern;
  uint8_t pad_mode;
  uint8_t navigate;
  uint8_t duplicate;
  uint8_t select;
  uint8_t solo;
  uint8_t mute;

  uint8_t volume;
  uint8_t swing;
  uint8_t tempo;
  uint8_t master_left;
  uint8_t master_right;
  uint8_t enter;
  uint8_t note_repeat;
} __attribute__((packed));

/** a message received from the device */
struct ni_mk2_msg {
  uint8_t type;

  union {
    uint16_t pads[32]; // from 0 to 15 and 16 to 31 are the same
    struct ni_mk2_bts bts;
    uint8_t  reserved[128]; // reserved space
  };
} __attribute__((packed));

/** the device context */
struct ni_mk2 {
  int               fd;

  /* current state */
  uint16_t          pads[32];
  struct ni_mk2_bts bts;

  /* previous state */
  uint16_t          prev_pads[32];
  struct ni_mk2_bts prev_bts;

  struct ni_mk2_rgb_light pads_lights[16];
  struct ni_mk2_group_lights groups_lights[8];
  struct ni_mk2_transport_lights transport_lights;
  struct ni_mk2_buttons_lights buttons_lights;
  uint8_t screen_left[64 * 256 / 8];
  uint8_t screen_right[64 * 256 / 8];
};

/**
 * opens and initialize the mk2 device
 * @return false on error, true on success
 */
bool ni_mk2_open(struct ni_mk2 *ctx, const char *hid_path);

/** closes the device */
void ni_mk2_close(struct ni_mk2 *ctx);

/**
 * reads a message from the device and store it into msg.
 * @return the number of bytes read
 */
ssize_t ni_mk2_read(struct ni_mk2 *ctx,
                    struct ni_mk2_msg *msg);

bool ni_mk2_pads_set_lights(struct ni_mk2 *ctx);
bool ni_mk2_groups_transport_set_lights(struct ni_mk2 *ctx);
bool ni_mk2_buttons_set_lights(struct ni_mk2 *ctx);
bool ni_mk2_screens_draw(struct ni_mk2 *ctx);

#endif /* !NI_MK2_H */
