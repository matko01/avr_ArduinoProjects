#include "pca.h"
#include "../model.h"
#include "main.h"

#include <avr/interrupt.h>
#include <util/atomic.h>

#define LED_DDRD DDRB
#define LED_PORT PORTB
#define LED_PIN PORTB0


/**
 * @brief global software timer variable declaration
 */
static volatile struct soft_timer g_st;


// interrupt service routine for Timer Compare A match
ISR(TIMER0_COMPA_vect) {

	// increment software timer
	g_st.cnt++;

	// if timer is higher than the programmed maximum
	if (g_st.cnt >= g_st.max) {

		// zero the timer
		g_st.cnt = 0x00;

		// run overflow callback
		if (NULL != g_st.cb_overflow) 
			g_st.cb_overflow();
	}

}


/**
 * @brief software timer overflow routine
 */
static void st_overflow(void) {
	// toggle the port bit on software timer overflow
	LED_PORT ^= _BV(LED_PIN);
}


int main(void)
{	
	uint8_t size = 0x00;
	struct response *resp;
	struct timer_data *data;

	// temporary buffer
	uint8_t buff[8] = {0};
	
	serial_init(E_BAUD_9600);
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_install_stdio();
	serial_flush();

	// configure it as output
	/* LED_DDRD |= _BV(LED_PIN); */
	LED_DDRD = 0xff;

	// initialize the software timer
	g_st.cnt = 0x00;
	g_st.max = 0x20; // = 32
	g_st.cb_overflow = st_overflow;

	// configure timer CTC mode
	TCCR0A = 0x02;
	TCCR0B = 0x05; // prescaler = 1024
	OCR0A = 0xff; // 255
	TIMSK0 |= _BV(OCIE0A);
	sei();

	// software timer will run with frequency bellow 1 Hz

	while (1) {

		// ignore reception errors
		if (0 >= (size = slip_recv((void *)buff, sizeof(buff)))) {
			continue;
		}

		// ignore incomplete data chunks
		if (size < sizeof(struct timer_data)) {
			continue;
		}

		// verify if the information is genuine
		if (!slip_verify_crc16(buff, sizeof(struct timer_data), 0)) {

			// send NACK if CRC fails
			resp = (struct response *)buff;
			resp->status = NACK;
			slip_prepend_crc16(buff, sizeof(struct response));
			slip_send(buff, sizeof(struct response));
			continue;
		}

		// new data has arrived, reprogram the timer
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			data = (struct timer_data *)buff;
			OCR0A = data->ocr;
			TCCR0B &= 0xf8;
			TCCR0B |= (data->prescaler & 0x07);
			g_st.max = data->st_max;
		}

		// ... and send the acknowledgement
		resp = (struct response *)buff;
		resp->status = ACK;
		slip_prepend_crc16(buff, sizeof(struct response));
		slip_send(buff, sizeof(struct response));
	}

	return 0;
}

