#include "tm.h"
#include "util.h"

void bc_inc_time(struct tm *a_data) {
	// increment by a second
	++a_data->secs;
	bc_bcd_correct(&(a_data->secs));

	if (a_data->secs >= 0x60) {
		a_data->secs = 0x00;
		++a_data->mins;
		bc_bcd_correct(&(a_data->mins));
	}

	if (a_data->mins >= 0x60) {
		a_data->mins = 0x00;
		++a_data->hours;
		bc_bcd_correct(&(a_data->hours));
	}

	if (a_data->hours >= 0x24) {
		a_data->hours = 0x00;
	}
}

