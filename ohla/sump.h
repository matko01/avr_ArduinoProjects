#ifndef SUMP_H_GT2BR81O
#define SUMP_H_GT2BR81O

#define SUMP_RESET 0x00
#define SUMP_RUN 0x01
#define SUMP_XON 0x11
#define SUMP_XOFF 0x13

// 5 byte commands, first byte is unique for every command
#define SUMP_SET_TRIGGER_MASK 0xc0
#define SUMP_SET_TRIGGER_VALUE 0xc1
#define SUMP_SET_TRIGGER_CONF 0xc2
#define SUMP_SET_DIVIDER 0x80
#define SUMP_SET_READY_DELAY_COUNT 0x81
#define SUMP_SET_FLAGS 0x82

#endif /* end of include guard: SUMP_H_GT2BR81O */

