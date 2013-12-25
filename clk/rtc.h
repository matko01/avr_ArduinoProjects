#ifndef __RTC_H__
#define __RTC_H__

#include "pca.h"
#include "temperature.h"

#include <stdint.h>


/**
 * @brief settings magic identifier
 */
#define RTC_MAGIC_ID 0x0e


struct time_ctx {
	// time
	ds1307_time_t tm;
	volatile struct twi_ctx *twi;
};


/**
 * @brief initialize the RTC hardware and it's settings
 *
 * @param a_ctx system context
 */
void rtc_setup(volatile struct twi_ctx *a_ctx, struct temp_msr_ctx *msr);
void rtc_store_temperatures(volatile struct twi_ctx *a_ctx, struct temp_msr_ctx *msr_ctx);
void rtc_restore_temperatures(volatile struct twi_ctx *a_ctx, struct temp_msr_ctx *msr_ctx);


#endif /* __RTC_H__ */
