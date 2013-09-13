#include "aos.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "timer_common.h"

// tick every 10 ms
#define SCHED_TICK_FREQUENCY 100

static struct aos_sem semaphore;

void task1(void *a_data UNUSED) {
	struct aos_timer tm UNUSED;
	DDRB = 0xff;
	aos_sem_init(&semaphore);

	while (1) {
		PORTB++;
		aos_sem_p(&semaphore);
	}
}

void task2(void *a_data UNUSED) {
	struct aos_timer tm;
	DDRD = 0xff;
	while (1) {
		PORTD++;
		aos_sem_v(&semaphore);
		aos_timer_wait(&tm, 50);
	}
}

struct task_cb *t3;

void task3(void *a_data UNUSED) {
	uint32_t a = 0;
	DDRC = 0xff;
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


	aos_init(SCHED_TICK_FREQUENCY);

	/* aos_common_hook_install(AOS_HOOK_IDLE, hook_task); */

	struct task_cb *t1 UNUSED = 
		aos_task_create(task1, NULL, AOS_TASK_PRIORITY_LOW, 48);

	struct task_cb *t2 UNUSED =
		aos_task_create(task2, NULL, AOS_TASK_PRIORITY_NORMAL, 48);

	t3 = aos_task_create(task3, NULL, AOS_TASK_PRIORITY_NORMAL, 48);

	aos_run();
	return 0;
}
