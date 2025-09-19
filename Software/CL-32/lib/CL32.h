#ifndef CL32_h
#define CL32_h
#include "Arduino.h" 

extern const byte RTC_ADDRESS = 0x51;
extern const byte BATTERY_ADDRESS = 0x36;
extern const byte CL32_sck = 9;
extern const byte CL32_epd_cs = 6;
extern const byte CL32_sd_cs = 7;
extern const byte CL32_mosi = 10;
extern const byte CL32_miso = 11;
extern const byte CL32_dc = 13;
extern const byte CL32_rst = 12;
extern const byte CL32_bsy = 14;
extern const byte CL32_sda = 1;
extern const byte CL32_scl = 2;
extern const byte CL32_int = 3;
extern const byte CL32_RGB = 4;
extern const byte CL32_buz = 5;
extern const byte CL32_kill = 45;

struct Event {
  int keyData;
  bool keyDown;
  bool isChar;
};

extern Event KB_eventLog[20];
extern byte KB_eventCount;



#endif