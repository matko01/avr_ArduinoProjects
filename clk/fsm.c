#include "fsm.h"
#include "fsm_private_data.h"
#include "pca.h"
#include "sys_display.h"
#include "lcd.h"
#include "int_ctx.h"
#include "menu.h"
#include "main_menu.h"


/* ================================================================================ */


void fsm_init(struct fsm_t *a_fsm, void *a_pd) {
	a_fsm->cs.cb = FSM_INITIAL_STATE;
	a_fsm->ps.cb = FSM_INITIAL_STATE;
	a_fsm->pd = a_pd;
	menu_set_private_data(&g_main_menu, a_pd);
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

f_state fsm_state_disp_time(struct fsm_t *a_fsm, uint8_t ev) {
	static uint8_t cnt = 0;
	struct fsm_pd *pd = (struct fsm_pd *)a_fsm->pd;

	f_state state;	
	state.cb = fsm_state_disp_time;

	// run the display procedure
	display_time((struct lcd_ctx *)pd->lcd, pd->tm);

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
			pd->lcd->_vis_pos = (LCD_CHARACTERS_PER_LINE + 1)*8;
			a_fsm->ps.cb = fsm_state_disp_time;
			break;			

		case E_EVENT_BUTTON_MENU:
			lcd_clean((struct lcd_ctx *)pd->lcd, 1);
			pd->lcd->_vis_pos = (LCD_CHARACTERS_PER_LINE + 1)*8;

			state.cb = fsm_state_scroll_menu;
			a_fsm->ps.cb = fsm_state_disp_time;
			break;
	}

	return state;
}


f_state fsm_state_disp_temp(struct fsm_t *a_fsm, uint8_t ev) {
	f_state state;
	state.cb = fsm_state_disp_temp;
	struct fsm_pd *pd = (struct fsm_pd *)a_fsm->pd;

	display_temp((struct lcd_ctx *)pd->lcd, &pd->temp->msr);

	switch (ev) {
		case E_EVENT_TO:
		case E_EVENT_BUTTON_OK:
			state.cb = fsm_state_scroll_temp;
			a_fsm->ps.cb = fsm_state_disp_temp;
			pd->lcd->_vis_pos = 0;
			break;

		case E_EVENT_BUTTON_MENU:
			state.cb = fsm_state_scroll_menu;
			lcd_clean((struct lcd_ctx *)pd->lcd, 0);
			pd->lcd->_vis_pos = 0;
			a_fsm->ps.cb = fsm_state_disp_temp;
			break;
	}

	return state;
}


f_state fsm_state_disp_nm(struct fsm_t *a_fsm, uint8_t ev) {
	f_state state;
	state.cb = fsm_state_disp_nm;
	struct fsm_pd *pd = (struct fsm_pd *)a_fsm->pd;

	// run the display procedure
	display_nameday((struct lcd_ctx *)pd->lcd, &pd->tm->tm);

	switch (ev) {
		case E_EVENT_TO:
		case E_EVENT_BUTTON_OK:
			state.cb = fsm_state_scroll_nm;
			a_fsm->ps.cb = fsm_state_disp_nm;
			pd->lcd->_vis_pos = 0;
			break;

		case E_EVENT_BUTTON_MENU:
			state.cb = fsm_state_scroll_menu;
			pd->lcd->_vis_pos = 0;
			lcd_clean((struct lcd_ctx *)pd->lcd, 0);
			a_fsm->ps.cb = fsm_state_disp_nm;
			break;
	}

	return state;
}


f_state fsm_state_disp_pv(struct fsm_t *a_fsm, uint8_t ev) {
	f_state state;
	state.cb = fsm_state_disp_pv;
	struct fsm_pd *pd = (struct fsm_pd *)a_fsm->pd;

	// run the display procedure
	display_proverb((struct lcd_ctx *)pd->lcd, &pd->tm->tm);

	switch (ev) {
		case E_EVENT_TO:
		case E_EVENT_BUTTON_OK:
			state.cb = fsm_state_scroll_pv;
			a_fsm->ps.cb = fsm_state_disp_pv;
			pd->lcd->_vis_pos = 0;
			break;

		case E_EVENT_BUTTON_MENU:
			state.cb = fsm_state_scroll_menu;
			pd->lcd->_vis_pos = 0;
			lcd_clean((struct lcd_ctx *)pd->lcd, 0);
			a_fsm->ps.cb = fsm_state_disp_pv;
			break;

	} // switch

	return state;
}


f_state fsm_state_disp_menu(struct fsm_t *a_fsm, uint8_t ev) {
	f_state state;
	state.cb = fsm_state_disp_menu;
	struct fsm_pd *pd = (struct fsm_pd *)a_fsm->pd;

	menu_render((struct lcd_ctx *)pd->lcd, &g_main_menu);

	switch (ev) {
		case E_EVENT_TO:
		case E_EVENT_BUTTON_MENU:
			// go back to previous state
			state.cb = fsm_state_scroll_menu;
			a_fsm->ps.cb = fsm_state_disp_menu;
			pd->lcd->_vis_pos = pd->lcd->_vis_pos ? 0 : ((LCD_CHARACTERS_PER_LINE + 1)*8);
			break;

		case E_EVENT_BUTTON_OK:
		case E_EVENT_BUTTON_MINUS:
		case E_EVENT_BUTTON_PLUS:
			menu_process_input(&g_main_menu, ev);
			g_int_ctx._event_timer = 60;
			break;
	}
	return state;
}



