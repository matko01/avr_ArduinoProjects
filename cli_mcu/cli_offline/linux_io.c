#include "linux_io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>


void cm_off(void) {
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	t.c_lflag &= ~ICANON; 
	t.c_lflag &= ~ECHO;

	// Apply the new settings
	tcsetattr(STDIN_FILENO, TCSANOW, &t); 
}


void cm_on(void) {
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	t.c_lflag |= ICANON; 
	t.c_lflag |= ECHO;

	// Apply the new settings
	tcsetattr(STDIN_FILENO, TCSANOW, &t); 
}


unsigned char linux_getc(unsigned char *a_data) {
	*a_data = getchar();
	return 1;
}


unsigned char linux_putc(unsigned char *data, unsigned char a_len) {
	while (a_len--) {
		fputc(*data++, stdout);
	}
}

