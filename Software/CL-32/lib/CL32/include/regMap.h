#pragma once

//registers
#define CL32_VERSION 0x00 //hardware version
#define CL32_STATUS 0x01 //system status | 0 power on | 1 USB | 2 charge | 7 request off
#define CL32_INTERRUPT 0x02 //interrupt status | 0 keyboard | 1 USB | 2 charge | 3 battery
#define CL32_EVENT_COUNT 0x03 //event count
#define CL32_EVENT1 0x04 //event data 1
#define CL32_EVENT2 0x05 //event data 2
#define CL32_EVENT3 0x06 //event data 3
#define CL32_EVENT4 0x07 //event data 4
#define CL32_EVENT5 0x08 //event data 5
#define CL32_EVENT6 0x09 //event data 6
#define CL32_EVENT7 0x0A //event data 7
#define CL32_EVENT8 0x0B //event data 8
#define CL32_EVENT9 0x0C //event data 9
#define CL32_EVENT10 0x0D //event data 10
#define CL32_POWER_BRIGHT 0x0E //power LED brightness
#define CL32_BL_BRIGHT 0x0F //backlight brightness
#define CL32_BL_TIME 0x10 //backlight timeout
#define CL32_RAW_ROW0 0x11 //raw row0 data
#define CL32_RAW_ROW1 0x12 //raw row1 data
#define CL32_RAW_ROW2 0x13 //raw row2 data
#define CL32_RAW_ROW3 0x14 //raw row3 data
#define CL32_RAW_ROW4 0x15 //raw row4 data
#define CL32_RAW_ROW5 0x16 //raw row5 data
#define CL32_RAW_ROW6 0x17 //raw row6 data
#define CL32_RAW_ROW7 0x18 //raw row7 data
#define CL32_SUPPLY 0x19 //supply voltage /25
#define CL32_VOLTAGE 0x1A //battery voltage /25
#define CL32_SPARE3 0x1B //spare
#define CL32_SPARE4 0x1C //spare
#define CL32_SPARE5 0x1D //spare