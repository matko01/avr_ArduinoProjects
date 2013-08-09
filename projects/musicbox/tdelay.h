#ifndef TDELAY_H_IPOQKAD0
#define TDELAY_H_IPOQKAD0

#include <stdint.h>

// #define TIMER_DELAY_ISR_0
// #define TIMER_DELAY_ISR_1
// #define TIMER_DELAY_ISR_2

typedef enum _e_timer {
	E_TIMER0,
	E_TIMER1,
	E_TIMER2,

	E_TIMER_LAST
} e_timer;

void tdelay_init(e_timer a_timer);
void tdelay_ms(e_timer, uint32_t a_delay);

#endif /* end of include guard: TDELAY_H_IPOQKAD0 */

