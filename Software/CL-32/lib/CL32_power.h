#ifndef CL32_power_h
#define CL32_power_h
#include "Arduino.h" 


const byte BATTERY_ADDRESS = 0x36;

class CL32_power{
public:
	CL32_power();
	void powerInit();
    void loadPower();
    int getVoltage();
    int getPercent();
    int getLoad();
private:
    int  _CL32voltage;
    int  _CL32percent;
    int  _CL32load;
};
#endif