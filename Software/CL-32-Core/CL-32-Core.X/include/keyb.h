#ifndef KEYB_H
#define KEYB_H
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 3333333UL
#include <util/delay.h>

extern volatile uint8_t keyVal[8][8]; 

extern volatile uint8_t fifo_head;
extern volatile uint8_t fifo_tail;
extern volatile uint8_t fifo_count;

#define EVENT_FIFO_SIZE 10

void fifo_push(uint8_t event);
uint8_t fifo_pop(void);
void keyb_init(void);
void set_key_interrupt(void);
void clear_key_interrupt(void);
void read_keys(void);
void check_keys(void);
uint8_t read_key(uint8_t row,uint8_t col);


#endif