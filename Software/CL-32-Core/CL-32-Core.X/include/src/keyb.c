#include "../keyb.h"
#include "../general.h"
#include "../i2c.h"
#include "../led.h"
#include "../power.h"

//volatile uint8_t keyVal[8][8] = { 43, 20, 26,  8, 21, 23, 28, 24,//tab Q W E R T Y U
//                      12,127,  4, 22,  7,  9, 10, 11,//I shift A S D F G H 
//                      13, 14,101, 29, 27,  6, 44, 44,//J K fn Z X C space space
//                      25,  5, 17, 46, 55, 98, 87, 82,//V B N = . 0 + up
//                      79,119, 40, 16, 91, 90, 89, 86,//right ok return M 3 2 1 -
//                      80, 81,120, 40, 15, 94, 93, 92,//left down cancel return L 6 5 4
//                      85,116,118, 42, 19, 18, 97, 96,//* file, menu bspace P O 9 8
//                      95, 84,  0,  0,  0,  0 , 0 , 0};//7 / 

volatile uint8_t keyVal[8][8] =
{
    { 43, 12, 13, 25, 79, 80, 85, 95 },//tab I     J     V  RGHT   LFT    *      7
    { 20,127, 14,  5,119, 81,116, 84 },//Q   SHIFT K     B  OK     DWN    FILE   /
    { 26,  4,101, 17, 40,120,118,  0 },//W   A     FN    N  RETURN CANCEL MENU
    {  8, 22, 29, 46, 16, 40, 42,  0 },//E   S     Z     =  M      RETURN BSPACE
    { 21,  7, 27, 55, 91, 15, 19,  0 },//R   D     X     .  3      L      P
    { 23,  9,  6, 98, 90, 94, 18,  0 },//T   F     C     0  2      6      O
    { 28, 10, 44, 87, 89, 93, 97,  0 },//Y   G     SPACE +  1      5      9
    { 24, 11, 44, 82, 86, 92, 96,  0 }//U    H     SPACE UP -      4      8
};

volatile uint8_t fifo_head = 0;
volatile uint8_t fifo_tail = 0;
volatile uint8_t fifo_count = 0;

//set a row struct
typedef struct {
    volatile uint8_t *outset;
    volatile uint8_t *outclr;
    uint8_t mask;
} row_pin_t;
//now make an array of the rows that we can use
const row_pin_t rows[] = {
    { &PORTD.OUTSET, &PORTD.OUTCLR, PIN5_bm },//{ &PORTF.OUTSET, &PORTF.OUTCLR, PIN6_bm },
    { &PORTD.OUTSET, &PORTD.OUTCLR, PIN4_bm },//{ &PORTF.OUTSET, &PORTF.OUTCLR, PIN4_bm },
    { &PORTD.OUTSET, &PORTD.OUTCLR, PIN3_bm },//{ &PORTF.OUTSET, &PORTF.OUTCLR, PIN3_bm },
    { &PORTD.OUTSET, &PORTD.OUTCLR, PIN2_bm },//{ &PORTF.OUTSET, &PORTF.OUTCLR, PIN2_bm },
    { &PORTA.OUTSET, &PORTA.OUTCLR, PIN7_bm },//{ &PORTF.OUTSET, &PORTF.OUTCLR, PIN1_bm },
    { &PORTA.OUTSET, &PORTA.OUTCLR, PIN6_bm },//{ &PORTF.OUTSET, &PORTF.OUTCLR, PIN0_bm },
    { &PORTA.OUTSET, &PORTA.OUTCLR, PIN5_bm },//{ &PORTD.OUTSET, &PORTD.OUTCLR, PIN7_bm },
    { &PORTA.OUTSET, &PORTA.OUTCLR, PIN4_bm },//{ &PORTD.OUTSET, &PORTD.OUTCLR, PIN6_bm },
};
//and finally helper macros
#define ROW_HIGH(r) (*rows[r].outset = rows[r].mask)
#define ROW_LOW(r)  (*rows[r].outclr = rows[r].mask)

