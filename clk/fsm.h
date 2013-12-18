#ifndef FSM_H_RWRTTM0S
#define FSM_H_RWRTTM0S

#include <stdint.h>

#define FSM_EVENT_QUEUE_LEN 8


typedef enum _fsm_event_t {
	E_EVENT_TO = 0,
	E_EVENT_TRANSITION_END,
	E_EVENT_BUTTON_MENU,
	E_EVENT_BUTTON_MINUS,
	E_EVENT_BUTTON_PLUS,
	E_EVENT_BUTTON_OK,

	// no event
	E_EVENT_NONE
} fsm_event_t;


struct event_queue {
	uint8_t events[FSM_EVENT_QUEUE_LEN];
	uint8_t head;
	uint8_t tail;
};

// forward declaration
struct sys_ctx;

// state action callback
typedef struct f_state {
	struct f_state (*cb)(volatile struct sys_ctx*, uint8_t);
} f_state;


struct fsm_t {
	// current state
	f_state cs;

	// previous state
	f_state ps;
};


void fsm_init(struct fsm_t *a_fsm);

uint8_t fsm_event_available(volatile struct event_queue *eq);
void fsm_event_push(volatile struct event_queue *eq, uint8_t event);
uint8_t fsm_event_pop(volatile struct event_queue *eq);

// state functions
f_state fsm_state_disp_time(volatile struct sys_ctx *a_ctx, uint8_t ev);
f_state fsm_state_disp_temp(volatile struct sys_ctx *a_ctx, uint8_t ev);
f_state fsm_state_disp_nm(volatile struct sys_ctx *a_ctx, uint8_t ev);
f_state fsm_state_disp_pv(volatile struct sys_ctx *a_ctx, uint8_t ev);
f_state fsm_state_disp_menu(volatile struct sys_ctx *a_ctx, uint8_t ev);
f_state fsm_state_scroll_time(volatile struct sys_ctx *a_ctx, uint8_t ev);
f_state fsm_state_scroll_temp(volatile struct sys_ctx *a_ctx, uint8_t ev);
f_state fsm_state_scroll_nm(volatile struct sys_ctx *a_ctx, uint8_t ev);
f_state fsm_state_scroll_pv(volatile struct sys_ctx *a_ctx, uint8_t ev);
f_state fsm_state_scroll_menu(volatile struct sys_ctx *a_ctx, uint8_t ev);

#endif /* FSM_H_RWRTTM0S */

