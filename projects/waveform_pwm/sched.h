#ifndef __SCHED_H__
#define __SCHED_H__

#include "config.h"
#include "common.h"

/**
 * @brief time resolution definition for scheduler
 */
#define SCHED_TIME_RESOLUTION uint8_t


/**
 * @brief tick time resolution definition (by default a single tick is a millisecond)
 */
#define SCHED_TICK_FREQUENCY 1000


/**
 * @brief which timer to use as a tick generator
 */
#define SCHED_TIMER 1


/**
 * @brief maximum number of tasks allowed to run simultaneously
 */
#define SCHED_MAX_TASKS 32


/**
 * @brief time resolution type
 */
typedef SCHED_TIME_RESOLUTION t_timeres;


/**
 * @brief callback handler format
 *
 * @param a_data custom data
 *
 * @return none
 */
typedef void (*t_fh)(void *a_data);


/**
 * @brief task status bitmasks
 */
#define SCHED_TS_NOT_EXECUTABLE 0x00
#define SCHED_TS_EXECUTABLE 0x02
#define SCHED_TS_PAUZED 0x00
#define SCHED_TS_RUNNING 0x01


/**
 * @brief create a compound variable
 */
#define SCHED_COMBINE_TS_ID(__ts, __id) ((__ts << 6) | (__id & 0x3f))

/**
 * @brief check if task is executable
 */
#define SCHED_TASK_IS_EXECUTABLE(__id) ( __id & (SCHED_TS_EXECUTABLE << 6))


/**
 * @brief task struct
 */
typedef struct _t_task {
	uint8_t ts_id; // uppder bit holds the state, lower 7 bits hold the task id

	t_timeres interval_static;
	volatile t_timeres interval_dynamic;
	
	t_fh handler;
	struct _t_task *prv, *nxt;
} t_task;


void sched_init();
uint8_t sched_task_new(t_fh a_handler, t_timeres a_interval);
void sched_task_delete(uint8_t a_id);
void sched_task_pauze(uint8_t a_id);
void sched_task_resume(uint8_t a_id);
void sched_run();
uint8_t sched_task_count();

#endif /* __SCHED_H__ */
