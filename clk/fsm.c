#include "fsm.h"
#include "pca.h"
#include "sys_common.h"
#include "sys_ctx.h"

/* ================================================================================ */

void fsm_init(struct fsm_t *a_fsm) {
	a_fsm->cs.cb = fsm_state_disp_time;
	a_fsm->ps.cb = fsm_state_disp_time;
}

/* ================================================================================ */

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

/* ================================================================================ */

f_state fsm_state_disp_time(volatile struct sys_ctx *a_ctx, uint8_t ev) {
	static uint8_t cnt = 0;
	f_state state = {0x00};

	// run the display procedure
	displayTime(a_ctx);

	switch (ev) {
		case E_EVENT_TO:
		case E_EVENT_BUTTON_OK:
			if (!(cnt++%2)) {
				state.cb = fsm_state_scroll_nm;
			}
			else if (!(cnt%3)) {
				state.cb = fsm_state_scroll_pv;
			}
			else {
				// scroll to temp
				state.cb = fsm_state_scroll_time;
			}
			a_ctx->_vis_pos = (LCD_CHARACTERS_PER_LINE + 1)*8;
			a_ctx->fsm.ps.cb = fsm_state_disp_time;
			break;			

		case E_EVENT_BUTTON_MENU:
			state.cb = fsm_state_scroll_menu;
			a_ctx->_vis_pos = (LCD_CHARACTERS_PER_LINE + 1)*8;
			a_ctx->fsm.ps.cb = fsm_state_disp_time;
			break;

		default:
			state.cb = fsm_state_disp_time;
			break;
	}

	return state;
}


f_state fsm_state_disp_temp(volatile struct sys_ctx *a_ctx, uint8_t ev) {
	f_state state = {0x00};
	displayTemp(a_ctx);

	switch (ev) {
		case E_EVENT_TO:
		case E_EVENT_BUTTON_OK:
			state.cb = fsm_state_scroll_temp;
			a_ctx->fsm.ps.cb = fsm_state_disp_temp;
			a_ctx->_vis_pos = 0;
			break;

		case E_EVENT_BUTTON_MENU:
			state.cb = fsm_state_scroll_menu;
			a_ctx->_vis_pos = 0;
			a_ctx->fsm.ps.cb = fsm_state_disp_temp;
			break;

		default:
			state.cb = fsm_state_disp_temp;
			break;
	}

	return state;
}


f_state fsm_state_disp_nm(volatile struct sys_ctx *a_ctx, uint8_t ev) {
	f_state state = {0x00};
	displayNameday(a_ctx);

	switch (ev) {
		case E_EVENT_TO:
		case E_EVENT_BUTTON_OK:
			state.cb = fsm_state_scroll_nm;
			a_ctx->fsm.ps.cb = fsm_state_disp_nm;
			a_ctx->_vis_pos = 0;
			break;

		case E_EVENT_BUTTON_MENU:
			state.cb = fsm_state_scroll_menu;
			a_ctx->_vis_pos = 0;
			a_ctx->fsm.ps.cb = fsm_state_disp_nm;
			break;

		default:
			state.cb = fsm_state_disp_nm;
			break;
	}

	return state;
}


f_state fsm_state_disp_pv(volatile struct sys_ctx *a_ctx, uint8_t ev) {
	f_state state = {0x00};
	displayProverb(a_ctx);

	switch (ev) {
		case E_EVENT_TO:
		case E_EVENT_BUTTON_OK:
			state.cb = fsm_state_scroll_pv;
			a_ctx->fsm.ps.cb = fsm_state_disp_pv;
			a_ctx->_vis_pos = 0;
			break;

		case E_EVENT_BUTTON_MENU:
			state.cb = fsm_state_scroll_menu;
			a_ctx->_vis_pos = 0;
			a_ctx->fsm.ps.cb = fsm_state_disp_pv;
			break;

		default:
			state.cb = fsm_state_disp_pv;
			break;
	}

	return state;
}


