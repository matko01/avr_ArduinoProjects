#include "aos.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "timer_common.h"

// tick every 1 ms
#define SCHED_TICK_FREQUENCY 100

void task1(void *a_data UNUSED) {
	while (1) {
		_delay_ms(500);
		PORTB++;
	}
}

void task2(void *a_data UNUSED) {
	while (1) {
		_delay_ms(250);
		PORTD ^= 0x01;
	}
}

void task3(void *a_data UNUSED) {
	uint32_t a = 0;
	while (1) {
		if (!(a%8192)) {
			PORTC ^= 0x01;
		}
		a++;
	}
}

int main(void) {

	DDRB = 0xff;
	DDRD = 0xff;
	DDRC = 0xff;

	aos_init(SCHED_TICK_FREQUENCY);

	/* aos_common_hook_install(AOS_HOOK_IDLE, task1); */

	struct task_cb *t1 UNUSED = 
		aos_task_create(task1, NULL, AOS_TASK_PRIORITY_IDLE, 32);

	struct task_cb *t2 UNUSED =
		aos_task_create(task2, NULL, AOS_TASK_PRIORITY_IDLE, 32);

	struct task_cb *t3 UNUSED =
		aos_task_create(task3, NULL, AOS_TASK_PRIORITY_IDLE, 32);

	aos_run();
	return 0;
}
