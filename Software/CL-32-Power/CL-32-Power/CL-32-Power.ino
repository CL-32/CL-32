
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <Wire.h>

const byte RED_LED = 0;
const byte GRN_LED = 1;
const byte ON_OFF = 2;
const byte V_BATT = 3;
const byte CHARGE = 4;
const byte USB_IN = 5;
const byte I2C_INT = 9;
const byte EN_3V3 = 8;
const byte I2C_SCL = 7;
const byte I2C_SDA = 6;
#define TIMEBASE_VALUE					(uint8_t)ceil(F_CPU*0.000001)
#define I2C_SLAVE_ADDRESS 0x08//apparently this is not reserved or popular!!

enum reason {BUTTON,I2C,USB};
reason wakeReason;
bool isOn;
long loopCount, buttonCount;
byte CURRENT_LED;


volatile uint8_t i2c_regs[] =
{
    0x00, //voltage divided by 23
    0x00, //request shutdown if > 0
    0x00, //power led brightness control
    0x00, //interrupt cause button/usb/charging
    0x00, //button/usb/charg status
    0x00, //spare
    0x00, //spare
    0x00, //spare
    0x00, //spare
    0x00, //spare
    0x00, //spare
    0x00, //spare
    0x00, //spare
    0x00, //spare
};
// Tracks the current register pointer position
volatile byte reg_position;

 
void setup() {
  sei();           
  //set the pins as we need them
  pinMode(RED_LED, OUTPUT);
  pinMode(GRN_LED, OUTPUT);
  pinMode(ON_OFF, INPUT_PULLUP);
  pinMode(EN_3V3, OUTPUT);
  pinMode(I2C_INT, INPUT_PULLUP);
  pinMode(USB_IN, INPUT_PULLUP);
  //init the i2c
  Wire.onReceive(receiveEvent); 
  Wire.onRequest(requestEvent);
  Wire.begin(I2C_SLAVE_ADDRESS); 
  //default some data
  i2c_regs[1]=0;//default disable shutdown
  i2c_regs[2]=255;//default brightness
  i2c_regs[3]=0;//default nothing needs interrupt
  sleep();
}

void loop() {
  if(loopCount==500000){
      i2c_regs[0] = trunc(readVoltage()/25);//if the voltage is in mv we can cover a range to 5.86v in 23 mv steps
      loopCount=0;
      //check for low voltage
      if(i2c_regs[0]<132){//3300 mv
        i2c_regs[3] = bitSet(i2c_regs[3],3);
        pinMode(I2C_INT, OUTPUT);
        digitalWrite(I2C_INT,LOW);
      }
  }
  //refresh the LED
  analogWrite(CURRENT_LED, i2c_regs[2]);
  if(!digitalRead(ON_OFF)){
    buttonCount++;
  }
  else{
    buttonCount = 0;
  }
  if(buttonCount>500000){
    digitalWrite(CURRENT_LED,LOW);
    while(digitalRead(ON_OFF)==LOW){
      //just do nothing!!  
    }   
    sleep();
  }
  //check for sleep request
  if(i2c_regs[1]>0){
    i2c_regs[1]=0;
    delay(200);
    sleep();
  }
  loopCount++;
}


void requestEvent(){ 
  Wire.write(i2c_regs[reg_position]);
  // Increment the reg position on each read, and loop back to zero
  reg_position++;
  if (reg_position > sizeof(i2c_regs))
    reg_position = 0;
}

void receiveEvent(uint8_t dataCount){
  // Sanity-check
  if (dataCount < 1) {
    // If too small
    return;
  }
  if (dataCount > 10) {
    // If larger than our buffer
    return;
  }
  
  // Set reg position
  reg_position = Wire.read();
  
  // Reduce count because we read position
  dataCount--;
  
  // If this was a set position only write
  if (!dataCount) {
    return;
  }
  
  // For each bit of data, add to registers
  while(dataCount--) {
    // Load new data into reg
    i2c_regs[reg_position] = Wire.read();
    
    // Increase reg position for next write
    reg_position++;
    if (reg_position > sizeof(i2c_regs))
      reg_position = 0;
  }
  //check for interrupt fire
  if(i2c_regs[3]==0){//nothing is wrong, no interrupt needed
    pinMode(I2C_INT, INPUT_PULLUP);
  }
}

