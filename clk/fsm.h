#ifndef FSM_H_RWRTTM0S
#define FSM_H_RWRTTM0S

#include <stdint.h>

#define FSM_EVENT_QUEUE_LEN 8


typedef enum _fsm_event_t {
	E_EVENT_TO = 0,
	E_EVENT_TRANSITION_END,

	// no event
	E_EVENT_NONE
} fsm_event_t;


typedef enum _fsm_state_t {
	E_DISP_TIME = 0,
	E_DISP_TEMP,
	E_SCROLL_TIME,
	E_SCROLL_TEMP,
	
	// invalid state
	E_INVALID
} fsm_state_t;


struct event_queue {
	uint8_t events[FSM_EVENT_QUEUE_LEN];
	uint8_t head;
	uint8_t tail;
};

// forward declaration
struct sys_ctx;
// state action callback
typedef uint8_t (*f_state_cb)(volatile struct sys_ctx*, uint8_t);


struct fsm_t {
	// current state
	uint8_t cs; 
};


void fsm_setup_cb(f_state_cb *scb);

uint8_t fsm_event_available(volatile struct event_queue *eq);
void fsm_event_push(volatile struct event_queue *eq, uint8_t event);
uint8_t fsm_event_pop(volatile struct event_queue *eq);

// state functions
uint8_t fsm_state_disp_time(volatile struct sys_ctx *a_ctx, uint8_t ev);
uint8_t fsm_state_disp_temp(volatile struct sys_ctx *a_ctx, uint8_t ev);
uint8_t fsm_state_scroll_time(volatile struct sys_ctx *a_ctx, uint8_t ev);
uint8_t fsm_state_scroll_temp(volatile struct sys_ctx *a_ctx, uint8_t ev);


#endif /* FSM_H_RWRTTM0S */

