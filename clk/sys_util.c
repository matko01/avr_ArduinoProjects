#include "sys_util.h"


uint8_t is_leap_year(uint8_t a_year) {
	uint16_t y = a_year + 2000;
	return ((y%4 == 0 && y%100 != 0) || y%400 == 0);
}


uint8_t get_month_days(uint8_t month, uint8_t year) {
	uint8_t mon[] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};

	if (is_leap_year(year)) {
		mon[1] = 29;
	}

	return mon[month - 1];
}


uint16_t get_year_day(ds1307_time_t *tm) {
	uint16_t day = 0;
	int8_t i = tm->month;

	while (--i) {
		day += get_month_days(i, BCD2BIN(tm->year));
	}

	day += tm->dom;
	return day;
}


uint8_t get_day_of_week(uint16_t y, uint8_t m, uint8_t d) {
	uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	y -= m < 3;
	return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

