#ifndef SUMP_H_GT2BR81O
#define SUMP_H_GT2BR81O

#include <stdint.h>
#include "defines.h"

// 1 byte (short) commands 
#define SUMP_RESET 0x00
#define SUMP_RUN 0x01
#define SUMP_ID 0x02
#define SUMP_GET_METADATA 0x04
#define SUMP_XON 0x11
#define SUMP_XOFF 0x13

// 5 byte (long) commands, first byte is unique for every command
#define SUMP_SET_TRIGGER_MASK 0xc0
#define SUMP_SET_TRIGGER_VALUE 0xc1
#define SUMP_SET_TRIGGER_CONF 0xc2
#define SUMP_SET_DIVIDER 0x80
#define SUMP_SET_READY_DELAY_COUNT 0x81
#define SUMP_SET_FLAGS 0x82


// meta data information
#define SUMP_META_STR_NULL 0x00
#define SUMP_META_STR_DEVNAME 0x01
#define SUMP_META_STR_FPGA_VER 0x02
#define SUMP_META_STR_ANC_VER 0x03

#define SUMP_META_INT_PROBES 0x20
#define SUMP_META_INT_SSAMPLES 0x21
#define SUMP_META_INT_DSAMPLES 0x22
#define SUMP_META_INT_SRATE 0x23
#define SUMP_META_INT_PROTO_VER 0x24

#define SUMP_META_INT_PROBES8 0x40
#define SUMP_META_INT_PROTO_VER8 0x41


void sump_provide_metadata(uint32_t probes,
		uint32_t ssamples,
		uint32_t dsamples,
		uint32_t srate);

#endif /* end of include guard: SUMP_H_GT2BR81O */

