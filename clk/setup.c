#include "setup.h"
#include "main.h"


void lcd_setup(struct dev_hd44780_ctx *a_lcd_ctx) {
	// lcd setup
	a_lcd_ctx->rs.port = &LCD_RS_PORT;
	a_lcd_ctx->rs.pin = LCD_RS_PIN;
	a_lcd_ctx->e.port = &LCD_E_PORT;
	a_lcd_ctx->e.pin = LCD_E_PIN;

	// setup data lines
	for (uint8_t x = 0; x<4; x++) {
		a_lcd_ctx->data[x].port = &LCD_DB_PORT;
		a_lcd_ctx->data[x].pin = LCD_DB_PIN_FIRST + x;
	}

	// display specifics
	a_lcd_ctx->lines = 2;
	a_lcd_ctx->font = HD44780_FONT_5X8;

	// initialize the device
	hd44780_init(a_lcd_ctx);
}


void thermo_setup(struct soft_ow *a_sow_ctx) {
	// soft one wire bus setup
	a_sow_ctx->conf = 
		SOFT_OW_CONF_MAKE(OW_POWER_PARASITE, OW_SINGLEDROP);
	a_sow_ctx->bus.port = &OW_THERMO_OUTP;
	a_sow_ctx->bus.pin = OW_THERMO_PIN;

	// initialize one-wire
	sow_init(a_sow_ctx);
}



void rtc_setup(volatile struct twi_ctx *a_ctx) {

	// I2C CTX
	/* twi_ctx = twi_init(E_TWI_SCL_100K); */

}

