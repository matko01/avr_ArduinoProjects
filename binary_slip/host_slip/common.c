#include "common.h"

#include <unistd.h>
#include <stdio.h>

extern int g_fd;

unsigned int slip_sendc(unsigned char a_char) {
	/* printf("Sending char: %02x\n", a_char); */
	return write(g_fd, (void *)&a_char, 1);
}


unsigned char slip_getc(unsigned char *a_data) {

	 unsigned char x = read(g_fd, a_data, 1);
	 /* printf("Recv: %02x\n", *a_data); */
	 return x;
}

