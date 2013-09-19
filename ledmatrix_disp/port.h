#ifndef PORT_H_YGLBOTDE
#define PORT_H_YGLBOTDE

/**
 * @brief port description
 */
struct t_pin {
	volatile unsigned char *port;
	unsigned char pin;
};

#define PIN_SET(__pin) \
	*(__pin)->port |= (unsigned char)_BV((__pin)->pin)

#define PIN_CLEAR(__pin) \
	*(__pin)->port &= ~(unsigned char)_BV((__pin)->pin)

#define PIN_SET_VALUE(__pin, __value) \
	if (__value) \
		PIN_SET(__pin); \
	else \
		PIN_CLEAR(__pin);

void pin_conf_output(volatile struct t_pin *a_pin);


#endif /* end of include guard: PORT_H_YGLBOTDE */

