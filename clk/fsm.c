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
	uint8_t next = (eq->head + 1) % FSM_EVENT_QUEUE_LEN;
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

f_state fsm_state_disp_time(uint8_t ev) {
	static uint8_t cnt = 0;
	f_state state;
	state.cb = fsm_state_disp_time;

	// run the display procedure
	displayTime();

	switch (ev) {
		case E_EVENT_TO:
		case E_EVENT_BUTTON_OK:
			if (0 == (cnt++%2)) {
				state.cb = fsm_state_scroll_nm;
			}
			else if (0 == (cnt%3)) {
				state.cb = fsm_state_scroll_pv;
			}
			else {
				// scroll to temp
				state.cb = fsm_state_scroll_time;
			}
			g_sys_ctx._vis_pos = (LCD_CHARACTERS_PER_LINE + 1)*8;
			g_sys_ctx.fsm.ps.cb = fsm_state_disp_time;
			break;			

		case E_EVENT_BUTTON_MENU:
			state.cb = fsm_state_scroll_menu;
			g_sys_ctx._vis_pos = (LCD_CHARACTERS_PER_LINE + 1)*8;
			displayClean(1);
			g_sys_ctx.fsm.ps.cb = fsm_state_disp_time;
			break;
	}

	return state;
}


f_state fsm_state_disp_temp(uint8_t ev) {
	f_state state;
	state.cb = fsm_state_disp_temp;
	displayTemp();

	switch (ev) {
		case E_EVENT_TO:
		case E_EVENT_BUTTON_OK:
			state.cb = fsm_state_scroll_temp;
			g_sys_ctx.fsm.ps.cb = fsm_state_disp_temp;
			g_sys_ctx._vis_pos = 0;
			break;

		case E_EVENT_BUTTON_MENU:
			state.cb = fsm_state_scroll_menu;
			g_sys_ctx._vis_pos = 0;
			displayClean(0);
			g_sys_ctx.fsm.ps.cb = fsm_state_disp_temp;
			break;
	}

	return state;
}


f_state fsm_state_disp_nm(uint8_t ev) {
	f_state state;
	state.cb = fsm_state_disp_nm;
	displayNameday();

	switch (ev) {
		case E_EVENT_TO:
		case E_EVENT_BUTTON_OK:
			state.cb = fsm_state_scroll_nm;
			g_sys_ctx.fsm.ps.cb = fsm_state_disp_nm;
			g_sys_ctx._vis_pos = 0;
			break;

		case E_EVENT_BUTTON_MENU:
			state.cb = fsm_state_scroll_menu;
			g_sys_ctx._vis_pos = 0;
			displayClean(0);
			g_sys_ctx.fsm.ps.cb = fsm_state_disp_nm;
			break;
	}

	return state;
}


f_state fsm_state_disp_pv(uint8_t ev) {
	f_state state;
	state.cb = fsm_state_disp_pv;
	displayProverb();

	switch (ev) {
		case E_EVENT_TO:
		case E_EVENT_BUTTON_OK:
			state.cb = fsm_state_scroll_pv;
			g_sys_ctx.fsm.ps.cb = fsm_state_disp_pv;
			g_sys_ctx._vis_pos = 0;
			break;

		case E_EVENT_BUTTON_MENU:
			state.cb = fsm_state_scroll_menu;
			g_sys_ctx._vis_pos = 0;
			displayClean(0);
			g_sys_ctx.fsm.ps.cb = fsm_state_disp_pv;
			break;

	} // switch

	return state;
}


f_state fsm_state_disp_menu(uint8_t ev) {
	f_state state;
	state.cb = fsm_state_disp_menu;

	displayMenu();

	switch (ev) {
		case E_EVENT_TO:
		case E_EVENT_BUTTON_MENU:
			// go back to previous state
			state.cb = fsm_state_scroll_menu;
			g_sys_ctx.fsm.ps.cb = fsm_state_disp_menu;
			g_sys_ctx._vis_pos = g_sys_ctx._vis_pos ? 0 : ((LCD_CHARACTERS_PER_LINE + 1)*8);
			break;

		case E_EVENT_BUTTON_OK:
		case E_EVENT_BUTTON_MINUS:
		case E_EVENT_BUTTON_PLUS:
			menu_process_input(g_sys_ctx.menu, ev);
			g_sys_ctx._event_timer = 60;
			break;
	}
	return state;
}


