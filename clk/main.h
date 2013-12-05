#ifndef MAIN_H_6OSZU1UF
#define MAIN_H_6OSZU1UF

// size of the data exchange buffer
#define XBUFF_SIZE 16

// I2C address of the TWI device
#define TWI_RTC_ADDR 0x68

// one wire thermometer setup
#define OW_THERMO_OUTP PORTB
#define OW_THERMO_PIN 0

// lcd io lines setup
#define LCD_DB_PORT PORTD
#define LCD_DB_PIN_FIRST 4
#define LCD_RS_PORT PORTB
#define LCD_RS_PIN 1
#define LCD_E_PORT PORTB
#define LCD_E_PIN 2

extern volatile struct sys_ctx g_sys_ctx;

void main(void) __attribute__ ((noreturn));

#endif /* end of include guard: MAIN_H_6OSZU1UF */

