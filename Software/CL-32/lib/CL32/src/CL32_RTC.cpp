#include <Arduino.h>
#include <Wire.h>
#include "CL32_rtc.h"


CL32_rtc::CL32_rtc() {
    //start the i2c
    Wire.begin(CL32_sda,CL32_scl);
}

void CL32_rtc::init(){   
    //set the defaults
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0xc0);//setting for backup fallover
    Wire.write(0x20);// not used 0 - clock out 0 - backup switchover 10 - charger resistor (not needed) 00 - charger off 00
    Wire.endTransmission();
    loadTime();
}

void CL32_rtc::loadTime(){ 
    byte bData;
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x01);//0x01 is where we start
    Wire.endTransmission();
    //read the contents
    Wire.requestFrom(RTC_ADDRESS,7);
    while(Wire.available() < 1);//sit and wait for response
    //read the value
    bData = Wire.read();
    _CL32time.tm_sec = ((bData >> 4) * 10) + (bData & 0xf);
    bData = Wire.read();
    _CL32time.tm_min = ((bData >> 4) * 10) + (bData & 0xf);
    bData = Wire.read();
    _CL32time.tm_hour = ((bData >> 4) * 10) + (bData & 0xf);
    bData = Wire.read();
    _CL32time.tm_wday = bData - 1; // day of the week (0-6)
    // day of the month
    bData = Wire.read();
    _CL32time.tm_mday = ((bData >> 4) * 10) + (bData & 0xf);
    // month
    bData = Wire.read();
    _CL32time.tm_mon = (((bData >> 4) & 1) * 10 + (bData & 0xf)) -1; // 0-11 
    bData = Wire.read();    
    _CL32time.tm_year = 100 + ((bData >> 4) * 10) + (bData & 0xf);
    //update timetext variable
    sprintf(timeText, "%02d:%02d", _CL32time.tm_hour, _CL32time.tm_min);
}

void CL32_rtc::saveTime(struct tm timeIn){
    byte bData;
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x01);//0x01 is where we start
    //seconds
    bData = ((timeIn.tm_sec / 10) << 4);
    bData |= (timeIn.tm_sec % 10);
    Wire.write(bData);
    // minutes
    bData = ((timeIn.tm_min / 10) << 4);
    bData |= (timeIn.tm_min % 10);
    Wire.write(bData);
    // hours
    bData = ((timeIn.tm_hour / 10) << 4);
    bData |= (timeIn.tm_hour % 10);
    Wire.write(bData);
    // day of the week
    bData = timeIn.tm_wday + 1;
    Wire.write(bData);
    // day of the month
    bData = (timeIn.tm_mday / 10) << 4;
    bData |= (timeIn.tm_mday % 10);
    Wire.write(bData);
    // month
    uint8_t i;
    i = timeIn.tm_mon+1; // 1-12 on the RTC
    bData = (i / 10) << 4;
    bData |= (i % 10);
    Wire.write(bData);
    // year
    bData = (((timeIn.tm_year % 100)/10) << 4);
    bData |= (timeIn.tm_year % 10);
    Wire.write(bData);
    Wire.endTransmission();
}

void CL32_rtc::read(){
    //read/clear the interrupt
    byte bStatus;

    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x0d);//0x0d is the status register
    Wire.endTransmission();

    Wire.requestFrom(RTC_ADDRESS,1);
    while(Wire.available() < 1);//sit and wait for response
    //read the value
    bStatus = Wire.read();
    if (bStatus!=0){
        Serial.print("RTC Status ");
        Serial.print(bStatus,BIN);
        //10000000 - temp high flag
        //01000000 - temp low flag
        //00100000 - time update flag
        //00010000 - countdown timer flag
        //00001000 - alarm flag
        //00000100 - external event flag
        //00000010 - power on reset
        //00000001 - low voltage flag
        //maybe do some stuff here
        Wire.beginTransmission(RTC_ADDRESS);
        Wire.write(0x0d);//0x0d is the status register
        Wire.write(0xff);//setting a bit to 1 clears its interrupt 11111111
        Wire.endTransmission();
    }
}
