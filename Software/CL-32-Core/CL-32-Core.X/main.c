#define F_CPU 3333333UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "include/led.h"
#include "include/i2c.h"
#include "include/keyb.h"
#include "include/power.h"
#include "include/general.h"


void sleep() {
    esp_off();
    clear_pwm_interrupt();
    set_key_interrupt();
    set_usb_interrupt();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
    sleep_enable();             
    sleep_cpu();
    sleep_disable(); 
    clear_key_interrupt();
    clear_usb_interrupt();
    set_pwm_interrupt();
  //reset some values
    read_voltage();
    //we need to see what triggered us to wake up
    if(wakeReason==USB){
        set_pwm_interrupt();
        currentLED = GRN;
        for(uint8_t j=0;j<255;j++){
          set_brightness(POWER, j);
          _delay_ms(3);
        }
        for(uint8_t j=255;j>0;j--){
          set_brightness(POWER, j);
          _delay_ms(3);
        }
        clear_pwm_interrupt();
        read_usb();
         read_keys();//just to reset the status
    }
    else if(wakeReason==BUTTON){
      //lets wait for debouncing
      set_pwm_interrupt();
      for(int i=0;i<5;i++){
        //if(i%2==0){
        for(uint8_t j=0;j<255;j++){
          set_brightness(POWER, j);
          _delay_us(750);
        }
        for(uint8_t j=255;j>0;j--){
          set_brightness(POWER, j);
          _delay_us(750);
        }
      }
      clear_pwm_interrupt();
      //reset the brightness
      //see if the button is still down
      if(read_key(1,4)==1){
        //the button is not still down, lets sleep again
        sleep();
      } 
      else{
        set_brightness(BACKLIGHT,i2c_regs[CL32_POWER_BRIGHT]);
        //just add a quick check for button still being down
        while(read_key(1,4)==0){
          //sit and wait
        }
        i2c_regs[CL32_INTERRUPT]=0;//default nothing needs interrupt
        i2c_regs[CL32_STATUS]=0;//Reset the status
        read_usb();
        read_keys();//just to reset the status
        clear_i2c_flag();//make sure things are clear
        esp_on();
        BLcount = 0;
      }
      buttonCount = 0;   
    }
} // sleep


int main(void)
{    
    led_init();//start the PWM
    keyb_init();//start the keyboard
    i2c_slave_init(0x08);//start the i2c
    power_init();//start the power
    sei();
    sleep();
    while(1){        
        if(is_on()){
            if(loopCount==500000){
                read_voltage();
                loopCount=0;
                //check for low voltage
//                if(i2c_regs[CL32_VOLTAGE]<132){//3300 mv
//                    bitWrite(i2c_regs[CL32_INTERRUPT],3,true);
//                }
            }
            //refresh the LED
            set_brightness(POWER,i2c_regs[CL32_POWER_BRIGHT]);
            //check for sleep request
            if(bitRead(i2c_regs[CL32_STATUS],7)){
                bitWrite(i2c_regs[CL32_STATUS],7,false);
                _delay_ms(200);
                set_brightness(BACKLIGHT,0);
                sleep();
            }
            //backlight shenanigans
            int iBLtime = i2c_regs[CL32_BL_TIME]*50;
            if (BLcount > iBLtime){
                set_brightness(BACKLIGHT,0);
            }
            else{
                int iFade;
                if(BLcount > (iBLtime - (i2c_regs[CL32_BL_BRIGHT]*5))){
                    iFade = trunc(((i2c_regs[CL32_BL_BRIGHT]*5)-(iBLtime-BLcount))/5);
                }
                else{
                    iFade = 0;
                }

                set_brightness(BACKLIGHT,i2c_regs[CL32_BL_BRIGHT]-iFade);
                BLcount++;
            }
            //check for interrupt fire
            //if (loopCount%500 == 0){
                check_keys();
                check_usb();
            //}
        }
        else{
            if(loopCount==100000){
                if(bitRead(i2c_regs[CL32_STATUS],1)==0){
                    //charging done or usb removed
                    sleep();
                }
                if(bitRead(i2c_regs[CL32_STATUS],2)==0){
                    //charging done
                    currentLED = GRN;
                    set_brightness(POWER, 125);
                }
                else{
                    currentLED = GRN;
                    set_brightness(POWER, 125);
                    _delay_ms(200);
                    set_brightness(POWER, 0);
                    _delay_ms(200);
                    set_brightness(POWER, 125);
                    _delay_ms(200);
                    set_brightness(POWER, 0);
                }
                loopCount=0;
                read_usb();
            }
            //check for the power button
            if(read_key(1,4)==0){
                //the button is down, lets flash some stuff
                set_pwm_interrupt();
                for(int i=0;i<5;i++){
                  //if(i%2==0){
                  for(uint8_t j=0;j<255;j++){
                    set_brightness(POWER, j);
                    _delay_ms(1);
                  }
                  for(uint8_t j=255;j>0;j--){
                    set_brightness(POWER, j);
                    _delay_ms(1);
                  }
                }
                clear_pwm_interrupt();
                //reset the brightness
                //see if the button is still down
                if(read_key(1,4)==0){
                  set_brightness(BACKLIGHT,i2c_regs[CL32_POWER_BRIGHT]);
                  i2c_regs[CL32_INTERRUPT]=0;//default nothing needs interrupt
                  i2c_regs[CL32_STATUS]=0;//Reset the status
                  read_usb();
                  read_keys();//just to reset the status
                  //just add a quick check for button still being down
                  while(read_key(1,4)==0){
                    //sit and wait
                  }
                  esp_on();
                  BLcount = 0;
                }
            }
        }
        loopCount++;
    }    
}