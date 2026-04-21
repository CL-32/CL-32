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
    Wire.write(0x01);//0x01 is where we start
	Wire.write(0x00);//disable all interrupts
    Wire.endTransmission();

    
    loadTime();
    _onTime = _CL32time;
}

void CL32_rtc::loadTime(){ 
    byte bData;
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x04);//0x04 is where we start
    Wire.endTransmission();
    //read the contents
    Wire.requestFrom(RTC_ADDRESS,7);
    while(Wire.available() < 1);//sit and wait for response
    //read the value
    _CL32time.tm_sec = bcdToDec( Wire.read() & 0x7F ); 	// ignore bit 7
    _CL32time.tm_min = bcdToDec( Wire.read() & 0x7F );
    _CL32time.tm_hour = bcdToDec( Wire.read() & 0x3F );		// ignore bits 7 & 6
    _CL32time.tm_mday = bcdToDec( Wire.read() & 0x3F );
    _CL32time.tm_wday = bcdToDec( Wire.read() & 0x07);	// ignore bits 7,6,5,4 & 3
    _CL32time.tm_mon = bcdToDec( Wire.read() & 0x1F);		// ignore bits 7,6 & 5
    _CL32time.tm_year = bcdToDec( Wire.read()) + 1970;
    //update timetext variable
    sprintf(_timeText, "%02d:%02d", _CL32time.tm_hour, _CL32time.tm_min);
}

void CL32_rtc::saveTime(struct tm timeIn){
    byte bData;
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x04);//0x04 is where we start
	Wire.write(decToBcd(_CL32time.tm_sec));
	Wire.write(decToBcd(_CL32time.tm_min));
	Wire.write(decToBcd(_CL32time.tm_hour));
	Wire.write(decToBcd(_CL32time.tm_mday));
	Wire.write(decToBcd(_CL32time.tm_wday));
	Wire.write(decToBcd(_CL32time.tm_mon));
	Wire.write(decToBcd(_CL32time.tm_year-1970));
    Wire.endTransmission();
}

void CL32_rtc::read(){
    //read/clear the interrupt
    byte bStatus;

    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(0x03);//0x03 is the RAM register
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
        Wire.write(0x00);//setting a bit to 1 clears its interrupt 11111111
        Wire.endTransmission();
    }
}


char* CL32_rtc::getTimeText(){
    return _timeText;
}

char* CL32_rtc::getUptimeText(){
    double uptimeSec;
    int h,m;
    uptimeSec = difftime(mktime(&_CL32time),mktime(&_onTime));
    h = trunc(uptimeSec/3600);//get the whole hours from the seconds
    m = int(uptimeSec) % 3600;//get the remainder seconds
    m = trunc(m/60);//chop out the number of minutes from those seconds
    //we dont care about the seconds left, we are not being that precise
    sprintf(_uptimeText,"Uptime %d:%02d",h,m);
    return _uptimeText;
}

struct tm CL32_rtc::getTimeStruct(){
    return _CL32time;
}


byte CL32_rtc::bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

byte CL32_rtc::decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}