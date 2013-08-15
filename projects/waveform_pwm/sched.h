#ifndef __SCHED_H__
#define __SCHED_H__

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
#define SCHED_MAX_TASKS 64


/**
 * @brief time resolution type
 */
typedef TIME_RESOLUTION t_timeres;


/**
 * @brief callback handler format
 *
 * @param a_data custom data
 *
 * @return none
 */
typedef void (*t_fh)(void *a_data);


/**
 * @brief task status
 */
#define SCHED_TS_KILLED 0
#define SCHED_TS_STOPPED 1 
#define SCHED_TS_PAUZED 2
#define SCHED_TS_RUNNING 3


/**
 * @brief create a compound variable
 */
#define SCHED_COMBINE_TS_ID(__ts, __id) ((__ts << 6) | (__id & 0x3f))


/**
 * @brief task struct
 */
typedef struct _t_task {
	uint8_t ts_id; // two uppder bits hold the state, lower 6 bits hold the task id
	t_timeres interval;
	volatile t_timeres counter;
	t_fh handler;
} t_task;


void sched_init();
uint8_t sched_task_new(t_fh a_handler, t_timeres a_interval);
void sched_task_delete(uint8_t a_id);
void sched_task_pauze(uint8_t a_id);
void sched_task_resume(uint8_t a_id);
void sched_run();

#endif /* __SCHED_H__ */
