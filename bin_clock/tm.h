#ifndef TM_H_OEWRIIZJ
#define TM_H_OEWRIIZJ

/**
 * @brief time structure
 */
struct tm {
	unsigned char hours;
	unsigned char mins;
	unsigned char secs;
};


// increment time by a smallest unit (second)
void bc_inc_time(struct tm *a_data);


#endif /* end of include guard: TM_H_OEWRIIZJ */

