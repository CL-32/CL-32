#ifndef CL32_pins_h
#define CL32_pins_h
#include <Arduino.h>
#include <SPI.h>

extern byte RTC_ADDRESS;
extern byte BATTERY_ADDRESS;
extern byte CL32_sck;
extern byte CL32_epd_cs;
extern byte CL32_sd_cs;
extern byte CL32_mosi;
extern byte CL32_miso;
extern byte CL32_dc;
extern byte CL32_rst;
extern byte CL32_bsy;
extern byte CL32_sda;
extern byte CL32_scl;
extern byte CL32_int;
extern byte CL32_RGB;
extern byte CL32_buz;
extern byte CL32_kill;

extern SPIClass hspi;

#endif