#ifndef FSM_H_RWRTTM0S
#define FSM_H_RWRTTM0S

#include <stdint.h>


typedef enum _fsm_state_t {
	E_STATE_TIME = 0,
	E_STATE_TEMPERATURE,	
	E_STATE_MENU,

	E_STATE_LAST
} fsm_state_t;


typedef enum _fsm_event_t {
	E_EVENT_TIME_TO = 0,
	E_EVENT_TEMP_TO,

	E_EVENT_LAST
} fsm_event_t;


typedef void (*fsm_trans_t)(void*);


struct fsm_st {
	uint8_t cur_state;
	uint8_t new_state;
	fsm_trans_t trans;
};


struct fsm_ctx {
	uint8_t cstate;
	// truth table
	struct fsm_st *tt;
};


#endif /* FSM_H_RWRTTM0S */