//then lets sort some cols
typedef struct {
    volatile uint8_t *in;
    uint8_t mask;
} col_pin_t;
//and the array of cols
const col_pin_t cols[] = {
    { &PORTF.IN, PIN6_bm },//{ &PORTD.IN, PIN5_bm },
    { &PORTF.IN, PIN4_bm },//{ &PORTD.IN, PIN4_bm },
    { &PORTF.IN, PIN3_bm },//{ &PORTD.IN, PIN3_bm },
    { &PORTF.IN, PIN2_bm },//{ &PORTD.IN, PIN2_bm },
    { &PORTF.IN, PIN1_bm },//{ &PORTA.IN, PIN7_bm },
    { &PORTF.IN, PIN0_bm },//{ &PORTA.IN, PIN6_bm },
    { &PORTD.IN, PIN7_bm },//{ &PORTA.IN, PIN5_bm },
    { &PORTD.IN, PIN6_bm },//{ &PORTA.IN, PIN4_bm },
};
//and the macro to use this
#define COL_READ(c) ((*cols[c].in & cols[c].mask) ? 1 : 0)


void keyb_init(void){
    //set the rows as output
    PORTD.DIRSET = PIN5_bm;//PORTF.DIRSET = PIN6_bm;
    PORTD.DIRSET = PIN4_bm;//PORTF.DIRSET = PIN4_bm;
    PORTD.DIRSET = PIN3_bm;//PORTF.DIRSET = PIN3_bm;
    PORTD.DIRSET = PIN2_bm;//PORTF.DIRSET = PIN2_bm;
    PORTA.DIRSET = PIN7_bm;//PORTF.DIRSET = PIN1_bm;
    PORTA.DIRSET = PIN6_bm;//PORTF.DIRSET = PIN0_bm;
    PORTA.DIRSET = PIN5_bm;//PORTD.DIRSET = PIN7_bm;
    PORTA.DIRSET = PIN4_bm;//PORTD.DIRSET = PIN6_bm;
    //ser the cols as input
    PORTF.DIRCLR = PIN6_bm;//PORTD.DIRCLR = PIN5_bm;
    PORTF.DIRCLR = PIN4_bm;//PORTD.DIRCLR = PIN4_bm;
    PORTF.DIRCLR = PIN3_bm;//PORTD.DIRCLR = PIN3_bm;
    PORTF.DIRCLR = PIN2_bm;//PORTD.DIRCLR = PIN2_bm;
    PORTF.DIRCLR = PIN1_bm;//PORTA.DIRCLR = PIN7_bm;
    PORTF.DIRCLR = PIN0_bm;//PORTA.DIRCLR = PIN6_bm;
    PORTD.DIRCLR = PIN7_bm;//PORTA.DIRCLR = PIN5_bm;
    PORTD.DIRCLR = PIN6_bm;//PORTA.DIRCLR = PIN4_bm;
    //and the pull up for the cols
    PORTF_PIN6CTRL |= PORT_PULLUPEN_bm;//PORTD_PIN5CTRL |= PORT_PULLUPEN_bm;
    PORTF_PIN4CTRL |= PORT_PULLUPEN_bm;//PORTD_PIN4CTRL |= PORT_PULLUPEN_bm;
    PORTF_PIN3CTRL |= PORT_PULLUPEN_bm;//PORTD_PIN3CTRL |= PORT_PULLUPEN_bm;
    PORTF_PIN2CTRL |= PORT_PULLUPEN_bm;//PORTD_PIN2CTRL |= PORT_PULLUPEN_bm;
    PORTF_PIN1CTRL |= PORT_PULLUPEN_bm;//PORTA_PIN7CTRL |= PORT_PULLUPEN_bm;
    PORTF_PIN0CTRL |= PORT_PULLUPEN_bm;//PORTA_PIN6CTRL |= PORT_PULLUPEN_bm;
    PORTD_PIN7CTRL |= PORT_PULLUPEN_bm;//PORTA_PIN5CTRL |= PORT_PULLUPEN_bm;
    PORTD_PIN6CTRL |= PORT_PULLUPEN_bm;//PORTA_PIN4CTRL |= PORT_PULLUPEN_bm;
    //set some defaults
    read_keys();//to init the current key states
    fifo_count = 0;
    fifo_head = 0;
    fifo_tail = 0;
}

