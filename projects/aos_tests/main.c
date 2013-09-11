#include "aos.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "timer_common.h"

// tick every 10 ms
#define SCHED_TICK_FREQUENCY 100

void task1(void *a_data UNUSED) {
	struct aos_timer tm;

	while (1) {
		PORTB++;
		aos_timer_wait(&tm, 50);
	}
}

void task2(void *a_data UNUSED) {
	while (1) {
		_delay_ms(250);
		PORTD ^= 0x01;
	}
}

struct task_cb *t3;

void task3(void *a_data UNUSED) {
	uint32_t a = 0;
	while (1) {
		if (!(a%65536)) {
			PORTC ^= 0x01;
		}
		a++;
	}
}

void hook_task(void) {
	PORTB ^= 0xff;
	_delay_ms(250);
}

int main(void) {

	DDRB = 0xff;
	DDRD = 0xff;
	DDRC = 0xff;

	aos_init(SCHED_TICK_FREQUENCY);

	/* aos_common_hook_install(AOS_HOOK_IDLE, hook_task); */

	struct task_cb *t1 UNUSED = 
		aos_task_create(task1, NULL, AOS_TASK_PRIORITY_NORMAL, 64);

	/* struct task_cb *t2 UNUSED = */
	/* 	aos_task_create(task2, NULL, AOS_TASK_PRIORITY_NORMAL, 32); */

	/* t3 = aos_task_create(task3, NULL, AOS_TASK_PRIORITY_HIGH, 32); */

	aos_run();
	return 0;
}
