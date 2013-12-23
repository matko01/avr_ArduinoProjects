#ifndef __SYS_DISPLAY_H__
#define __SYS_DISPLAY_H__

#include "rtc.h"
#include "lcd.h"
#include "temperature.h"

#include <stdint.h>

void display_time(struct lcd_ctx *a_lcd, struct time_ctx *tm);
void display_temp(struct lcd_ctx *a_lcd, struct temp_msr_ctx *temp);
void display_nameday(struct lcd_ctx *a_lcd, ds1307_time_t *tm);
void display_proverb(struct lcd_ctx *a_lcd, ds1307_time_t *tm);
 
#endif /* __SYS_DISPLAY_H__ */
