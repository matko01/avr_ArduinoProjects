#include "fsm.h"
#include "pca.h"
#include "sys_common.h"
#include "sys_ctx.h"



void fsm_setup_cb(f_state_cb *scb) {
	scb[E_DISP_TIME] = fsm_state_disp_time;
	scb[E_DISP_TEMP] = fsm_state_disp_temp;
	scb[E_SCROLL_TIME] = fsm_state_scroll_time;
	scb[E_SCROLL_TEMP] = fsm_state_scroll_temp;
}


__inline__
uint8_t fsm_event_available(volatile struct event_queue *eq) {
	return (FSM_EVENT_QUEUE_LEN + eq->head - eq->tail) % FSM_EVENT_QUEUE_LEN;
}


void fsm_event_push(volatile struct event_queue *eq, uint8_t event) {
	unsigned char next = (eq->head + 1) % FSM_EVENT_QUEUE_LEN;
	if (next != eq->tail) {
		eq->events[eq->head] = event;
		eq->head = next;
	}
}


uint8_t fsm_event_pop(volatile struct event_queue *eq) {

	uint8_t ev = E_EVENT_NONE;

	if (eq->head != eq->tail) {
		ev = eq->events[eq->tail];
		eq->tail = (eq->tail + 1) % FSM_EVENT_QUEUE_LEN;
	}

	return ev;
}


uint8_t fsm_state_disp_time(volatile struct sys_ctx *a_ctx, uint8_t ev) {
	uint8_t state;
	displayTime(a_ctx);

	switch (ev) {
		case E_EVENT_TO:
			state = E_SCROLL_TIME;
			a_ctx->_vis_pos = (LCD_CHARACTERS_PER_LINE + 1)*8;
			break;

		default:
			state = E_DISP_TIME;
			break;
	}

	return state;
}


uint8_t fsm_state_disp_temp(volatile struct sys_ctx *a_ctx, uint8_t ev) {
	uint8_t state;
	displayTemp(a_ctx);

	switch (ev) {
		case E_EVENT_TO:
			state = E_SCROLL_TEMP;
			a_ctx->_vis_pos = 0;
			break;

		default:
			state = E_DISP_TEMP;
			break;
	}

	return state;
}


uint8_t fsm_state_scroll_time(volatile struct sys_ctx *a_ctx, uint8_t ev) {
	uint8_t state;
	displayTime(a_ctx);
	displayTemp(a_ctx);

	switch (ev) {
		case E_EVENT_TRANSITION_END:
			state = E_DISP_TEMP;
			a_ctx->_event_timer = a_ctx->settings.temp_time;
			break;

		default:
		case E_EVENT_TO:
			state = E_SCROLL_TIME;
			break;
	}

	return state;
}


uint8_t fsm_state_scroll_temp(volatile struct sys_ctx *a_ctx, uint8_t ev) {
	uint8_t state;
	displayTime(a_ctx);
	displayTemp(a_ctx);


	switch (ev) {

		case E_EVENT_TRANSITION_END:
			state = E_DISP_TIME;
			a_ctx->_event_timer = a_ctx->settings.time_time;
			break;

		default:
		case E_EVENT_TO:
			state = E_SCROLL_TEMP;
			break;
	}

	return state;
}
