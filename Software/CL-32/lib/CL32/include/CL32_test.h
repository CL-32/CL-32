#ifndef CL32_test_h
#define CL32_test_h
#include "Arduino.h" 
const byte TEST_ADDRESS = 0x08;

class CL32_test{
public:
	CL32_test();
	void init();
    void read();
    int getVolts();
private:
    int  _voltage;
    bool  _buttonState;
    bool _USBstate;
    bool _chargeState;
};
#endif