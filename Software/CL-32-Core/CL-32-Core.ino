
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <Wire.h>
#include "keymap.h"

const byte RED_LED = 2;
const byte GRN_LED = 3;
const byte BL1_LED = 12;
const byte BL2_LED = 13;
const byte VBAT = 25;
const byte CHG = 1;
const byte P_OK = 0;
const byte EN_3V3 = 8;
const byte I2C_INT = 9;
const byte I2C_SCL = 11;
const byte I2C_SDA = 10;
const byte ROW[8] = {26,24,23,22,21,20,19,18};
const byte COL[8] = {17,16,15,14, 7, 6, 5, 4};
#define I2C_SLAVE_ADDRESS 0x08//apparently this is not reserved or popular!!

enum reason {BUTTON,I2C,USB};
reason wakeReason;
bool isOn, startBuffer;
long loopCount, buttonCount, BLcount;
byte CURRENT_LED;


volatile uint8_t i2c_regs[] =
{
    0x00, //voltage divided by 25
    0x00, //system status | 0 power on | 1 USB | 2 charge | 7 request off
    0x00, //interrupt status | 0 keyboard | 1 USB | 2 charge | 3 battery
    0x00, //event count
    0x00, //event data 1
    0x00, //event data 2
    0x00, //event data 3
    0x00, //event data 4
    0x00, //event data 5
    0x00, //event data 6
    0x00, //event data 7
    0x00, //event data 8
    0x00, //event data 9
    0x00, //event data 10
    0x00, //power LED brightness
    0x00, //backlight brightness
    0x00, //backlight timeout
    0x00, //raw row0 data
    0x00, //raw row1 data
    0x00, //raw row2 data
    0x00, //raw row3 data
    0x00, //raw row4 data
    0x00, //raw row5 data
    0x00, //raw row6 data
    0x00, //raw row7 data
    0x00, //spare
    0x00, //spare
    0x00, //spare
    0x00, //spare
    0x00, //spare
};
// Tracks the current register pointer position
volatile byte reg_position;

 
void setup() {
  int i, j;
  sei();        
  analogReference(INTERNAL1V5);   
  //set the pins as we need them
  pinMode(RED_LED, OUTPUT);
  pinMode(GRN_LED, OUTPUT);
  pinMode(BL1_LED, OUTPUT);
  pinMode(BL2_LED, OUTPUT);
  pinMode(EN_3V3, OUTPUT);
  //set row/col pin modes
  for(i=0;i<8;i++){
    pinMode(ROW[i], OUTPUT);
    digitalWrite(ROW[i], HIGH);
    pinMode(COL[i], INPUT_PULLUP);
  }
  pinMode(I2C_INT, INPUT);
  pinMode(CHG, INPUT_PULLUP);
  pinMode(P_OK, INPUT_PULLUP);
  //flash some led's to show life
  for(j=0;j<255;j++){
    analogWrite(RED_LED, j);
    delay(1);
  }
  for(j=255;j>=0;j--){
    analogWrite(RED_LED, j);
    delay(1);
  }
  for(j=0;j<255;j++){
    analogWrite(GRN_LED, j);
    delay(1);
  }
  for(j=255;j>=0;j--){
    analogWrite(GRN_LED, j);
    delay(1);
  }
  for(j=0;j<255;j++){
    analogWrite(BL1_LED, j);
    delay(1);
  }
  for(j=255;j>=0;j--){
    analogWrite(BL1_LED, j);
    delay(1);
  }
  for(j=0;j<255;j++){
    analogWrite(BL2_LED, j);
    delay(1);
  }
  for(j=255;j>=0;j--){
    analogWrite(BL2_LED, j);
    delay(1);
  }
  //init the i2c
  reg_position = 0;
  Wire.swap(1);
  Wire.onReceive(receiveEvent); 
  Wire.onRequest(requestEvent);
  Wire.begin(I2C_SLAVE_ADDRESS); 
  //default some data
  i2c_regs[1]=0;//clear system status
  i2c_regs[2]=0;//clear interrupt status
  i2c_regs[3]=0;//no events
  i2c_regs[14]=127;//power LED brightness
  i2c_regs[15]=50;//backlight brightness
  i2c_regs[16]=100;//backlight duration  
  sleep();
}

void loop() {
  if(loopCount==500000){
      i2c_regs[0] = trunc(readVoltage()/25);//if the voltage is in mv we can cover a range to 5.86v in 25 mv steps
      loopCount=0;
      //check for low voltage
      if(i2c_regs[0]<132){//3300 mv
        i2c_regs[2] = bitSet(i2c_regs[2],3);
        pinMode(I2C_INT, OUTPUT);
        digitalWrite(I2C_INT,LOW);
      }
  }
  //refresh the LED
  analogWrite(CURRENT_LED, i2c_regs[14]);
  //check for sleep request
  if(bitRead(i2c_regs[1],7)){
    bitWrite(i2c_regs[1],7,false);
    delay(200);
    sleep();
  }
  //backlight shenanigans
  int iBLtime = i2c_regs[16]*50;
  if (BLcount > iBLtime){
    analogWrite(BL1_LED, 0);
    analogWrite(BL2_LED, 0);
  }
  else{
    int iFade;
    if(BLcount > (iBLtime - (i2c_regs[15]*5))){
      iFade = trunc(((i2c_regs[15]*5)-(iBLtime-BLcount))/5);
    }
    else{
      iFade = 0;
    }

    analogWrite(BL1_LED, i2c_regs[15]-iFade);
    analogWrite(BL2_LED, i2c_regs[15]-iFade);
    BLcount++;
  }
  readKeys();
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
  if (dataCount > sizeof(i2c_regs)) {
    // If larger than our buffer
    return;
  }
  
  // Set reg position
  uint8_t rTemp = Wire.read();//read the value
  if(startBuffer){//the last pos requested was the start of the buffer
    trimBuffer(reg_position-4);
    //if we have gone to 4, then read one value, position should be 5, we thus need to trim 1
  }
  reg_position = rTemp;//set the new pos
  startBuffer = (rTemp==4);//flag the start of buffer being requested or not
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
  if(i2c_regs[2]==0){//nothing is wrong, no interrupt needed
    pinMode(I2C_INT, INPUT);
  }
}

