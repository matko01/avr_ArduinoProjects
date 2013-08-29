#include "serial.h"
#include "slip.h"
#include "sched.h"

static unsigned char g_conversion = 0x00;
static struct tm g_time  = { 0x00 };


static void _bcd_correct(unsigned char *a_bcd) {
	unsigned char x = 0;
	for (; x<2; x++) {
		if ((*a_bcd & (0x0f << (4*x))) > (0x09 << (4*x))) {
				*a_bcd += (0x06 << (4*x));
		}
	}
}


void bclk_inc_time(void *a_data) {
	struct tm *data = (struct tm *)a_data;

	// protect this section to prevent displaying garbage
	// even for a very short fragment of time
	g_conversion = 0x01;

	// increment by a second
	_bcd_correct(++data->secs);

	if (data->secs >= 0x60) {
		data->secs = 0x00;
		_bcd_correct(++data->mins);
	}

	if (data->mins >= 0x60) {
		data->mins = 0x00;
		_bcd_correct(++data->hours);
	}

	if (data->hours >= 0x24) {
		data->hours = 0x00;
	}

	// unprotect
	g_conversion = 0x00;
}






int main(void)
{
	serial_init(E_BAUD_9600);	
	serial_install_interrupts(SERIAL_RX_INTERRUPT);
	serial_flush();

	// 256 hertz - ticks per second (system clock)
	sched_init(256);

	// increment clock
	sched_task_add(blck_inc_time, (void *)&g_time, 256);

	// run scheduled tasks
	while(1) sched_run();
	return 0;
}