/* ================================================================================ */

f_state fsm_state_scroll_time(uint8_t ev) {
	f_state state;
	state.cb = fsm_state_scroll_time;

	displayTime();
	displayTemp();

	switch (ev) {
		case E_EVENT_TRANSITION_END:
			state.cb = fsm_state_disp_temp;
			g_sys_ctx._event_timer = g_sys_ctx.settings.temp_time;
			break;

		default:
		case E_EVENT_TO:
			state.cb = fsm_state_scroll_time;
			break;
	}

	return state;
}


f_state fsm_state_scroll_temp(uint8_t ev) {
	f_state state;
	state.cb = fsm_state_scroll_temp;

	displayTime();
	displayTemp();

	switch (ev) {

		case E_EVENT_TRANSITION_END:
			state.cb = fsm_state_disp_time;
			g_sys_ctx._event_timer = g_sys_ctx.settings.time_time;
			break;

		default:
		case E_EVENT_TO:
			state.cb = fsm_state_scroll_temp;
			break;
	}

	return state;
}


f_state fsm_state_scroll_nm(uint8_t ev) {
	f_state state;
	state.cb = fsm_state_scroll_nm;

	displayTime();
	displayNameday();

	switch (ev) {

		case E_EVENT_TRANSITION_END:
			if (g_sys_ctx.fsm.ps.cb == fsm_state_disp_nm) {
				state.cb = fsm_state_disp_time;
				g_sys_ctx._event_timer = g_sys_ctx.settings.time_time;
			}
			else {
				state.cb = fsm_state_disp_nm;
				g_sys_ctx._event_timer = g_sys_ctx.settings.nm_time;
			}
			break;

		default:
		case E_EVENT_TO:
			state.cb = fsm_state_scroll_nm;
			break;
	}

	return state;
}


f_state fsm_state_scroll_pv(uint8_t ev) {
	f_state state;
	state.cb = fsm_state_scroll_pv;

	displayTime();
	displayProverb();

	switch (ev) {

		case E_EVENT_TRANSITION_END:
			if (g_sys_ctx.fsm.ps.cb == fsm_state_disp_pv) {
				state.cb = fsm_state_disp_time;
				g_sys_ctx._event_timer = g_sys_ctx.settings.time_time;
			}
			else {
				state.cb = fsm_state_disp_pv;
				g_sys_ctx._event_timer = g_sys_ctx.settings.pv_time;
			}
			g_sys_ctx.fsm.ps.cb = fsm_state_scroll_pv;
			break;

		default:
		case E_EVENT_TO:
			state.cb = fsm_state_scroll_pv;
			break;
	}

	return state;

}


f_state fsm_state_scroll_menu(uint8_t ev) {
	f_state state;
	state.cb = fsm_state_scroll_menu;

	if (g_sys_ctx.fsm.ps.cb != fsm_state_disp_menu) {
		// render menu only, the previous screen execution will be frozen
		// during the transition
		displayMenu();
	}

	switch (ev) {
		case E_EVENT_TRANSITION_END:
			if (g_sys_ctx.fsm.ps.cb == fsm_state_disp_menu) {
				// transition from menu to the original screen

				// restore the old state from the private data
				state = *((f_state *)g_sys_ctx.fsm.pd);
				// fixed time in order to make it simple
				g_sys_ctx._event_timer = 10;
			}
			else {
				// transition to menu screen

				// save the current state to private data				
				*((f_state *)g_sys_ctx.fsm.pd) = g_sys_ctx.fsm.ps;

				state.cb = fsm_state_disp_menu;
				// event timer should be refreshed with every button press in the menu
				g_sys_ctx._event_timer = 60;
			}
			break;

		default:
		case E_EVENT_TO:
			state.cb = fsm_state_scroll_menu;
			break;
	} // switch

	return state;
}

/* ================================================================================ */
