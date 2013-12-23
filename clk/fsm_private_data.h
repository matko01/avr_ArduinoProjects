#ifndef FSM_PRIVATE_DATA_H_GH5XKFUI
#define FSM_PRIVATE_DATA_H_GH5XKFUI

#include "lcd.h"
#include "rtc.h"
#include "temperature.h"
#include "sys_settings.h"
#include "fsm.h"


struct fsm_pd {
	struct time_ctx *tm;
	volatile struct temp_ctx *temp;
	volatile struct sys_settings *ss;
	volatile struct lcd_ctx *lcd;
	volatile struct event_queue *eq;
};


#endif /* end of include guard: FSM_PRIVATE_DATA_H_GH5XKFUI */