uint16_t readVoltage() {
  uint32_t v = analogRead(VBAT);
  v =  (4096UL * 1024UL) / v;
  if((uint16_t) v>3700){
    CURRENT_LED = GRN_LED;
    digitalWrite(RED_LED, LOW);  
  }
  else {
    CURRENT_LED = RED_LED;
    digitalWrite(GRN_LED, LOW);
  }	
  return ((uint16_t) v);
}

void sleep() {
  detachInterrupt(CHG);
  detachInterrupt(P_OK);
  digitalWrite(EN_3V3, LOW);    
  analogWrite(RED_LED, 0);  
  analogWrite(GRN_LED, 0);
  analogWrite(BL1_LED, 0);
  analogWrite(BL2_LED, 0);
  for(int i=0;i<8;i++){//set the rows all low
    digitalWrite(ROW[i], HIGH);
  }
  digitalWrite(ROW[4], LOW);
  isOn = false;  
  //interrupts
  attachInterrupt(COL[1], sleepButton, LOW);
  attachInterrupt(P_OK, sleepUSB, HIGH);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
  sleep_enable();             
  sleep_cpu();
  sleep_disable();             
  detachInterrupt(P_OK);
  detachInterrupt(COL[1]);
  //reset some values
  i2c_regs[0] = trunc(readVoltage()/23);
  //we need to see what triggered us to wake up
  if(wakeReason==USB){

  }
  else if(wakeReason==BUTTON){
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
    analogWrite(CURRENT_LED, i2c_regs[14]);
    //see if the button is still down
    if(digitalRead(COL[1])==HIGH){
      //the button is not still down, lets sleep again
      sleep();
    } 
    else{
      isOn = true;  
      i2c_regs[2]=0;//default nothing needs interrupt
      pinMode(I2C_INT, INPUT);
      // pinMode(I2C_INT, OUTPUT);
      // digitalWrite(I2C_INT,LOW);
      //just add a quick check for button still being down
      while(digitalRead(COL[1])==LOW){
        //sit and wait
      }
      buttonCount = 0;   
      digitalWrite(EN_3V3, HIGH);
      attachInterrupt(P_OK, onUSB, CHANGE);
      attachInterrupt(CHG, onCharge, CHANGE);
      BLcount = 0;
    }
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

void onUSB(){
  bitWrite(i2c_regs[2],1,digitalRead(P_OK)==HIGH);
  pinMode(I2C_INT, OUTPUT);
  digitalWrite(I2C_INT,LOW);
}
void onCharge(){
  bitWrite(i2c_regs[2],2,digitalRead(CHG)==HIGH);
  pinMode(I2C_INT, OUTPUT);
  digitalWrite(I2C_INT,LOW);
}

void readKeys(){
  byte r, c, changeCount;
  changeCount=0;
  for(r=0;r<8;r++){//set the rows all low
    digitalWrite(ROW[r], HIGH);
  }
  for(r=0;r<8;r++){
    digitalWrite(ROW[r], LOW);//set the pin high to start
    for(c=0;c<8;c++){
      if(bitRead(i2c_regs[r+17],c) != (digitalRead(COL[c])==LOW)){//state change
        //add event
        i2c_regs[4 + i2c_regs[3]] = keyVal[r][c];
        //set the status 
        bitWrite(i2c_regs[4 + i2c_regs[3]],7,digitalRead(COL[c])==LOW);
        //increment the count
        i2c_regs[3]++;
        if(i2c_regs[3]>9){
          i2c_regs[3]=9;
        }
        //log the change
        bitWrite(i2c_regs[r+17],c,(digitalRead(COL[c])==LOW));
        changeCount++;
      }
    }
    digitalWrite(ROW[r], HIGH);//set it low to tidy up
  }
  if(isOn&&changeCount>0){    
    bitWrite(i2c_regs[2],0,true);
    pinMode(I2C_INT, OUTPUT);
    digitalWrite(I2C_INT,LOW);
    BLcount = 0;
  }
}

void trimBuffer(byte howMuch){
  byte i, b;
  if(howMuch > 10){
    howMuch = 10;
  }
  for(i=howMuch;i>0;i--){//how many passes
    for(b=12;b>3;b--){//loop backwards down the buffer
      i2c_regs[b]=i2c_regs[b+1];//pop the data backwards
    }
  }
  //deduct the count
  i2c_regs[3]-=howMuch;
}