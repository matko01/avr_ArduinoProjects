#ifndef FSM_PRIVATE_DATA_H_GH5XKFUI
#define FSM_PRIVATE_DATA_H_GH5XKFUI

#include "lcd.h"
#include "rtc.h"
#include "temperature.h"
#include "sys_settings.h"
#include "fsm.h"


struct fsm_pd {
	struct time_ctx *tm;
	struct temp_ctx *temp;
	struct sys_settings *ss;
	volatile struct lcd_ctx *lcd;
	struct event_queue *eq;
};


#endif /* end of include guard: FSM_PRIVATE_DATA_H_GH5XKFUI */

