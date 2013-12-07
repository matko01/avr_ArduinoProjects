#ifndef __SYS_COMMON_H__
#define __SYS_COMMON_H__

#include "pca.h"
#include "sys_ctx.h"


void timers_setup();
void rtc_setup(volatile struct twi_ctx *a_ctx);

void bus_ow_setup(struct soft_ow *a_sow_ctx);
void bus_twi_setup(struct twi_ctx **a_ctx);

void sys_settings_get(struct sys_settings *a_ss);


#endif /* __SYS_COMMON_H__ */
