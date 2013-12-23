#include "pca.h"
#include "main.h"
#include "int_ctx.h"


void main(void) {
	// one wire bus
	volatile struct soft_ow sow_ctx;

	// TWI interface
	volatile struct twi_ctx *twi_ctx = NULL;

	// led pin
	volatile gpio_pin led_pin;

	// lcd display
	volatile struct lcd_ctx lcd_ctx = {0x00};

	// system settings	
	volatile struct sys_settings settings = {0x00};

	// time
	struct time_ctx tm = {{0x00}};

	// temp
	volatile struct temp_ctx temp = {{0x00}};

	// main state machine
	struct fsm_t fsm = {{0x00}};
	struct fsm_pd fsmpd = {0x00};

	// get system settings from eeprom
	sys_settings_get(&settings);

	// setup buses
	bus_twi_setup((struct twi_ctx **)&twi_ctx);
	bus_ow_setup((struct soft_ow *)&sow_ctx);

	// setup the LED
	led_setup((gpio_pin *)&led_pin);

	// setup the display	
	lcd_setup((struct dev_hd44780_ctx *)&lcd_ctx.dev);
	lcd_ctx.settings = &settings;
	lcd_ctx._lcd_backlight_timer = settings.lcd_bt_time;

	// setup clock
	rtc_setup(twi_ctx);

	// disable interrupts temporary & 
	// continue with HW init
	cli();
	timers_setup();
	temp.sow_ctx = &sow_ctx;
	tmp_setup(&temp);

	fsmpd.tm = &tm;
	fsmpd.temp = &temp;
	fsmpd.lcd = &lcd_ctx;
	fsmpd.ss = &settings;
	fsmpd.eq = &fsm.eq;

	fsm_init(&fsm, &fsmpd);
	g_int_ctx._event_timer = settings.time_time;
	
	// event queue pointer for the interrupts
	g_int_ctx.eq = &fsm.eq;
	g_int_ctx.led = &led_pin;
	g_int_ctx.lcd = &lcd_ctx;
	g_int_ctx.tmp = &temp;

	// now it's safe to globally enable ints
	// HW should be up & running
	sei();

	// restore saved contrast value
	SET_CONTRAST(settings.lcd_contrast);

	serial_init(E_BAUD_9600);	
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_flush();
	serial_install_stdio();

	// execution loop
	for (;;) {

		// pop an event
		uint8_t event = fsm_event_pop(&fsm.eq);
		volatile uint8_t buttons = 0;		

		// perform temperature measurement
		tmp_update_measurements(&temp);

		// update measurement on interrupt
		if (E_EVENT_1HZ == event) {
			uint8_t ptr = 0x00;
			twi_mtx(TWI_RTC_ADDR, &ptr, 0x01, 0x00);
			while (twi_ctx->status & E_TWI_BIT_BUSY);
			twi_mrx(TWI_RTC_ADDR, (uint8_t *)&tm.tm, 
					sizeof(tm.tm), 
					E_TWI_BIT_SEND_STOP);
			while (twi_ctx->status & E_TWI_BIT_BUSY);
			continue;
		}

		// execute the state machine
		fsm.cs = fsm.cs.cb(&fsm, event);

		// if button pressed
		if ((buttons = buttons_get())) {

			// refresh the timer
			lcd_ctx._lcd_backlight_timer = settings.lcd_bt_time;

			// push a key-press event
			fsm_event_push(&fsm.eq, 0x10 + buttons);
		}

		// poll every 10 ms (or even longer
		// when considering the processing time)
		_delay_ms(10);
	} // for
}