uint16_t readVoltage() {
	uint32_t res;
 
	ADC0.CTRLA = ADC_ENABLE_bm;
	//ADC0.CTRLB = ADC_PRESC_DIV2_gc; // DEFAULT setting, no need to change
	ADC0.CTRLC = VREF_AC0REFSEL_1V024_gc | (TIMEBASE_VALUE << ADC_TIMEBASE_0_bp); // Vref = 1.024V
	ADC0.CTRLE = 150; // sample duration ((100 * 2) / F_CPU seconds), 1 MHz = 0.2ms
	ADC0.MUXPOS = ADC_MUXPOS_DACREF0_gc ; // using DAC as MUX voltage, ADC_MUXPOS_VDDDIV10_gc doesn't work
	ADC0.COMMAND = ADC_MODE_SINGLE_12BIT_gc; // single mode with 12 bit
	ADC0.COMMAND |= ADC_START_IMMEDIATE_gc; // start conversion
 
	while(true) {
		if(ADC0.INTFLAGS & ADC_RESRDY_bm) { // wait until measurement done
			res = (uint32_t) ADC0.RESULT; // get raw adc result
			if(res == 0) { res = 1; }
			res = (4096UL * 1024UL) / res; // convert result to mV
			break;
		}
	}
	ADC0.CTRLA = 0; // disable ADC
  if((uint16_t) res>3700){
    CURRENT_LED = GRN_LED;
    digitalWrite(RED_LED, LOW);  
  }
  else {
    CURRENT_LED = RED_LED;
    digitalWrite(GRN_LED, LOW);
  }	
  return ((uint16_t) res);
}

void sleep() {
    detachInterrupt(ON_OFF);
    detachInterrupt(USB_IN);
    detachInterrupt(CHARGE);
    digitalWrite(EN_3V3, LOW);    
    digitalWrite(RED_LED, LOW);  
    digitalWrite(GRN_LED, LOW);
    isOn = false;  
    //interrupts
    attachInterrupt(ON_OFF, sleepButton, LOW);
    attachInterrupt(USB_IN, sleepUSB, HIGH);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    
    sleep_enable();             
    sleep_cpu();                          
    sleep_disable(); 
    detachInterrupt(ON_OFF);
    detachInterrupt(USB_IN);
    //reset some values
    i2c_regs[0] = trunc(readVoltage()/23);
    //lets wait for debouncing
    for(int i=0;i<5;i++){
      //if(i%2==0){
      for(int j=0;j<255;j++){
        analogWrite(CURRENT_LED, j);
        delay(1);
      }
      for(int j=255;j>0;j--){
        analogWrite(CURRENT_LED, j);
        delay(1);
      }
    }
    //reset the brightness
    analogWrite(CURRENT_LED, i2c_regs[2]);
    //see if the button is still down
    if(digitalRead(ON_OFF)!=LOW){
      //the button is not still down, lets sleep again
      sleep();
    } 
    else{
      //button was down. lets wait a moment so the button gets released
      digitalWrite(EN_3V3, HIGH);
      isOn = true;  
      i2c_regs[3]=0;//default nothing needs interrupt
      //just add a quick check for button still being down
      while(digitalRead(ON_OFF)==LOW){
        //just do nothing!!  
      }   
      buttonCount = 0;   
      attachInterrupt(ON_OFF, onButton, CHANGE);
      attachInterrupt(USB_IN, onUSB, CHANGE);
      attachInterrupt(CHARGE, onCharge, CHANGE);
    }
} // sleep

void sleepButton(){
  wakeReason = BUTTON;
}

void sleepUSB(){
  wakeReason = USB;
}
void sleepI2C(){
  wakeReason = I2C;
}

void onButton(){
  bitWrite(i2c_regs[4],0,digitalRead(ON_OFF)==HIGH);
  if(bitRead(i2c_regs[4],0)){
    i2c_regs[3] = bitSet(i2c_regs[3],0);
    pinMode(I2C_INT, OUTPUT);
    digitalWrite(I2C_INT,LOW);
  }

}
void onUSB(){
  bitWrite(i2c_regs[4],0,digitalRead(USB_IN)==HIGH);
  i2c_regs[3] = bitSet(i2c_regs[3],1);
  pinMode(I2C_INT, OUTPUT);
  digitalWrite(I2C_INT,LOW);
}
void onCharge(){
  bitWrite(i2c_regs[4],0,digitalRead(CHARGE)==HIGH);
  i2c_regs[3] = bitSet(i2c_regs[3],2);
  pinMode(I2C_INT, OUTPUT);
  digitalWrite(I2C_INT,LOW);
}