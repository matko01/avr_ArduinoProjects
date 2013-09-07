#include "aos.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "timer_common.h"

// tick every 1 ms
#define SCHED_TICK_FREQUENCY 100

void task1(void *a_data UNUSED) {
	uint16_t x = 0;
	while (1) {
		if (!(x%20000)) PORTB ^= 0xff;
		x++;		
	}
}

static uint8_t y = 0;

void task2(void *a_data UNUSED) {
	uint8_t *yy = (uint8_t *)a_data;

	while (1) {
		if (0 == (*yy))
			PORTD ^= 0xff;

		*yy = *yy + 1;
	}
}


int main(void) {

	DDRB = 0xff;
	DDRD = 0xff;

	aos_init(SCHED_TICK_FREQUENCY);
	struct task_cb *t1 UNUSED = 
		aos_task_create(task1, NULL, AOS_TASK_PRIORITY_IDLE, 32);

	struct task_cb *t2 UNUSED =
		aos_task_create(task2, (void *)&y, AOS_TASK_PRIORITY_IDLE, 32);

	aos_run();
	return 0;
}
