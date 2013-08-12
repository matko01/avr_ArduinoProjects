#ifndef BEEPER_H_URHFWD8M
#define BEEPER_H_URHFWD8M

#include "tdelay.h"

#define beeper_block(__timer) _tdc_block(__timer)


void beeper_init(e_timer a_timer);
void beeper_beep(e_timer a_timer, uint32_t freq, uint32_t duration);
void beeper_off(e_timer a_timer);

#endif /* end of include guard: BEEPER_H_URHFWD8M */

