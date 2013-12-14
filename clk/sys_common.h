#ifndef __SYS_COMMON_H__
#define __SYS_COMMON_H__

#include "pca.h"
#include "sys_ctx.h"

#define SET_CONTRAST(__contrast) OCR2B = __contrast

void timers_setup();
void rtc_setup(volatile struct twi_ctx *a_ctx);
void bus_ow_setup(struct soft_ow *a_sow_ctx);
void bus_twi_setup(struct twi_ctx **a_ctx);
void led_setup(volatile struct sys_ctx *a_ctx);

void sys_settings_get(struct sys_settings *a_ss);

void displayTemp(volatile struct sys_ctx *a_ctx);
void displayTime(volatile struct sys_ctx *a_ctx);
void displayNameday(volatile struct sys_ctx *a_ctx);


#endif /* __SYS_COMMON_H__ */
