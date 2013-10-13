#include "../model.h"
#include "main.h"
#include "crc.h"

#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>


#define SERIAL_PORT "/dev/ttyACM0"

// serial port file descriptor
int g_fd = 0x00;


#define BAUD_2400 B2400
#define BAUD_4800 B4800
#define BAUD_9600 B9600
#define BAUD_38400 B38400
#define BAUD_57600 B57600
#define BAUD_115200 B115200


static int tty_attrib_conf(int fd, int speed, int parity) {
	struct termios tty;
	memset (&tty, 0, sizeof(tty));
	
	if (tcgetattr (fd, &tty) != 0) {
		fprintf(stderr, "Error %d from tcgetattr\n", errno);
		return -1;
	}

	// set I/O tty speed
	cfsetospeed (&tty, speed);
	cfsetispeed (&tty, speed);

	// 8-bit chars;
	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;

	// disable IGNBRK for mismatched speed tests; otherwise receive break as \000 chars
	// ignore break signal
	tty.c_iflag &= ~IGNBRK;         

	// no signaling chars, no echo,
	tty.c_lflag = 0;

	// no canonical processing
	// no remapping, no delays
	tty.c_oflag = 0; 

	// read doesn't block
	tty.c_cc[VMIN]  = 0; 

	// 0.5 seconds read timeout
	tty.c_cc[VTIME] = 5; 

	// shut off xon/xoff ctrl
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); 

	// ignore modem controls,
	tty.c_cflag |= (CLOCAL | CREAD);

	// enable reading / shut off parity
	tty.c_cflag &= ~(PARENB | PARODD);
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr (fd, TCSANOW, &tty) != 0) {
		fprintf(stderr, "Error %d from tcsetattr\n", errno);
		return -1;
	}
	return 0;
}


/**
 * @brief whether a port should block (read() will block until the requested amount of bytes is received) or not
 *
 * @param fd port descriptor
 * @param should_block 1 - should block, 0 shouldn't
 */
static void tty_block_conf(int fd, int should_block) {
	struct termios tty;
	memset (&tty, 0, sizeof tty);

	if (tcgetattr (fd, &tty) != 0) {
		fprintf(stderr, "Error %d from tcgetattr\n", errno);
		return;
	}

	tty.c_cc[VMIN]  = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

	if (tcsetattr (fd, TCSANOW, &tty) != 0) {
		fprintf(stderr, "Error %d setting term attributes\n", errno);
		return;
	}
}


int main(int argc, char const *argv[]) {
	
	g_fd = 0x00;
	struct response *resp;
	struct timer_data *data;
	uint8_t buff[8] = {0x00};
	uint16_t crc = 0x00;

	if (argc <= 3) {
		fprintf(stderr, "host_slip <prescaler> <ocr> <st_max>\n");
		return -1;
	}

	if (0 > (g_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_SYNC))) {
		fprintf(stderr, "Unable to open device [%s]\n", SERIAL_PORT);
		return 0;
	}

	tty_attrib_conf(g_fd, BAUD_9600, 0);
	tty_block_conf(g_fd, 1);

	data = (struct timer_data *)buff;

	data->crc16 = 0x0000;
	data->prescaler = atoi(argv[1]);
	data->ocr = atoi(argv[2]);
	data->st_max = atoi(argv[3]);

	// insert the CRC
	crc = crc16(0x00, buff, sizeof(struct timer_data));
	data->crc16 = crc;

	// wait for the board to reset itself
	sleep(4);

	printf("Sending: %d/%d/%d, CRC: 0x%04x\n", 
			data->prescaler,
			data->ocr,
			data->st_max,
			data->crc16);

	slip_send(buff, sizeof(struct timer_data));
	slip_recv(buff, sizeof(buff));

	resp = (struct response *)buff;

	// zero the CRC to perform verification
	buff[6] = buff[0];
	buff[7] = buff[1];
	buff[0] = 0x00;
	buff[1] = 0x00;

	// we've got the calculated CRC now
	crc = crc16(0x00, buff, sizeof(struct response));

	// restore the CRC
	buff[0] = buff[6];
	buff[1] = buff[7];

	printf("CRC/CRC_CALCD/STATUS: 0x%04x/0x%04x/%s\n", 
			resp->crc16, 
			crc,
			(resp->status == 0x10 ? "ACK" : "NACK"));

	close(g_fd);

	return 0;
}
