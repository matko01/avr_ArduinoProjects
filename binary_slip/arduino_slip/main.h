#ifndef MAIN_H_B62AZO9J
#define MAIN_H_B62AZO9J

#include <stdint.h>

// modify this to change the timer resolution
typedef uint8_t st_type;

/**
 * @brief software timer callback handler type declaration
 *
 * @return void
 */
typedef void (*st_cb)(void);


/**
 * @brief software timer declaration
 */
struct soft_timer {
	volatile st_type cnt;
	volatile st_type max;
	st_cb cb_overflow;
};

#endif /* end of include guard: MAIN_H_B62AZO9J */

