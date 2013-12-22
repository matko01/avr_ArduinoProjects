#ifndef FSM_H_RWRTTM0S
#define FSM_H_RWRTTM0S

#include <stdint.h>

#define FSM_EVENT_QUEUE_LEN 4
#define FSM_PRIVATE_DATA 4


typedef enum _fsm_event_t {
	E_EVENT_TO = 0,
	E_EVENT_TRANSITION_END,

	// button events
	E_EVENT_BUTTON_MENU = 0x11,
	E_EVENT_BUTTON_MINUS = 0x12,
	E_EVENT_BUTTON_PLUS = 0x14,
	E_EVENT_BUTTON_PM = 0x16,
	E_EVENT_BUTTON_OK = 0x18,

	// no event
	E_EVENT_NONE
} fsm_event_t;


/**
 * @brief event queue
 */
struct event_queue {
	uint8_t events[FSM_EVENT_QUEUE_LEN];
	uint8_t head;
	uint8_t tail;
};


// state action callback
typedef struct f_state {
	struct f_state (*cb)(uint8_t);
} f_state;


struct fsm_t {
	// current state
	f_state cs;

	// previous state
	f_state ps;

	// private data
	uint8_t pd[FSM_PRIVATE_DATA];
};


void fsm_init(struct fsm_t *a_fsm);

uint8_t fsm_event_available(volatile struct event_queue *eq);
void fsm_event_push(volatile struct event_queue *eq, uint8_t event);
uint8_t fsm_event_pop(volatile struct event_queue *eq);

// state functions
f_state fsm_state_disp_time(uint8_t ev);
f_state fsm_state_disp_temp(uint8_t ev);
f_state fsm_state_disp_nm(uint8_t ev);
f_state fsm_state_disp_pv(uint8_t ev);
f_state fsm_state_disp_menu(uint8_t ev);
f_state fsm_state_scroll_time(uint8_t ev);
f_state fsm_state_scroll_temp(uint8_t ev);
f_state fsm_state_scroll_nm(uint8_t ev);
f_state fsm_state_scroll_pv(uint8_t ev);
f_state fsm_state_scroll_menu(uint8_t ev);

#endif /* FSM_H_RWRTTM0S */

