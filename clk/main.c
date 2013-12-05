#include "pca.h"
#include "main.h"
#include "sys_ctx.h"

#include <util/delay.h>
#include "sys_conf.h"

/**
 * @brief global system context
 */
volatile struct sys_ctx g_sys_ctx;


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


void main(void) {

	struct dev_hd44780_ctx lcd_ctx;

	// transfer buffer & time_t
	/* uint8_t xbuff[XBUFF_SIZE] = {0x00}; */

	// setup the display	
	lcd_setup(&lcd_ctx);

	// setup the rtc
	/* rtc_setup() */

	char str[40];
	/* uint8_t x = 0x00; */

	/* power_timer2_enable(); */
	/* TCCR2A = 0x83; */
	/* TCCR2B = 0x03; */
	DDRB |= _BV(3);
	PORTB |= _BV(3);
	
	hd44780_cmd(&lcd_ctx, HD44780_CMD_DISPLAY_CONTROL(1, 1, 1));

	for (;;) {
		tmp_update_measurements(&g_sys_ctx.temp_ctx,
				(struct soft_ow *)&g_sys_ctx.sow_ctx);

		// SLA + W
		/* xbuff[0] = DS1307_SECONDS_ADDR; */
		/* twi_mtx(TWI_RTC_ADDR, xbuff, 0x01, 0x00); */

		/* x+=8; */
		/* OCR2A = x; */

		/* // wait for I2C bus */
		/* while (twi_ctx->status & E_TWI_BIT_BUSY); */

		/* // get data */
		/* twi_mrx(TWI_RTC_ADDR, xbuff, sizeof(ds1307_time_t), E_TWI_BIT_SEND_STOP); */
		/* while (twi_ctx->status & E_TWI_BIT_BUSY); */

		/* sprintf(str, "%02x:%02x:%02x",  */
		/* 	tm->mode_ampm_hour & 0x3f, */
		/* 	tm->min, */
		/* 	tm->ch_sec & 0x7f); */

		/* hd44780_goto(&lcd_ctx, 0x00); */
		/* hd44780_puts(&lcd_ctx, str); */

		/* hd44780_goto(&lcd_ctx, 0x40); */
		/* sprintf(str, "%04u/%04u/%04u", temp_min, *temp, temp_max); */
		/* hd44780_puts(&lcd_ctx, str); */

		hd44780_goto(&lcd_ctx, 0x00);
		hd44780_puts(&lcd_ctx, "test");
		hd44780_goto(&lcd_ctx, 0x02);
		_delay_ms(800);
	}
} // main
