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

struct ni_mk2_msg {
  uint8_t type;

  union {
    uint16_t pads[32]; // from 0 to 15 and 16 to 31 are the same
    struct ni_mk2_bts bts;
    uint8_t  reserved[128]; // reserved space
  };
} __attribute__((packed));

struct ni_mk2 {
  int fd;
};

/** opens and initialize the mk2 device */
bool ni_mk2_open(struct ni_mk2 *ctx, const char *hid_path);

/** closes the device */
void ni_mk2_close(struct ni_mk2 *ctx);

/**
 * reads a message from the device and store it into msg.
 * @return the number of bytes read
 */
ssize_t ni_mk2_read(struct ni_mk2 *ctx, struct ni_mk2_msg *msg);

#endif /* !NI_MK2_H */
