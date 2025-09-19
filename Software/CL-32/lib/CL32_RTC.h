#ifndef CL32_rtc_h
#define CL32_rtc_h
#include "Arduino.h" 


const byte RTC_ADDRESS = 0x51;

class CL32_rtc{
public:
	CL32_rtc();
	void rtcInit();
    void loadTime();
    void saveTime(struct tm timeIn);
    char * timeText();
private:
    struct tm _CL32time;
};
#endif