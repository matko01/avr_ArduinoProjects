#include "sys_util.h"


uint8_t is_leap_year(uint8_t a_year) {
	uint16_t y = a_year + 2000;
	return ((y%4 == 0 && y%100 != 0) || y%400 == 0);
}


uint16_t get_year_day(ds1307_time_t *tm) {
	uint16_t day = 0;
	uint8_t mon[] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};

	if (is_leap_year(tm->year)) {
		mon[1] = 29;
	}

	for (int8_t i = (tm->month - 1); i>0; i--) {
		day += mon[i - 1];
	}

	day += tm->dom;
	return day;
}

