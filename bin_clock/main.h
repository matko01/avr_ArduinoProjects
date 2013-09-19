#ifndef __MAIN_H__
#define __MAIN_H__

// pca
#include "common.h"
#include "serial.h"
#include "slip.h"

// arduino OS
#include "aos.h"

// own
#include "util.h"
#include "tm.h"

/**
 * @brief task enumeration
 */
enum tasks {
	TASK_TIMER = 0,
	TASK_SERIAL_SYNC,
	TASK_MULTIPLEXER,

	// used for allocations
	TASK_LAST
};


/**
 * @brief system context
 */
struct ctx {
	struct tm g_time;	
	struct task_cb *tasks[TASK_LAST];
	uint8_t row;
};

#endif /* __MAIN_H__ */
