#include <Arduino.h>
#include <Wire.h>
#include "CL32.h"
#include "CL32_test.h"

char theMessage[30];
bool turnOff = false;

CL32_test::CL32_test() {
    //start the i2c
    Wire.begin(CL32_sda,CL32_scl);
}

void CL32_test::init(){
}

void CL32_test::read(){
    byte bStatus;
    Wire.beginTransmission(TEST_ADDRESS);
    Wire.write(0x03);//0x03 is the interrupt register
    Wire.endTransmission();

    Wire.requestFrom(TEST_ADDRESS,1);
    while(Wire.available() < 1);//sit and wait for response
    //read the value
    bStatus = Wire.read();
    if (bStatus!=0){
        if(bitRead(bStatus,0)){
            turnOff = false;
            sprintf(theMessage,"Shutdown Your CL-32?");
            _keys.add_callback(prompt_keys);
            _screen.yesNoDialog(theMessage,turnOff);
        }
        if(bitRead(bStatus,1)){
            _screen.showMsg("USB Change");
            delay(50);
        }
        if(bitRead(bStatus,2)){
            _screen.showMsg("Charge Change");
            delay(50);
        }
        if(bitRead(bStatus,3)){
            turnOff = false;
            sprintf(theMessage,"Battery Low, Shutdown?");
            _keys.add_callback(prompt_keys);
            _screen.yesNoDialog(theMessage,turnOff);
        }
    }
    Wire.beginTransmission(TEST_ADDRESS);
    Wire.write(0x03);//0x03 is the interrupt register
    Wire.write(0x00);//setting a bit to 0 clears its interrupt 
    Wire.endTransmission();
}

void prompt_keys(){
    for(byte i = _keys.eventCount();i>0;i--){
        Event eTemp = _keys.getKey();
        if(eTemp.keyDown && !eTemp.isChar && (eTemp.keyData==KB_LEFT || eTemp.keyData==KB_RGHT)){
            turnOff = !turnOff;
            _screen.yesNoDialog(theMessage,turnOff);
        }
        if(eTemp.keyDown && !eTemp.isChar && eTemp.keyData==KB_RET){
            if(turnOff){
                _screen.drawOff();
                Wire.beginTransmission(TEST_ADDRESS);
                Wire.write(0x01);//0x01 is the shutdown register
                Wire.write(0xff);//setting the register over 0 requests shutown
                Wire.endTransmission();
            } 
            else{
                _keys.last_callback();
                _keys.trigger_callback();///just force it to think something has happened so the screen gets refreshed again
            }   
        }
        if(eTemp.keyDown && !eTemp.isChar && eTemp.keyData==KB_ESC){
            _keys.last_callback();
            _keys.trigger_callback();///just force it to think something has happened so the screen gets refreshed again
        }
    }
}