#include "aos.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "timer_common.h"

// tick every 1 ms
#define SCHED_TICK_FREQUENCY 35

void task1(void *a_data UNUSED) {
	while (1) {
		PORTB = 0x00;
		_delay_ms(500);

		PORTB = 0xff;
		_delay_ms(500);
	}
}

void task2(void *a_data UNUSED) {
	while (1) {
		PORTD = 0x00;
		_delay_ms(250);

		PORTD = 0xff;
		_delay_ms(250);
	}
}


int main(void) 
{
	DDRB = 0xff;
	DDRD = 0xff;

	aos_init(SCHED_TICK_FREQUENCY);
	/* struct task_cb *t1 UNUSED =  */
	/* 	aos_task_create(task1, NULL, AOS_TASK_PRIORITY_IDLE, 128); */

	aos_run();
	return 0;
}
