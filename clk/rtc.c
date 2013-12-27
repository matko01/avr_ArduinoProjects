#include "rtc.h"
#include "pca.h"
#include "sys_config.h"


void rtc_setup(volatile struct twi_ctx *a_ctx, struct temp_msr_ctx *msr) {
	uint8_t data[10] = {DS1307_CONTROL_ADDR};
	uint8_t len = 0x00;

	twi_mtx(TWI_RTC_ADDR, data, 0x01, 0x00);
	while (a_ctx->status & E_TWI_BIT_BUSY);
	
	twi_mrx(TWI_RTC_ADDR, &data[1], sizeof(uint8_t)*2, E_TWI_BIT_SEND_STOP);
	while (a_ctx->status & E_TWI_BIT_BUSY);

	// out = 0
	// sqw = 1
	// rs = 0
	if (0x10!=data[1] || RTC_MAGIC_ID!=data[2]) {
		// the clock is not properly initialized
		len = sizeof(data);
		common_zero_mem(data,len);

		// TODO temporary changed, should be 0x80 - HALTED
		data[1] = 0x80; // clock halted

		data[1 + DS1307_DOW_ADDR] = 1;
		data[1 + DS1307_DOM_ADDR] = 1;
		data[1 + DS1307_MONTHS_ADDR] = 1;

		data[DS1307_CONTROL_ADDR + 1] = 0x10;
		data[RTC_MAGIC_ID_ADDR + 1] = RTC_MAGIC_ID;

		twi_mtx(TWI_RTC_ADDR, data, sizeof(data), E_TWI_BIT_SEND_STOP);
		while (a_ctx->status & E_TWI_BIT_BUSY);

		rtc_store_temperatures(a_ctx, msr);
	}
	else {
		rtc_restore_temperatures(a_ctx, msr);
	}
}


void rtc_store_temperatures(volatile struct twi_ctx *a_ctx, struct temp_msr_ctx *msr_ctx) {
	uint8_t data[5] = {MAX_TEMP_ADDR};
	*((uint16_t *)&data[1]) = msr_ctx->temp_max;
	*((uint16_t *)&data[3]) = msr_ctx->temp_min;

	twi_mtx(TWI_RTC_ADDR, data, sizeof(data), E_TWI_BIT_SEND_STOP);
	while (a_ctx->status & E_TWI_BIT_BUSY);
}


void rtc_restore_temperatures(volatile struct twi_ctx *a_ctx, struct temp_msr_ctx *msr_ctx) {
	uint8_t data[5] = {MAX_TEMP_ADDR};

	twi_mtx(TWI_RTC_ADDR, data, 0x01, 0x00);
	while (a_ctx->status & E_TWI_BIT_BUSY);
	twi_mrx(TWI_RTC_ADDR, &data[1], sizeof(uint8_t)*4, E_TWI_BIT_SEND_STOP);
	while (a_ctx->status & E_TWI_BIT_BUSY);

	msr_ctx->temp_max = *((uint16_t *)&data[1]);
	msr_ctx->temp_min = *((uint16_t *)&data[3]);
}
