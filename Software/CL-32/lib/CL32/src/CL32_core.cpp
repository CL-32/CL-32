#include <Arduino.h>
#include <Wire.h>
#include "CL32.h"
#include "CL32_core.h"

//some keyboard things mapping to make things readable
byte KB_SHIFT = 225;
byte KB_RET = 40;
byte KB_ESC = 41;
byte KB_BSP = 42;
byte KB_TAB = 43;
byte KB_FN = 58;
byte KB_FILE = 67;
byte KB_MENU = 68;
byte KB_RGHT = 79;
byte KB_LEFT = 80;
byte KB_DOWN = 81;
byte KB_UP = 82;
byte KB_PG_UP = 75;
byte KB_PG_DN = 78;
byte KB_HOME = 74;
byte KB_END = 77;

CL32_core::CL32_core() {
    byte bStatus;
    _eventCount= 0;
    //start the i2c
    Wire.begin(CL32_sda,CL32_scl);


    Wire.beginTransmission(CORE_ADDRESS);
    Wire.write(0x02);//0x02 is where we start
    Wire.endTransmission();
    Wire.requestFrom(CORE_ADDRESS,3);
    while(Wire.available() < 1);//sit and wait for response
    bStatus = Wire.read();
	_CL32usb = bitRead(bStatus,1);
	_CL32charging = bitRead(bStatus,1);
    Wire.endTransmission();
    //could make a keymap in here
    //i guess this could be loaded in from a file...
    _matrix[0]  = { 0 ,	     0 ,    0 ,     0 };//no key pressed
    _matrix[1]  = { 0 ,	     0 ,    0 ,     0 };//keyboard roll over fail
    _matrix[2]  = { 0 ,	     0 ,    0 ,     0 };//keyboard POST fail
    _matrix[3]  = { 0 ,	     0 ,    0 ,     0 };//keyboard error undefined
    _matrix[4]  = {'a',     'A',	'@',    true};//a
    _matrix[5]  = {'b',     'B',    '£',    true};//b
    _matrix[6]  = {'c',     'C',	'$',    true};//c
    _matrix[7]  = {'d',     'D',	'%',    true};//d
    _matrix[8]  = {'e',     'E',	'^',    true};//e
    _matrix[9]  = {'f',     'F',	'&',    true};//f
    _matrix[10] = {'g' ,	'G' ,    0 ,    true};//g
    _matrix[11] = {'h',     'H',KB_SHIFT ,  true};//h
    _matrix[12] = {'i',     'I',	'|',    true};//i
    _matrix[13] = {'j',     'J',	'#',    true};//j
    _matrix[14] = {'k',     'K',	',',    true};//k
    _matrix[15] = {'l',     'L',	'?',    true};//l
    _matrix[16] = {'m',     'M',	'\'',   true};//m
    _matrix[17] = {'n',     'N',	':',    true};//n
    _matrix[18] = {'o',     'O',    ';',    true};//o
    _matrix[19] = {'p',     'P',	'{',    true};//p
    _matrix[20] = {'q',	    'Q',	    0 , true};//q
    _matrix[21] = {'r',     'R',  KB_FN,    true};//r
    _matrix[22] = {'s',     'S',	' ',    true};//s
    _matrix[23] = {'t',     'T',	'\\',   true};//t
    _matrix[24] = {'u',     'U',	'~',    true};//u
    _matrix[25] = {'v',     'V',	' ' ,   true};//v
    _matrix[26] = {'w',     'W',	' ' ,   true};//w
    _matrix[27] = {'x',     'X',	'<',    true};//x
    _matrix[28] = {'y',     'Y',	'>',    true};//y
    _matrix[29] = {'z',     'Z',	'[',    true};//z
    _matrix[30] = { 0 ,	    0 ,	    0 ,     0 };//1
    _matrix[31] = { 0 ,	    0 ,	    0 ,     0 };//2
    _matrix[32] = { 0 ,	    0 ,	    0 ,     0 };//3
    _matrix[33] = { 0 ,	    0 ,	    0 ,     0 };//4
    _matrix[34] = { 0 ,	    0 ,	    0 ,     0 };//5
    _matrix[35] = { 0 ,	    0 ,	    0 ,     0 };//6
    _matrix[36] = { 0 ,	    0 ,	    0 ,     0 };//7
    _matrix[37] = { 0 ,	    0 ,	    0 ,     0 };//8
    _matrix[38] = { 0 ,	    0 ,	    0 ,     0 };//9
    _matrix[39] = { 0 ,	    0 ,	    0 ,     0 };//0
    _matrix[40] = {KB_RET, KB_RET, KB_RET, false};//return
    _matrix[41] = {KB_ESC, KB_ESC, KB_ESC, false};//escape
    _matrix[42] = {KB_BSP, KB_BSP, KB_BSP, false};//backspace
    _matrix[43] = { 0 ,	    0 ,	    0 ,     0 };//tab
    _matrix[44] = {' ',    ' ',    ' ',    true};//spacebar
    _matrix[45] = { 0 ,	    0 ,	    0 ,     0 };//- or _
    _matrix[46] = { 0 ,	    0 ,	    0 ,     0 };//= or +
    _matrix[47] = { 0 ,	    0 ,	    0 ,     0 };//{ or [
    _matrix[48] = { 0 ,	    0 ,	    0 ,     0 };//} or ]
    _matrix[49] = { 0 ,	    0 ,	    0 ,     0 };//\ or |
    _matrix[50] = { 0 ,	    0 ,	    0 ,     0 };//# or ~
    _matrix[51] = { 0 ,	    0 ,	    0 ,     0 };//; or :
    _matrix[52] = { 0 ,	    0 ,	    0 ,     0 };//' or "
    _matrix[53] = { 0 ,	    0 ,	    0 ,     0 };//` or ~
    _matrix[54] = { 0 ,	    0 ,	    0 ,     0 };//, or <
    _matrix[55] = { 0 ,	    0 ,	    0 ,     0 };//. or >
    _matrix[56] = { 0 ,	    0 ,	    0 ,     0 };// / or ?
    _matrix[57] = { 0 ,	    0 ,	    0 ,     0 };//capslock
    _matrix[58] = { 0 ,	    0 ,	    0 ,     0 };//f1
    _matrix[59] = { 0 ,	    0 ,	    0 ,     0 };//f2
    _matrix[60] = { 0 ,	    0 ,	    0 ,     0 };//f3
    _matrix[61] = { 0 ,	    0 ,	    0 ,     0 };//f4
    _matrix[62] = { 0 ,	    0 ,	    0 ,     0 };//f5
    _matrix[63] = { 0 ,	    0 ,	    0 ,     0 };//f6
    _matrix[64] = { 0 ,	    0 ,	    0 ,     0 };//f7  
    _matrix[65] = { 0 ,     0 ,     0 ,     0 };//f8
    _matrix[66] = { 0 ,	    0 ,	    0 ,     0 };//f9
    _matrix[67] = { 0 ,	    0 ,	    0 ,     0 };//f10
    _matrix[68] = { 0 ,	    0 ,	    0 ,     0 };//f11
    _matrix[69] = { 0 ,	    0 ,	    0 ,     0 };//f12
    _matrix[70] = { 0 ,	    0 ,	    0 ,     0 };//print screen
    _matrix[71] = { 0 ,	    0 ,	    0 ,     0 };//scroll lock
    _matrix[72] = { 0 ,	    0 ,	    0 ,     0 };//pause
    _matrix[73] = { 0 ,	    0 ,	    0 ,     0 };//insert
    _matrix[74] = { 0 ,	    0 ,	    0 ,     0 };//home
    _matrix[75] = { 0 ,	    0 ,     0 ,     0 };//page up
    _matrix[76] = { 0 ,	    0 ,	    0 ,     0 };//del
    _matrix[77] = { 0 ,	    0 ,	    0 ,     0 };//end
    _matrix[78] = { 0 ,	    0 ,	    0 ,     0 };//page down
    _matrix[79] = {KB_RGHT, KB_RGHT,KB_END,false};//right
    _matrix[80] = {KB_LEFT, KB_LEFT,KB_HOME,false};//left
    _matrix[81] = {KB_DOWN, KB_DOWN,KB_PG_DN,false};//down
    _matrix[82] = {KB_UP,   KB_UP,  KB_PG_UP,  false};//up
    _matrix[83] = {0,       0,      0,          0};//numlock
    _matrix[84] = {'/',     '/',	'/',    true};//keypad /
    _matrix[85] = {'*',     '*',    '*',    true};// keypad *
    _matrix[86] = {'-',     '-',    '-',    true};//keypad -
    _matrix[87] = {'+',     '+',	'+',    true};//keypad +
    _matrix[88] = {KB_RET, KB_RET, KB_RET, false};//keypad enter
    _matrix[89] = {'1',     '1',	'1',    true};//keypad 1
    _matrix[90] = {'2',     '2',	'2' ,   true};//keypad 2
    _matrix[91] = {'3',     '3',	'3' ,   true};//keypad 3
    _matrix[92] = {'4',     '4',	'4',    true};//keypad 4
    _matrix[93] = {'5',     '5',	'5',    true};//keypad 5
    _matrix[94] = {'6',     '6',	'6',    true};//keypad 6
    _matrix[95] = {'7' ,	'7' ,   '7' ,   true};//keypad 7
    _matrix[96] = {'8',     '9',    '8',    true };//keypad 8
    _matrix[97] = {'9',     '9',    '9',    true};//keypad 9
    _matrix[98] = {'0',     '0',    '0',    true};//keypad 0
    _matrix[99] = {'+',     '+',    '+',    true};//keypad .
    _matrix[100] = {0,       0,      0,          0};// \ or |
    _matrix[101] = {KB_FN,KB_FN,   KB_FN,   false};//application
    _matrix[102] = {0,       0,      0,          0};//power
    _matrix[103] = {0,       0,      0,          0};//keypad =
    _matrix[104] = {0,       0,      0,          0};//f13
    _matrix[105] = {0,       0,      0,          0};//f14
    _matrix[106] = {0,       0,      0,          0};//f15
    _matrix[107] = {0,       0,      0,          0};//f16
    _matrix[108] = {0,       0,      0,          0};//f17
    _matrix[109] = {0,       0,      0,          0};//f18
    _matrix[110] = {0,       0,      0,          0};//f19
    _matrix[111] = {0,       0,      0,          0};//f20
    _matrix[112] = {0,       0,      0,          0};//f21
    _matrix[113] = {0,       0,      0,          0};//f22
    _matrix[114] = {0,       0,      0,          0};//f23
    _matrix[115] = {0,       0,      0,          0};//f24
    _matrix[116] = {KB_FILE, KB_FILE, KB_FILE, false};//execute
    _matrix[117] = {0,       0,      0,          0};//help
    _matrix[118] = {KB_MENU, KB_MENU, KB_MENU, false};//menu
    _matrix[119] = {KB_RET,  KB_RET,  KB_RET,  false};//select
    _matrix[120] = {KB_ESC,  KB_ESC, KB_ESC,   false};//stop
    _matrix[121] = {0,       0,      0,          0};//again
    _matrix[122] = {0,       0,      0,          0};//undo
    _matrix[123] = {0,       0,      0,          0};//cut
    _matrix[124] = {0,       0,      0,          0};//copy
    _matrix[125] = {0,       0,      0,          0};//paste
    _matrix[126] = {0,       0,      0,          0};//find
    _matrix[127] = {KB_SHIFT,KB_SHIFT,KB_SHIFT,false};//shift - should be mute
    
}
void CL32_core::init(){
    //start up the keyboard
    //we dont need to set anything up anymore, it just works!! TM
}  

