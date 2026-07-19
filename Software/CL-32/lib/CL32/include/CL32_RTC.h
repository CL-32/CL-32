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
    void read();
    char* getTimeText();
    char* getUptimeText();
    struct tm getTimeStruct();
private:
    char _timeText[6], _uptimeText[20];
    struct tm _CL32time, _onTime;
    uint8_t bcdToDec(uint8_t val);
    uint8_t decToBcd(uint8_t val);
};
#endif