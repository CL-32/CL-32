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
