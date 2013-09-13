#include "main.h"

// 1 ms tick
#define AOS_TICK_FREQUENCY 1000


void task_timer(void *a_data) {
	struct aos_timer tm;
	struct ctx *pctx = (struct ctx *)a_data;

	sched_systime_t tick1 = aos_common_systime_get(),
					tick2 = 0,
					delta = 0;

	// count ticks
	while (1) {
		tick2 = aos_common_systime_get();
		delta = tick2 - tick1;

		if (delta > 1000) {
			bc_inc_time(&pctx->g_time);
			tick1 = aos_common_systime_get() + (1000 - delta);
		}
		
		aos_timer_wait(&tm, 300);
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
