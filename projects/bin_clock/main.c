#include "main.h"

#include <string.h>
#include <avr/io.h>
#include <stdio.h>

// 10 ms tick
#define AOS_TICK_FREQUENCY 100

extern volatile struct aos_sys _g_sys;

// counts ticks in order to track time
void task_timer(void *a_data) {
	struct aos_timer tm;
	struct ctx *pctx = (struct ctx *)a_data;

	// capture ticks
	sched_systime_t tick1 = 0, tick2;
	int16_t delta;

	// count ticks
	while (1) {
		tick2 = aos_common_systime_get();
		delta = tick2 - tick1;

		// have a second passed by ?
		if (delta > (AOS_TICK_FREQUENCY << 1)) {
			bc_inc_time(&pctx->g_time);
			tick1 = aos_common_systime_get() + ((AOS_TICK_FREQUENCY << 1) - delta);
		}

		// sleep for a while
		// no need to run this with full speed
		aos_timer_wait(&tm, 30);
	}
}


void task_multiplexer(void *a_data) {
	struct ctx *pctx = (struct ctx *)a_data;
	struct aos_timer tm;
	DDRB = 0xff;
	DDRD = 0xff;

	while (1) {
		PORTB = pctx->g_time.secs & 0x0f;
		PORTD = (pctx->g_time.secs & 0xf0) >> 4;
		PORTD |= (pctx->g_time.mins & 0x0f) << 4;
		

		// segment selector
		pctx->row = (pctx->row == 0x20) ? 0x01 : (pctx->row << 1);		
		aos_timer_wait(&tm, 8);
	}

}



void task_serial_sync(void *a_data) {


	while (1) {
	}
}




/**
 * @brief entry point
 */
int main(void) {

	// system context
	struct ctx sctx;

	/* serial_init(E_BAUD_9600);	 */
	/* serial_install_interrupts(SERIAL_RX_INTERRUPT);	 */
	/* serial_flush(); */
	/* serial_install_stdio(); */

	// clear the context
	memset(&sctx, 0x00, sizeof (struct ctx));

	// initialize the OS
	aos_init(AOS_TICK_FREQUENCY);

	// create timer task 
	sctx.tasks[TASK_TIMER] = 
		aos_task_create(task_timer, (void*)&sctx, AOS_TASK_PRIORITY_NORMAL, 64);

	sctx.tasks[TASK_MULTIPLEXER] = 
		aos_task_create(task_multiplexer, (void*)&sctx, AOS_TASK_PRIORITY_NORMAL, 64);

	/* sctx.tasks[TASK_SERIAL_SYNC] =  */
	/* 	aos_task_create(task_serial_sync, (void*)&sctx, AOS_TASK_PRIORITY_NORMAL, 64); */

	aos_run();	
	return 0;
}