f_state fsm_state_scroll_time(struct fsm_t *a_fsm, uint8_t ev) {
	f_state state;
	state.cb = fsm_state_scroll_time;
	struct fsm_pd *pd = (struct fsm_pd *)a_fsm->pd;

	display_time((struct lcd_ctx *)pd->lcd, pd->tm);
	display_temp((struct lcd_ctx *)pd->lcd, &pd->temp->msr);

	switch (ev) {
		case E_EVENT_TRANSITION_END:
			state.cb = fsm_state_disp_temp;
			g_int_ctx._event_timer = pd->ss->temp_time;
			break;

		default:
		case E_EVENT_TO:
			state.cb = fsm_state_scroll_time;
			break;
	}

	return state;
}


f_state fsm_state_scroll_temp(struct fsm_t *a_fsm, uint8_t ev) {
	f_state state;
	state.cb = fsm_state_scroll_temp;
	struct fsm_pd *pd = (struct fsm_pd *)a_fsm->pd;

	display_time((struct lcd_ctx *)pd->lcd, pd->tm);
	display_temp((struct lcd_ctx *)pd->lcd, &pd->temp->msr);

	switch (ev) {

		case E_EVENT_TRANSITION_END:
			state.cb = fsm_state_disp_time;
			g_int_ctx._event_timer = pd->ss->time_time;
			break;

		default:
		case E_EVENT_TO:
			state.cb = fsm_state_scroll_temp;
			break;
	}

	return state;
}


f_state fsm_state_scroll_nm(struct fsm_t *a_fsm, uint8_t ev) {
	f_state state;
	state.cb = fsm_state_scroll_nm;
	struct fsm_pd *pd = (struct fsm_pd *)a_fsm->pd;

	display_time((struct lcd_ctx *)pd->lcd, pd->tm);
	display_nameday((struct lcd_ctx *)pd->lcd, &pd->tm->tm);

	switch (ev) {

		case E_EVENT_TRANSITION_END:
			if (a_fsm->ps.cb == fsm_state_disp_nm) {
				state.cb = fsm_state_disp_time;
				g_int_ctx._event_timer = pd->ss->time_time;
			}
			else {
				state.cb = fsm_state_disp_nm;
				g_int_ctx._event_timer = pd->ss->nm_time;
			}
			break;

		default:
		case E_EVENT_TO:
			state.cb = fsm_state_scroll_nm;
			break;
	}

	return state;
}


f_state fsm_state_scroll_pv(struct fsm_t *a_fsm, uint8_t ev) {
	f_state state;
	state.cb = fsm_state_scroll_pv;
	struct fsm_pd *pd = (struct fsm_pd *)a_fsm->pd;

	display_time((struct lcd_ctx *)pd->lcd, pd->tm);
	display_proverb((struct lcd_ctx *)pd->lcd, &pd->tm->tm);

	switch (ev) {

		case E_EVENT_TRANSITION_END:
			if (a_fsm->ps.cb == fsm_state_disp_pv) {
				state.cb = fsm_state_disp_time;
				g_int_ctx._event_timer = pd->ss->time_time;
			}
			else {
				state.cb = fsm_state_disp_pv;
				g_int_ctx._event_timer = pd->ss->pv_time;
			}
			a_fsm->ps.cb = fsm_state_scroll_pv;
			break;

		default:
		case E_EVENT_TO:
			state.cb = fsm_state_scroll_pv;
			break;
	}

	return state;

}


f_state fsm_state_scroll_menu(struct fsm_t *a_fsm, uint8_t ev) {
	f_state state;
	state.cb = fsm_state_scroll_menu;
	struct fsm_pd *pd = (struct fsm_pd *)a_fsm->pd;
	static f_state ps = {0x00};

	if (a_fsm->ps.cb != fsm_state_disp_menu) {
		// render menu only, the previous screen execution will be frozen
		// during the transition
		menu_render((struct lcd_ctx *)pd->lcd, &g_main_menu);
	}

	switch (ev) {
		case E_EVENT_TRANSITION_END:
			if (a_fsm->ps.cb == fsm_state_disp_menu) {
				// transition from menu to the original screen

				// restore the old state from the private data
				state = ps;

				// fixed time in order to make it simple
				g_int_ctx._event_timer = 10;
			}
			else {
				// transition to menu screen

				// save the current state to private data				
				ps = a_fsm->ps;

				state.cb = fsm_state_disp_menu;
				// event timer should be refreshed with every button press in the menu
				g_int_ctx._event_timer = 60;
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
