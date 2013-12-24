#ifndef __SYS_UTIL_H__
#define __SYS_UTIL_H__

#include "pca.h"

#include <stdint.h>


uint8_t is_leap_year(uint8_t a_year);
uint8_t get_month_days(uint8_t month, uint8_t year);
uint16_t get_year_day(ds1307_time_t *tm);

#endif /* __SYS_UTIL_H__ */
