#include "../power.h"
#include "../general.h"
#include "../i2c.h"

ISR(PORTA_PORT_vect){
    // Did PD4 trigger?
    if (PORTA.INTFLAGS & PIN0_bm){
        // clear interrupt flag
        PORTA.INTFLAGS = PIN0_bm;
        wakeReason = USB;
    }
}

void power_init(void){
    //set p-ok as an input with pull-up
    PORTA.DIRCLR = PIN0_bm;
    PORTA_PIN0CTRL = PORT_PULLUPEN_bm | PORT_INVEN_bm;
    //set charging as an input with pull-up
    PORTA.DIRCLR = PIN1_bm;
    PORTA_PIN1CTRL = PORT_PULLUPEN_bm | PORT_INVEN_bm;//inverted too
    //set 3v3 pin as an output
    PORTC.DIRSET = PIN0_bm;
    read_usb();
}

void esp_on(void){
    //set it high
    PORTC_OUTSET = PIN0_bm;
}

void esp_off(void){
    //set it low
    PORTC_OUTCLR = PIN0_bm;
}

uint8_t is_on(void){
    return VPORTC.IN & (PIN0_bm << 0);
}

void set_usb_interrupt(void){
    PORTA.PIN0CTRL = (PORTA.PIN0CTRL & ~PORT_ISC_gm) | PORT_ISC_RISING_gc ;
}

void clear_usb_interrupt(void){
    PORTA.PIN0CTRL = (PORTA.PIN0CTRL & ~PORT_ISC_gm) | PORT_ISC_INTDISABLE_gc ;
}

void read_voltage(void){
    volatile  uint16_t vSupply, vBatt;
    volatile  uint32_t adc;
    ADC0.CTRLA = ADC_ENABLE_bm; 
    ADC0.CTRLB = ADC_PRESC_DIV16_gc; // ADC clock 
    ADC0.CTRLC = ADC_REFSEL_1V024_gc; // Reference 
    ADC0.CTRLE = 64; // Sample duration 
    ADC0.CTRLF = 0; // No accumulation, right adjust 
    ADC0.MUXPOS = ADC_MUXPOS_VDD10_gc;//read vcc/10 
    ADC0.MUXNEG = ADC_MUXNEG_GND_gc;//make sure the negative it right 
    ADC0.INTFLAGS = ADC_RESRDY_bm;//reset the adc flags 
    ADC0.COMMAND = ADC_MODE_SINGLE_12BIT_gc | ADC_START_IMMEDIATE_gc;//request the adc 
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));//wait for the result // discard first sample 
    ADC0.INTFLAGS = ADC_RESRDY_bm;//reset the adc flags 
    ADC0.COMMAND = ADC_MODE_SINGLE_12BIT_gc | ADC_START_IMMEDIATE_gc;//request the adc 
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));//wait for the result //grab the value 
    adc = ADC0.RESULT;//use the result 
    vSupply = ((1024 * adc) / 4095)* 10; 
    ADC0.INTFLAGS = ADC_RESRDY_bm;//reset the adc flags
    //now read the battery, and compare it against the supply
    ADC0.CTRLC = ADC_REFSEL_VDD_gc;//supply as reference
    ADC0.MUXPOS = ADC_MUXPOS_AIN21_gc;//PF5 - AIN21
    ADC0.COMMAND = ADC_MODE_SINGLE_12BIT_gc | ADC_START_IMMEDIATE_gc;//request the adc
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));//wait for the result
    ADC0.INTFLAGS = ADC_RESRDY_bm;//reset the adc flags
    // discard first sample
    ADC0.COMMAND = ADC_MODE_SINGLE_12BIT_gc | ADC_START_IMMEDIATE_gc;//request the adc
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));//wait for the result
    //grab the value
    adc = ADC0.RESULT;//use the result
    ADC0.INTFLAGS = ADC_RESRDY_bm;//reset the adc flags
    vBatt = vSupply * adc / 4095;//1023 makes vBatt = vSupply
    i2c_regs[CL32_SUPPLY] = vSupply / 25;
    i2c_regs[CL32_VOLTAGE] = vBatt / 25;
    ADC0.CTRLA = 0;//disable the adc
    VREF.DAC0REF = 0;//disable the dac;
    if (vBatt > 3700){
        currentLED = GRN;
    }
    else{
        currentLED = RED;
    }
}

void check_usb(void){
    uint8_t bChange = 0;
    bool p_ok = P_OK_GetValue();
    bool chg  = CHG_GetValue();

    bool old_p_ok = bitRead(i2c_regs[CL32_STATUS], 1);
    bool old_chg  = bitRead(i2c_regs[CL32_STATUS], 2);

    if (p_ok != old_p_ok)
    {
        bitWrite(i2c_regs[CL32_STATUS], 1, p_ok);
        bitWrite(i2c_regs[CL32_INTERRUPT], 1, 1);
        bChange++;
    }

    if (chg != old_chg)
    {
        bitWrite(i2c_regs[CL32_STATUS], 2, chg);
        bitWrite(i2c_regs[CL32_INTERRUPT], 2, 1);
        bChange++;
    }
    if(bChange>0){
        BLcount = 0;
        set_i2c_flag();
        read_voltage();
    }
    
}


void read_usb(void){    
    bitWrite(i2c_regs[CL32_STATUS],1,P_OK_GetValue());
    bitWrite(i2c_regs[CL32_STATUS],2,CHG_GetValue());
}
