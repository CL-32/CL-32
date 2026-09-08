#ifndef I2C_H
#define I2C_H
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

//  REGISTERS
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
#define CL32_FL_BRIGHT 0x11 //screen frontlight brightness
#define CL32_FL_TIME 0x12 //screen frontlight timeout
#define CL32_SUPPLY 0x13 //supply voltage /25
#define CL32_VOLTAGE 0x14 //battery voltage /25

extern volatile uint8_t i2c_regs[30];
extern volatile uint8_t reg_index;
extern volatile uint8_t addr_phase;
extern volatile uint8_t seto;
extern volatile uint8_t reseto;
extern volatile uint8_t reflag;


void i2c_slave_init(uint8_t address);
void set_i2c_flag(void);
void clear_i2c_flag(void);

#endif