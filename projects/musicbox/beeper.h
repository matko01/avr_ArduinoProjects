#ifndef __BEEPER_H__
#define __BEEPER_H__

#include <stdint.h>

#define TIMER_BEEPER_ISR_0
// #define TIMER_BEEPER_ISR_1
// #define TIMER_BEEPER_ISR_2

typedef enum _e_timer_pins {
	PD6_OC0A = 0,
	PD5_OC0B,
	PB1_OC1A,
	PB2_OC1B,
	PB3_OC2A,
	PD3_OC2B,

	TIMER_PIN_LAST
} e_timer_pins;

void beeper_init(e_timer_pins a_pin);
void beeper_beep(e_timer_pins a_pin, uint32_t freq, uint32_t duration);
void beeper_off(e_timer_pins a_pin);
void beeper_block(e_timer_pins a_pin);

#endif /* __BEEPER_H__ */
