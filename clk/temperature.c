#include "temperature.h"
#include "pca.h"


void tmp_update_tv(volatile struct temp_msr_ctx *a_ctx) {
	if ((TEMP_MEASUREMENT_TRIGGERED == a_ctx->state) && a_ctx->tv) {
		if (!--a_ctx->tv)
			a_ctx->state = TEMP_MEASUREMENT_TIMEOUT;
	}
}


void tmp_update_measurements(volatile struct temp_msr_ctx *a_ctx,
		struct soft_ow *a_bus) {
	if ((TEMP_MEASUREMENT_TIMEOUT == a_ctx->state)) {

		// read tmp
		ds18b20_read_scratchpad(a_bus, (uint8_t *)a_ctx->temp, 2);

		// update min/max values
		a_ctx->temp_min = a_ctx->temp < a_ctx->temp_min ? 
			a_ctx->temp : a_ctx->temp_min;
		a_ctx->temp_max = a_ctx->temp > a_ctx->temp_max ?
		   	a_ctx->temp : a_ctx->temp_max;
		
		a_ctx->state = TEMP_MEASUREMENT_IDLE;
	}
}


void tmp_trigger_measurement(volatile struct temp_msr_ctx *a_ctx,
		struct soft_ow *a_bus) {
	if ((TEMP_MEASUREMENT_IDLE == a_ctx->state)) {

		// SKIP ROM, rom code is null
		ds18b20_start_conversion(a_bus, NULL);
		
		a_ctx->state = TEMP_MEASUREMENT_TRIGGERED;
		a_ctx->tv = TEMPERATURE_MEASUREMENT_INTERVAL;
	}
}