void CL32_core::read(){
    
    byte bCount, bEvent, bEventCode, bStatus, bInterrupt, bData;
    bool bEventStat, bRefresh = false;
    //get the voltage
    Wire.beginTransmission(CORE_ADDRESS);
    Wire.write(0x00);//0x02 is where we start
    Wire.endTransmission();
    Wire.requestFrom(CORE_ADDRESS,1);
    while(Wire.available() < 1);//sit and wait for response
    _CL32voltage = Wire.read();
    _CL32voltage = _CL32voltage * 25;
    Wire.endTransmission();

    Wire.beginTransmission(CORE_ADDRESS);
    Wire.write(0x01);//0x02 is where we start
    Wire.endTransmission();
    Wire.requestFrom(CORE_ADDRESS,1);
    while(Wire.available() < 1);//sit and wait for response
    bStatus = Wire.read();
    Wire.endTransmission();


    Wire.beginTransmission(CORE_ADDRESS);
    Wire.write(0x02);//0x02 is where we start
    Wire.endTransmission();
    Wire.requestFrom(CORE_ADDRESS,1);
    while(Wire.available() < 1);//sit and wait for response
    bInterrupt = Wire.read();
    Wire.endTransmission();
    
    //check for keypress
    if (bitRead(bInterrupt,0)){
        Wire.beginTransmission(CORE_ADDRESS);
        Wire.write(0x03);//0x03 is the buffer count
        Wire.endTransmission();
        //read the contents
        Wire.requestFrom(CORE_ADDRESS,1);
        while(Wire.available() < 1);//sit and wait for response
        //read the value
        bCount = Wire.read();
        //if there is anything available, grab events
        while(bCount > 0){
            //the buffer is magic, when you read an event, it pops them
            //all back down the list so that you just keep reading the 
            //first item until you run out
            Wire.beginTransmission(CORE_ADDRESS);
            Wire.write(0x04);//0x04 is the fist event
            Wire.endTransmission();
            //read the contents
            Wire.requestFrom(CORE_ADDRESS,1);
            while(Wire.available() < 1);//sit and wait for response
            //read the value
            bEvent = Wire.read();
            bEventCode = bEvent & 0x7f;
            bEventStat = (bEvent & 0x80)>>7;
            //lets process shift/fn here, so we dont have to do it later
            if(bEventStat==1){//we only care about button going down at the moment
            bRefresh = true;//flag that there is something to update, even if its not being added to the buffer
            if(_matrix[bEventCode].lower==KB_SHIFT){
                switch (_shift) {
                    case UNPRESSED: _shift =  ONEPRESS; break;
                    case ONEPRESS: _shift = LOCKPRESS; break;
                    case LOCKPRESS: _shift = UNPRESSED;
                }
            }
            else if(_matrix[bEventCode].lower==KB_FN){
                switch (_fn) {
                    case UNPRESSED: _fn =  ONEPRESS; break;
                    case ONEPRESS: _fn = LOCKPRESS; break;
                    case LOCKPRESS: _fn = UNPRESSED;
                }
            }
            else if(_matrix[bEventCode].lower==KB_MENU){
                switch (isMenu) {
                    case OFF: isMenu =  ON; break;
                    case ON: isMenu = OFF; break;
                    case SUB: isMenu = ON;
                }
            }
            else if(_matrix[bEventCode].lower==KB_FILE){
                switch (isMenu) {
                    case OFF: isMenu =  SUB; break;
                    case SUB: isMenu = OFF;
                    //if the menu is on, we dont do anything, the file button is redundant when in the menu mode
                }
            }
            else{
                //add the event to the global register
                if(_fn!=UNPRESSED){
                    _eventLog[_eventCount] = {_matrix[bEventCode].func,bEventStat,_matrix[bEventCode].isChar};
                    if(_fn==ONEPRESS){//we are not at lock yet
                        _fn=UNPRESSED;
                    }
                }
                else if(_shift!=UNPRESSED){
                    _eventLog[_eventCount] = {_matrix[bEventCode].upper,bEventStat,_matrix[bEventCode].isChar};
                    if(_shift==ONEPRESS){//we are not at lock yet
                        _shift=UNPRESSED;
                    }
                }
                else{
                    _eventLog[_eventCount] = {_matrix[bEventCode].lower,bEventStat,_matrix[bEventCode].isChar};
                }
                _eventCount++;
            }
            }
            bCount --;
        }//reset the fact there have been keys pressed
    }
    //check for USB plug state change
    if (bitRead(bInterrupt,1)){
        bRefresh = true;
	    _CL32usb = bitRead(bStatus,1);
    }
    //check for Charge state change
    if (bitRead(bInterrupt,2)){
        bRefresh = true;        
	    _CL32charging = bitRead(bStatus,1);
    }
    //need to trigger the callback function for whatever app is live
    //we only need to trigger the callback if there is anything to process
    if(bRefresh){
        _action();
    }
    //reset the interruot status
    Wire.beginTransmission(CORE_ADDRESS);
    Wire.write(0x02);//0x02 is the interrupt register
    Wire.write(0x00);//setting a bit to 0 clears its interrupt
    Wire.endTransmission();
}
void CL32_core::add_callback(CL32_KB_t act){
    _lastAction = _action;//save the current callback before setting the new one
    _action = act;
}

void CL32_core::last_callback(){
    _action = _lastAction;//set the old callback back into play
}

void CL32_core::trigger_callback(){
    _action();
}

Event CL32_core::getKey(){
    Event eTemp;
    eTemp = _eventLog[0];
    //move everything down one
    for(byte i = 0;i<_eventCount;i++){
        _eventLog[i] = _eventLog[i+1];
    }
    if(_eventCount>0){
        _eventCount--;
    }
    return eTemp;
}
byte CL32_core::eventCount(){
    return _eventCount;
}


int CL32_core::getVoltage(){
    return _CL32voltage;
}
int CL32_core::getPercent(){
    return 50;
}

bool CL32_core::getUSB(){
    return _CL32usb;
}
bool CL32_core::getCharging(){
    return _CL32charging;
}