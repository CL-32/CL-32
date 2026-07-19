#ifndef LED_H
#define LED_H
#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile uint8_t pwm_counter;
extern volatile uint8_t PWM_CUR;


#define RED_LED_SetHigh() do { PORTA_OUTSET = 0x4; } while(0)
#define RED_LED_SetLow() do { PORTA_OUTCLR = 0x4; } while(0)
#define RED_LED_Toggle() do { PORTA_OUTTGL = 0x4; } while(0)

#define GRN_LED_SetHigh() do { PORTA_OUTSET = 0x8; } while(0)
#define GRN_LED_SetLow() do { PORTA_OUTCLR = 0x8; } while(0)
#define GRN_LED_Toggle() do { PORTA_OUTTGL = 0x8; } while(0)

typedef enum{
    POWER,
    BACKLIGHT
} led_t;
extern volatile led_t whichLED;

typedef enum{
    OFF,
    ON
} pwm_t;
extern volatile pwm_t pwmLED;


void led_init(void);//start the timer/led stuff
void set_pwm_interrupt(void);
void clear_pwm_interrupt(void);
void set_brightness(led_t choice, uint8_t bright);

#endif /* LED_H_INCLUDED */