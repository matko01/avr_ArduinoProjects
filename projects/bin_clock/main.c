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
