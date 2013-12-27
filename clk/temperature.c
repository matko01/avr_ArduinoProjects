#include "temperature.h"
#include "pca.h"
#include "rtc.h"

#include <util/atomic.h>


uint8_t tmp_setup(volatile struct temp_ctx *a_ctx) {

	// update min/max values
	tmp_reset(&a_ctx->msr);

	// perform HW init 12 bit resolution
	ds18b20_write_rom((struct soft_ow *)a_ctx->sow_ctx, NULL, 0x00, 0x00, 0x03, 0x00);

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		a_ctx->msr.state = TEMP_MEASUREMENT_IDLE;
		a_ctx->msr.tv = TEMPERATURE_MEASUREMENT_INTERVAL;
	}

	return RET_OK;
}


void tmp_reset(volatile struct temp_msr_ctx *msr) {
	msr->temp_min = 0xffff;
	msr->temp_max = 0x00;
	msr->temp = 0x00;
}


void tmp_update_tv(volatile struct temp_msr_ctx *a_ctx) {
	if ((TEMP_MEASUREMENT_TRIGGERED == a_ctx->state) && a_ctx->tv) {
		a_ctx->tv--;
	}
	else if ((TEMP_MEASUREMENT_TRIGGERED == a_ctx->state) && !a_ctx->tv) {
		a_ctx->state = TEMP_MEASUREMENT_TIMEOUT;
	}
}


void tmp_update_measurements(volatile struct twi_ctx *a_twi, volatile struct temp_ctx *a_ctx) {
	uint16_t tmp = a_ctx->msr.temp;

	if ((TEMP_MEASUREMENT_TIMEOUT != a_ctx->msr.state)) {
		return;
	}

	// read tmp
	ds18b20_read_scratchpad((struct soft_ow *)a_ctx->sow_ctx, 
			NULL, 
			(volatile uint8_t *)&a_ctx->msr.temp, 2);

	// ignore default reset value if it occurs (85 degree workaround)
	if (0x0550==a_ctx->msr.temp) {
		a_ctx->msr.temp = tmp;
	}

	// update min/max values
	a_ctx->msr.temp_min = a_ctx->msr.temp < a_ctx->msr.temp_min ? 
		a_ctx->msr.temp : a_ctx->msr.temp_min;
	a_ctx->msr.temp_max = a_ctx->msr.temp > a_ctx->msr.temp_max ?
		a_ctx->msr.temp : a_ctx->msr.temp_max;

	// save temperatures in RTC
	rtc_store_temperatures(a_twi,(struct temp_msr_ctx *)&a_ctx->msr);

	a_ctx->msr.state = TEMP_MEASUREMENT_IDLE;
}


void tmp_trigger_measurement(volatile struct temp_ctx *a_ctx) {
	if ((TEMP_MEASUREMENT_IDLE != a_ctx->msr.state)) {
		return;
	}

	// SKIP ROM, rom code is null
	ds18b20_start_conversion(a_ctx->sow_ctx, NULL);

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		a_ctx->msr.state = TEMP_MEASUREMENT_TRIGGERED;
		a_ctx->msr.tv = TEMPERATURE_MEASUREMENT_INTERVAL;
	}
}