ISR(PORTF_PORT_vect){
    // Did PD4 trigger?
    if (PORTF.INTFLAGS & PIN1_bm){
        // clear interrupt flag
        PORTF.INTFLAGS = PIN1_bm;
        wakeReason = BUTTON;
    }
}
void fifo_push(uint8_t event){
    if (fifo_count >= EVENT_FIFO_SIZE)
        return; // overflow handling

    i2c_regs[fifo_head + CL32_EVENT1] = event;

    fifo_head++;
    if (fifo_head >= EVENT_FIFO_SIZE)
        fifo_head = 0;

    fifo_count++;
    
    i2c_regs[CL32_EVENT_COUNT] = fifo_count;
}

uint8_t fifo_pop(void){
    if (fifo_count == 0)
        return 0;

    uint8_t event = i2c_regs[fifo_tail + CL32_EVENT1];

    fifo_tail++;
    if (fifo_tail >= EVENT_FIFO_SIZE)
        fifo_tail = 0;

    fifo_count--;

    i2c_regs[CL32_EVENT_COUNT] = fifo_count;
    
    return event;
}

void set_key_interrupt(void){ 
    for(uint8_t i=0;i<8;i++){//set the rows all high
        ROW_HIGH(i);
    }
    ROW_LOW(1);
    PORTF.PIN1CTRL = (PORTF.PIN1CTRL & ~PORT_ISC_gm) | PORT_ISC_FALLING_gc ;//falling
}

void clear_key_interrupt(void){
     PORTF.PIN1CTRL = (PORTF.PIN1CTRL & ~PORT_ISC_gm) | PORT_ISC_INTDISABLE_gc ;
}

void read_keys(void){
    uint8_t r, c;
    for(r=0;r<8;r++){//set the rows all high
        ROW_HIGH(r);
    }
    for(r=0;r<8;r++){
        ROW_LOW(r);//set the pin low to start
        for(c=0;c<8;c++){
            //log the change
            bitWrite(i2c_regs[r+CL32_RAW_ROW0], c, (COL_READ(c)) );
        }
        ROW_HIGH(r);//set it high to tidy up
    }
}

void check_keys(void){
    uint8_t r, c, changeCount, temp;
    changeCount=0;
    for(r=0;r<8;r++){//set the rows all high
        ROW_HIGH(r);
    }
    for(r=0;r<8;r++){
        ROW_LOW(r);//set the pin low to start
        for(c=0;c<8;c++){
            if(bitRead(i2c_regs[r+CL32_RAW_ROW0],c) != COL_READ(c) ){//state change
                //add event
                if(is_on()){//only log it if we are on
                    temp = keyVal[r][c];//get the key value//
                    if(COL_READ(c)==1){ 
                        bitWrite(temp, 7, 0);
                    }
                    else{
                        bitWrite(temp, 7, 1); 
                    }
                    fifo_push(temp);
                    changeCount++;
                }
                //log the change
                bitWrite(i2c_regs[r+CL32_RAW_ROW0], c, (COL_READ(c)) );
            }
        }
        ROW_HIGH(r);//set it high to tidy up
    }
    if(changeCount>0){    
        bitWrite(i2c_regs[CL32_INTERRUPT],0,1);
        set_i2c_flag();
        BLcount = 0;
    }
    if(bitRead(i2c_regs[CL32_RAW_ROW1],4)==0 && bitRead(i2c_regs[CL32_RAW_ROW2],2)==0){
        killCount++;
    }
    else{
        killCount=0;
    }
}

uint8_t read_key(uint8_t row,uint8_t col){
    uint8_t r; 
    for(r=0;r<8;r++){//set the rows all high
        ROW_HIGH(r);
    }
    ROW_LOW(row);//set the pin low to start
    return COL_READ(col);
    ROW_HIGH(row);//set it high to tidy up
    
}