f_state fsm_state_disp_menu(volatile struct sys_ctx *a_ctx, uint8_t ev) {
	f_state state = {0x00};
	state.cb = fsm_state_disp_menu;

	displayMenu(a_ctx);

	switch (ev) {
		case E_EVENT_TO:
		case E_EVENT_BUTTON_MENU:
			// go back to previous state
			break;

		case E_EVENT_BUTTON_OK:
			a_ctx->_event_timer = 60;
			break;

		case E_EVENT_BUTTON_MINUS:
			a_ctx->_event_timer = 60;
			break;

		case E_EVENT_BUTTON_PLUS:
			a_ctx->_event_timer = 60;
			break;
	}
	return state;
}


/* ================================================================================ */

f_state fsm_state_scroll_time(volatile struct sys_ctx *a_ctx, uint8_t ev) {
	f_state state = {0x00};
	displayTime(a_ctx);
	displayTemp(a_ctx);

	switch (ev) {
		case E_EVENT_TRANSITION_END:
			state.cb = fsm_state_disp_temp;
			a_ctx->_event_timer = a_ctx->settings.temp_time;
			break;

		default:
		case E_EVENT_TO:
			state.cb = fsm_state_scroll_time;
			break;
	}

	return state;
}


f_state fsm_state_scroll_temp(volatile struct sys_ctx *a_ctx, uint8_t ev) {
	f_state state = {0x00};
	displayTime(a_ctx);
	displayTemp(a_ctx);

	switch (ev) {

		case E_EVENT_TRANSITION_END:
			state.cb = fsm_state_disp_time;
			a_ctx->_event_timer = a_ctx->settings.time_time;
			break;

		default:
		case E_EVENT_TO:
			state.cb = fsm_state_scroll_temp;
			break;
	}

	return state;
}


f_state fsm_state_scroll_nm(volatile struct sys_ctx *a_ctx, uint8_t ev) {
	f_state state = {0x00};
	displayTime(a_ctx);
	displayNameday(a_ctx);

	switch (ev) {

		case E_EVENT_TRANSITION_END:
			if (a_ctx->fsm.ps.cb == fsm_state_disp_nm) {
				state.cb = fsm_state_disp_time;
				a_ctx->_event_timer = a_ctx->settings.time_time;
			}
			else {
				state.cb = fsm_state_disp_nm;
				a_ctx->_event_timer = a_ctx->settings.nm_time;
			}
			break;

		default:
		case E_EVENT_TO:
			state.cb = fsm_state_scroll_nm;
			break;
	}

	return state;
}


f_state fsm_state_scroll_pv(volatile struct sys_ctx *a_ctx, uint8_t ev) {
	f_state state = {0x00};
	displayTime(a_ctx);
	displayProverb(a_ctx);

	switch (ev) {

		case E_EVENT_TRANSITION_END:
			if (a_ctx->fsm.ps.cb == fsm_state_disp_pv) {
				state.cb = fsm_state_disp_time;
				a_ctx->_event_timer = a_ctx->settings.time_time;
			}
			else {
				state.cb = fsm_state_disp_pv;
				a_ctx->_event_timer = a_ctx->settings.pv_time;
			}
			a_ctx->fsm.ps.cb = fsm_state_scroll_pv;
			break;

		default:
		case E_EVENT_TO:
			state.cb = fsm_state_scroll_pv;
			break;
	}

	return state;

}


f_state fsm_state_scroll_menu(volatile struct sys_ctx *a_ctx, uint8_t ev) {
	f_state state = {0x00};

	// render menu only, the previous screen execution will be frozen
	// during the transition
	displayMenu(a_ctx);

	switch (ev) {

		case E_EVENT_TRANSITION_END:
			if (a_ctx->fsm.ps.cb == fsm_state_disp_menu) {
				state.cb = a_ctx->fsm.ps.cb;
				// fixed time in order to make it simple
				a_ctx->_event_timer = 10;
			}
			else {
				state.cb = fsm_state_disp_menu;
				// event timer should be refreshed with every button press in the menu
				a_ctx->_event_timer = 60;
			}
			break;

		default:
		case E_EVENT_TO:
			state.cb = fsm_state_scroll_menu;
			break;
	}

	return state;
}

/* ================================================================================ */
