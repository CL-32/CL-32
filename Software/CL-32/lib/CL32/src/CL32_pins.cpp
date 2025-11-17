#include "CL32_pins.h"
#include "Arduino.h" 
#include <SPI.h>

byte RTC_ADDRESS = 0x51;
byte BATTERY_ADDRESS = 0x36;
byte CL32_sck = 9;
byte CL32_epd_cs = 6;
byte CL32_sd_cs = 7;
byte CL32_mosi = 10;
byte CL32_miso = 11;
byte CL32_dc = 13;
byte CL32_rst = 12;
byte CL32_bsy = 14;
byte CL32_sda = 1;
byte CL32_scl = 2;
byte CL32_int = 3;
byte CL32_RGB = 4;
byte CL32_buz = 5;
byte CL32_kill = 45;

SPIClass hspi(HSPI);