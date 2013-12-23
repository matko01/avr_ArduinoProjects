#ifndef FSM_H_RWRTTM0S
#define FSM_H_RWRTTM0S

#include <stdint.h>

#define FSM_EVENT_QUEUE_LEN 8


/**
 * @brief default starting state for the FSM
 */
#define FSM_INITIAL_STATE fsm_state_disp_time


/**
 * @brief events 
 */
typedef enum _fsm_event_t {
	E_EVENT_TO = 0,
	E_EVENT_TRANSITION_END,
	E_EVENT_1HZ,

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


/**
 * @brief state action callback wrapped in a structure so, it's able to return itself
 */
struct fsm_t;
typedef struct f_state {
	struct f_state (*cb)(struct fsm_t*,uint8_t);
} f_state;


/**
 * @brief finite state machine declaration
 */
struct fsm_t {
	// current state
	f_state cs;

	// previous state
	f_state ps;

	// private data
	void *pd;

	// event queue
	struct event_queue eq;
};


// ================================================================================


/**
 * @brief fsm event queue - check how many events are pending
 *
 * @param eq pointer to event queue
 *
 * @return number of events pending
 */
uint8_t fsm_event_available(volatile struct event_queue *eq);

/**
 * @brief push an event to the queue
 *
 * @param eq pointer to the queue
 * @param event an event
 */
void fsm_event_push(volatile struct event_queue *eq, uint8_t event);

/**
 * @brief pop an event from the queue
 *
 * @param eq pointer to the queue
 *
 * @return event
 */
uint8_t fsm_event_pop(volatile struct event_queue *eq);


// ================================================================================


/**
 * @brief initialize the fsm
 *
 * @param a_fsm pointer to fsm
 */
void fsm_init(struct fsm_t *a_fsm, void *a_pd);


// ================================================================================


// state functions
f_state fsm_state_disp_time(struct fsm_t *a_fsm, uint8_t ev);
f_state fsm_state_disp_temp(struct fsm_t *a_fsm, uint8_t ev);
f_state fsm_state_disp_nm(struct fsm_t *a_fsm, uint8_t ev);
f_state fsm_state_disp_pv(struct fsm_t *a_fsm, uint8_t ev);
f_state fsm_state_disp_menu(struct fsm_t *a_fsm, uint8_t ev);
f_state fsm_state_scroll_time(struct fsm_t *a_fsm, uint8_t ev);
f_state fsm_state_scroll_temp(struct fsm_t *a_fsm, uint8_t ev);
f_state fsm_state_scroll_nm(struct fsm_t *a_fsm, uint8_t ev);
f_state fsm_state_scroll_pv(struct fsm_t *a_fsm, uint8_t ev);
f_state fsm_state_scroll_menu(struct fsm_t *a_fsm, uint8_t ev);


// ================================================================================


#endif /* FSM_H_RWRTTM0S */

