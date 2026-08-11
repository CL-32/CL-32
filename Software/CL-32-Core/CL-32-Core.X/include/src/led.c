#include "../led.h"
#include "../general.h"
#include "../i2c.h"
#define F_CPU 3333333UL
#include <util/delay.h>

volatile uint8_t pwm_counter = 0;
volatile uint8_t PWM_CUR = 0;

volatile led_t whichLED = BACKLIGHT;
volatile pwm_t pwmLED = OFF;

void led_init(void){
    //set the pins to output mode
    PORTA_DIRSET = 0x4;//RED LED output mode
    PORTA_DIRSET = 0x8;//GRN LED output mode
    PORTD_DIRSET = 0x1;//BL1 LED output mode
    PORTD_DIRSET = 0x2;//BL2 LED output mode
    //set PWM timer settings
    TCB0.CCMP = 150;// Period value 100 will drag it to the floor, 500 is like a rave
    TCB0.CTRLB = TCB_CNTMODE_INT_gc;  // Periodic interrupt mode
    TCB0.INTCTRL = TCB_CAPT_bm;// Enable interrupt
    set_pwm_interrupt();
    
    int j;
    currentLED = RED;
    for(j=0;j<255;j++){
          set_brightness(POWER, j);
      _delay_ms(1);
    }
    for(j=255;j>=0;j--){
          set_brightness(POWER, j);
      _delay_ms(1);
    }
    currentLED = GRN;
    for(j=0;j<255;j++){
          set_brightness(POWER, j);
      _delay_ms(1);
    }
    for(j=255;j>=0;j--){
          set_brightness(POWER, j);
      _delay_ms(1);
    }  
    //set hardware PWM  
    PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTD_gc;   // route TCA outputs to Port D
    TCA0.SINGLE.PER = 255;
    TCA0.SINGLE.CMP0 = 0;//default to off
    TCA0.SINGLE.CMP1 = 0;//default to off

    TCA0.SINGLE.CTRLB =
        TCA_SINGLE_CMP0EN_bm |
        TCA_SINGLE_CMP1EN_bm |
        TCA_SINGLE_WGMODE_SINGLESLOPE_gc;

    TCA0.SINGLE.CTRLA =
    TCA_SINGLE_ENABLE_bm;
    
    for(j=0;j<255;j++){
      TCA0.SINGLE.CMP0 = j;  // PA3
      _delay_ms(1);
    }
    for(j=255;j>=0;j--){
      TCA0.SINGLE.CMP0 = j;  // PA3
      _delay_ms(1);
    }  
    for(j=0;j<255;j++){
      TCA0.SINGLE.CMP1 = j;  // PA3
      _delay_ms(1);
    }
    for(j=255;j>=0;j--){
      TCA0.SINGLE.CMP1 = j;  // PA3
      _delay_ms(1);
    }  
    //set some LED defaults
    i2c_regs[CL32_POWER_BRIGHT]=127;//power LED brightness
    i2c_regs[CL32_BL_BRIGHT]=50;//backlight brightness
    i2c_regs[CL32_BL_TIME]=100;//backlight duration  
}

void set_pwm_interrupt(void){
    TCB0.CTRLA = TCB_ENABLE_bm;// Clock /1 and enable
    //reset everything to off
    PWM_CUR = 0;
    TCA0.SINGLE.CMP0 = 0;
    TCA0.SINGLE.CMP1 = 0;
    pwmLED = ON;
}

void clear_pwm_interrupt(void){
    TCB0.CTRLA = 0;// disable
    //turn off the leds
    RED_LED_SetLow();
    GRN_LED_SetLow();
    pwmLED = OFF;
    //make sure the leds are off too
    TCA0.SINGLE.CMP0 = 0;
    TCA0.SINGLE.CMP1 = 0;
    PWM_CUR = 0;
}

void set_brightness(led_t choice, uint8_t bright){
    if(choice==POWER){
        if(pwmLED == ON){
            PWM_CUR = bright;
        }
        else{ //if pwm is off, we just set the led on, no brightness control  
            if(currentLED == RED){
                if (bright > 0)
                    RED_LED_SetHigh();
                else
                    RED_LED_SetLow();
            }
            else{
                if (bright > 0)
                    GRN_LED_SetHigh();
                else
                    GRN_LED_SetLow();
            }
        }
    }
    else{//backlight
        TCA0.SINGLE.CMP0 = bright;
        TCA0.SINGLE.CMP1 = bright;
    }   
}

ISR(TCB0_INT_vect)
{
    pwm_counter++;
    if(currentLED == RED){
        if (pwm_counter < PWM_CUR)
            RED_LED_SetHigh();
        else
            RED_LED_SetLow();
    }
    else{
        if (pwm_counter < PWM_CUR)
            GRN_LED_SetHigh();
        else
            GRN_LED_SetLow();
    }

    TCB0.INTFLAGS = TCB_CAPT_bm;
}
