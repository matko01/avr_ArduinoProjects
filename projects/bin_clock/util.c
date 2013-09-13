#include "util.h"

void bc_bcd_correct(unsigned char *a_bcd) {
	unsigned char x = 0;
	for (; x<2; x++) {
		if ((*a_bcd & (0x0f << (4*x))) > (0x09 << (4*x))) {
				*a_bcd += (0x06 << (4*x));
		}
	}
}

