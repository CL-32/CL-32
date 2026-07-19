#ifndef POWER_H
#define	POWER_H
#include <avr/io.h>
#include <avr/interrupt.h>

#define P_OK_GetValue() (((PORTA.IN & PIN0_bm) != 0))
#define CHG_GetValue()  (((PORTA.IN & PIN1_bm) != 0))


void power_init(void);
void esp_on(void);
void esp_off(void);
uint8_t is_on(void);
void set_usb_interrupt(void);
void clear_usb_interrupt(void);
void read_voltage(void);
void check_usb(void);
void read_usb(void);

#endif	/* XC_HEADER_TEMPLATE_H */

