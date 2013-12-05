#include "temperature.h"
#include "main.h"
#include <avr/interrupt.h>
#include "sys_ctx.h"

/**
 * @brief timer 0 overflow interrupt handler
 *
 * @param TIMER0_OVF_vect
 */
ISR(TIMER0_OVF_vect) {
	// handle temperature measurements
	tmp_update_tv(&g_sys_ctx.temp_ctx);
}


/**
 * @brief external interrupt - 1Hz RTC output line triggers this interrupt
 *
 * @param INT0_vect
 */
ISR(INT0_vect) {
	// trigger temperature measurement
	tmp_trigger_measurement(&g_sys_ctx.temp_ctx,
			(struct soft_ow *)&g_sys_ctx.sow_ctx);
}
