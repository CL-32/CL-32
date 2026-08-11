#ifndef GEN_H
#define GEN_H
#include <avr/io.h>

#define bitWrite(value, bit, bitvalue) (bitvalue ? ((value) |= (1UL << (bit))) : ((value) &= ~(1UL << (bit))))
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

typedef enum{
    BUTTON,
    I2C,
    USB
} reason_t;
extern volatile reason_t wakeReason;

typedef enum{
    RED,
    GRN
} cur_led_t;
extern volatile cur_led_t currentLED;


extern volatile uint16_t BLcount;
extern volatile uint16_t buttonCount;
extern volatile uint16_t loopCount;
extern volatile uint16_t lastKey;
extern volatile uint16_t killCount;

#endif