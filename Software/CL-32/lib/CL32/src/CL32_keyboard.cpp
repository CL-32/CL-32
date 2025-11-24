#include <Arduino.h>
#include <Wire.h>
#include "CL32.h"
#include "CL32_keyboard.h"

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

CL32_keyboard::CL32_keyboard() {
    _eventCount= 0;
    //start the i2c
    Wire.begin(CL32_sda,CL32_scl);
    //could make a keymap in here
    //i guess this could be loaded in from a file...
    _matrix[0]  = { 0 ,	     0 ,    0 ,     0 };
    _matrix[1]  = {KB_TAB,  KB_TAB, KB_TAB, false};
    _matrix[2]  = {'q',     'Q',    '!',    true};
    _matrix[3]  = {'w',     'W',    '"',    true};
    _matrix[4]  = {'e',     'E',	'@',    true};
    _matrix[5]  = {'r',     'R',    '£',    true};
    _matrix[6]  = {'t',     'T',	'$',    true};
    _matrix[7]  = {'y',     'Y',	'%',    true};
    _matrix[8]  = {'u',     'U',	'^',    true};
    _matrix[9]  = {'i',     'I',	'&',    true};
    _matrix[10]  = { 0 ,	     0 ,    0 ,     0 };
    _matrix[11] = {KB_SHIFT, KB_SHIFT,KB_SHIFT , false };
    _matrix[12] = {'a',     'A',	'|',    true};
    _matrix[13] = {'s',     'S',	'#',    true};
    _matrix[14] = {'d',     'D',	',',    true};
    _matrix[15] = {'f',     'F',	'?',    true};
    _matrix[16] = {'g',     'G',	'\'',   true};
    _matrix[17] = {'h',     'H',	':',    true};
    _matrix[18] = {'j',     'J',    ';',    true};
    _matrix[19] = {'k',     'K',	'{',    true};
    _matrix[20] = { 0 ,	    0 ,	    0 ,     0 };
    _matrix[21] = {KB_FN,   KB_FN,  KB_FN,  false};
    _matrix[22] = {'z',     'Z',	' ',    true};
    _matrix[23] = {'x',     'X',	'\\',   true};
    _matrix[24] = {'c',     'C',	'~',    true};
    _matrix[25] = {' ',     ' ',	' ' ,   true};
    _matrix[26] = {' ',     ' ',	' ' ,   true};
    _matrix[27] = {'v',     'V',	'<',    true};
    _matrix[28] = {'b',     'B',	'>',    true};
    _matrix[29] = {'n',     'N',	'[',    true};
    _matrix[30] = { 0 ,	    0 ,	    0 ,     0 };
    _matrix[31] = {'=',     '=',    '=',    true };
    _matrix[32] = {'.',     '.',    '.',    true};
    _matrix[33] = {'0',     '0',    '0',    true};
    _matrix[34] = {'+',     '+',    '+',    true};
    _matrix[35] = {KB_UP,   KB_UP,  KB_PG_UP,  false };
    _matrix[36] = {KB_RGHT, KB_RGHT,KB_END,false};
    _matrix[37] = {KB_RET,  KB_RET, KB_RET, false};
    _matrix[38] = {KB_RET,  KB_RET, KB_RET, false};
    _matrix[39] = {'m',     'M',	']',    true};
    _matrix[40] = { 0 ,	    0 ,	    0 ,     0 };
    _matrix[41] = {'3',     '3',	'3',    true};
    _matrix[42] = {'2',     '2',    '2',    true};
    _matrix[43] = {'1',     '1',    '1',    true};
    _matrix[44] = {'-',     '-',    '-',    true};
    _matrix[45] = {KB_LEFT, KB_LEFT,KB_HOME,false};
    _matrix[46] = {KB_DOWN, KB_DOWN,KB_PG_DN,false};
    _matrix[47] = {KB_ESC,  KB_ESC, KB_ESC, false};
    _matrix[48] = {KB_RET,  KB_RET, KB_RET, false};
    _matrix[49] = {'l',     'L',	'}',    true};
    _matrix[50] = { 0 ,	    0 ,	    0 ,     0 };
    _matrix[51] = {'6',     '6',    '6',    true};
    _matrix[52] = {'5',     '5',    '5',    true};
    _matrix[53] = {'4',     '4',    '4',    true};
    _matrix[54] = {'*',     '*',    '*',    true};
    _matrix[55] = {KB_FILE, KB_FILE,KB_FILE,false};
    _matrix[56] = {KB_MENU, KB_MENU,KB_MENU,false};
    _matrix[57] = {KB_BSP,  KB_BSP, KB_BSP, false};
    _matrix[58] = {'p',     'P',    'p',    true};
    _matrix[59] = {'o',     'O',    'o',    true};
    _matrix[60] = { 0 ,	    0 ,	    0 ,     0 };
    _matrix[61] = {'9',     '9',    '9',    true};
    _matrix[62] = {'8',     '8',    '8',    true};
    _matrix[63] = {'7',     '7',    '7',    true};
    _matrix[64] = {'/',     '/',    '/',    true};    
\

    
}
void CL32_keyboard::init(){
    //start up the keyboard
    Wire.beginTransmission(KB_ADDRESS);
    Wire.write(0x01);//0x01 is the config register
    Wire.write(0x19);//same as the driver 00011001
    Wire.endTransmission();
    Wire.beginTransmission(KB_ADDRESS);
    Wire.write(0x02);//0x02 is the interrupt register
    Wire.write(0xff);//setting a bit to 1 clears its interrupt 11111111
    Wire.endTransmission();
    Wire.beginTransmission(KB_ADDRESS);
    Wire.write(0x1d);//0x1d is the row gpio mode flag
    Wire.write(0xff);//all 8 rows 11111111
    Wire.endTransmission();
    Wire.beginTransmission(KB_ADDRESS);
    Wire.write(0x1e);//0x1e is the col gpio mode flag
    Wire.write(0xff);//first 8 cols 11111111
    Wire.endTransmission();
    Wire.beginTransmission(KB_ADDRESS);
    Wire.write(0x1f);//0x1d is the extra col mode flag
    Wire.write(0x01);//last 1 col 00000001
    Wire.endTransmission();
    Wire.beginTransmission(KB_ADDRESS);
    Wire.write(0x29);//0x29 is the row debounce
    Wire.write(0xff);//all 8 rows 11111111
    Wire.endTransmission();
    Wire.beginTransmission(KB_ADDRESS);
    Wire.write(0x2a);//0x2a is the column debounce
    Wire.write(0xff);//first 8 cols 11111111
    Wire.endTransmission();
    Wire.beginTransmission(KB_ADDRESS);
    Wire.write(0x2b);//0x2b is the extra column debounce
    Wire.write(0x01);//last 1 col 00000001
    Wire.endTransmission();
}  

