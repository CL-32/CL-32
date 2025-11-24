#include <Arduino.h>
#include <Wire.h>
#include "CL32.h"
#include "CL32_power.h"


CL32_power::CL32_power() {
    //start the i2c
    Wire.begin(CL32_sda,CL32_scl);
}

void CL32_power::init(){
    //set some defaults for the power chip
    // Wire.beginTransmission(BATTERY_ADDRESS);
    // Wire.write(0xc0);//setting for backup fallover
    // Wire.write(0x20);// not used 0 - clock out 0 - backup switchover 10 - charger resistor (not needed) 00 - charger off 00
    // Wire.endTransmission();
    loadPower();
}

void CL32_power::loadPower(){
    byte bData;
    //get the voltage
    Wire.beginTransmission(BATTERY_ADDRESS);
    Wire.write(0x02);//0x02 is where we start
    Wire.endTransmission();
    Wire.requestFrom(BATTERY_ADDRESS,2);
    while(Wire.available() < 1);//sit and wait for response
    _CL32voltage = Wire.read();
    _CL32voltage <<= 8;
    _CL32voltage |= Wire.read();
    _CL32voltage = _CL32voltage * 78.125 / 1000;
    
    //get the percentage
    Wire.beginTransmission(BATTERY_ADDRESS);
    Wire.write(0x04);//0x02 is SOC
    Wire.endTransmission();
    Wire.requestFrom(BATTERY_ADDRESS,2);
    while(Wire.available() < 1);//sit and wait for response
    _CL32percent = Wire.read();
    _CL32percent <<= 8;
    _CL32percent |= Wire.read();
    _CL32percent = _CL32percent / 256;
    
    //get the load
    Wire.beginTransmission(BATTERY_ADDRESS);
    Wire.write(0x16);//0x16 is C rate
    Wire.endTransmission();
    Wire.requestFrom(BATTERY_ADDRESS,2);
    while(Wire.available() < 1);//sit and wait for response
    _CL32load = Wire.read();
    _CL32load <<= 8;
    _CL32load |= Wire.read();
    _CL32load = _CL32load * 0.208;
}

int CL32_power::getVoltage(){
    return _CL32voltage;
}

int CL32_power::getPercent(){
    return _CL32percent;
}

int CL32_power::getLoad(){
    return _CL32load;
}