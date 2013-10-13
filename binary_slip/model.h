#ifndef MODEL_H_F3EHXR7D
#define MODEL_H_F3EHXR7D

#include <stdint.h>

typedef enum _e_status {
	ACK = 0x10,
	NACK
} e_status;


struct timer_data {
	uint16_t crc16;
	uint8_t prescaler;
	uint8_t ocr;
	uint8_t st_max;
} __attribute__((packed));


struct response {
	uint16_t crc16;
	uint8_t status;
} __attribute__((packed));


#endif /* end of include guard: MODEL_H_F3EHXR7D */