void CL32_keyboard::read(){
    
  byte bCount, bEvent, bEventCode, bStatus;
  bool bEventStat, bRefresh = false;
  Wire.beginTransmission(KB_ADDRESS);
  Wire.write(0x02);//0x02 is the interrupt register
  Wire.endTransmission();
  
  Wire.requestFrom(KB_ADDRESS,1);
  while(Wire.available() < 1);//sit and wait for response
  //read the value
  bStatus = Wire.read();
  if (bStatus!=0){
    Wire.beginTransmission(KB_ADDRESS);
    Wire.write(0x03);//0x03 is the buffer count
    Wire.endTransmission();
    //read the contents
    Wire.requestFrom(KB_ADDRESS,1);
    while(Wire.available() < 1);//sit and wait for response
    //read the value
    bCount = Wire.read();
    //if there is anything available, grab events
    while(bCount > 0){
      //the buffer is magic, when you read an event, it pops them
      //all back down the list so that you just keep reading the 
      //first item until you run out
      Wire.beginTransmission(KB_ADDRESS);
      Wire.write(0x04);//0x04 is the fist event
      Wire.endTransmission();
      //read the contents
      Wire.requestFrom(KB_ADDRESS,1);
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
    Wire.beginTransmission(KB_ADDRESS);
    Wire.write(0x02);//0x02 is the interrupt register
    Wire.write(0xff);//setting a bit to 1 clears its interrupt 11111111
    Wire.endTransmission();
    //need to trigger the callback function for whatever app is live
    //we only need to trigger the callback if there is anything to process
    if(bRefresh){
        _action();
    }
  }
}
void CL32_keyboard::add_callback(CL32_KB_t act){
    _action = act;
}

Event CL32_keyboard::getKey(){
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
byte CL32_keyboard::eventCount(){
    return _eventCount;
}