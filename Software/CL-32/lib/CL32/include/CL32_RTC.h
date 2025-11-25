#ifndef CL32_rtc_h
#define CL32_rtc_h
#include "Arduino.h" 
#include "CL32_pins.h"


class CL32_rtc{
public:
	CL32_rtc();
	void init();
    void loadTime();
    void saveTime(struct tm timeIn);
    char timeText[6];
    void read();
private:
    struct tm _CL32time;
};
#endif