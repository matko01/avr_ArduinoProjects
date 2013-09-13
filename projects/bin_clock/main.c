#include "main.h"

// 1 ms tick
#define AOS_TICK_FREQUENCY 1000


void task_timer(void *a_data) {

	while (1) {
	}
}


void task_serial_sync(void *a_data) {

	serial_init(E_BAUD_9600);	
	serial_install_interrupts(SERIAL_RX_INTERRUPT);
	serial_flush();

	while (1) {
	}
}



/* void bclk_inc_time(void *a_data) { */
/* 	struct tm *data = (struct tm *)a_data; */
/*  */
/* 	// protect this section to prevent displaying garbage */
/* 	// even for a very short fragment of time */
/* 	g_conversion = 0x01; */
/*  */
/* 	// increment by a second */
/* 	_bcd_correct(++data->secs); */
/*  */
/* 	if (data->secs >= 0x60) { */
/* 		data->secs = 0x00; */
/* 		_bcd_correct(++data->mins); */
/* 	} */
/*  */
/* 	if (data->mins >= 0x60) { */
/* 		data->mins = 0x00; */
/* 		_bcd_correct(++data->hours); */
/* 	} */
/*  */
/* 	if (data->hours >= 0x24) { */
/* 		data->hours = 0x00; */
/* 	} */
/*  */
/* 	// unprotect */
/* 	g_conversion = 0x00; */
/* } */


/**
 * @brief entry point
 */
int main(void) {

	// system context
	struct ctx sctx;

	// initialize the OS
	aos_init(AOS_TICK_FREQUENCY);

	// create timer task 
	sctx.tasks[TASK_TIMER] = 
		aos_task_create(task_timer, (void*)&sctx, AOS_TASK_PRIORITY_NORMAL, 64);

	sctx.tasks[TASK_SERIAL_SYNC] = 
		aos_task_create(task_serial_sync, (void*)&sctx, AOS_TASK_PRIORITY_NORMAL, 64);

	aos_run();	
	return 0;
}
