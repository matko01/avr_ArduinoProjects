#ifndef TDELAY_COMMON_H_NXGPZAZA
#define TDELAY_COMMON_H_NXGPZAZA

#include "common.h"

#define TDELAY_IMPLEMENT_T0_INT 1
#define TDELAY_IMPLEMENT_T1_INT 1
#define TDELAY_IMPLEMENT_T2_INT 1


typedef enum _e_timer {
	E_TIMER0,
	E_TIMER1,
	E_TIMER2,

	E_TIMER_LAST
} e_timer;

void _tdc_set_duration(e_timer a_tim, uint32_t a_dur);
void _tdc_set_cmp_pin(e_timer a_tim, uint8_t a_pin);

void _tdc_setup_ms(e_timer a_timer, uint32_t a_delay);
void _tdc_enable_interrupt(e_timer a_timer);
void _tdc_block(e_timer a_timer);

#endif /* end of include guard: TDELAY_COMMON_H_NXGPZAZA */

