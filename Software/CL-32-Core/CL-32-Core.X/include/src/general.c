#include "../general.h"

volatile reason_t wakeReason;
volatile cur_led_t currentLED;


volatile uint16_t BLcount = 0;
volatile uint16_t buttonCount = 0;
volatile uint16_t loopCount = 0;
volatile uint16_t lastKey = 0;
volatile uint16_t killCount = 0;