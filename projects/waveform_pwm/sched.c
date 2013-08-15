#include "sched.h"
#include <string.h>

/* ================================================================================ */

static volatile t_task g_tasks[SCHED_MAX_TASKS];

/* ================================================================================ */

void sched_init() {
	memset(g_tasks, 0x00, sizeof(g_tasks));
}

uint8_t sched_task_new(t_fh a_handler, t_timeres a_interval) {
}

void sched_task_delete(uint8_t a_id) {
}

void sched_task_pauze(uint8_t a_id) {
	g_tasks[a_id].ts_id &= 0x3f;
	g_tasks[a_id].ts_id |= (SCHED_TS_PAUZED << 6);
}

void sched_task_resume(uint8_t a_id) {
	g_tasks[a_id].ts_id &= 0x3f;
	g_tasks[a_id].ts_id |= (SCHED_TS_RUNNING << 6);
}

void sched_run() {
}

