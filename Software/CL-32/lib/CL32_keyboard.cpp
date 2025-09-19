#include <Arduino.h>
#include <Wire.h>
#include "CL32.h"
#include "CL32_keyboard.h"


CL32_keyboard::CL32_keyboard() {
    KB_eventCount= 0;
    //start the i2c
    Wire.begin(CL32_sda,CL32_scl);
    //could make a keymap in here
    //i guess this could be loaded in from a file...
    _matrix[0]  = {KB_TAB,  KB_TAB, KB_TAB, false};
    _matrix[1]  = {'q',     'Q',    '!',    true};
    _matrix[2]  = {'w',     'W',    '"',    true};
    _matrix[3]  = {'e',     'E',	'@',    true};
    _matrix[4]  = {'r',     'R',    '£',    true};
    _matrix[5]  = {'t',     'T',	'$',    true};
    _matrix[6]  = {'y',     'Y',	'%',    true};
    _matrix[7]  = {'u',     'U',	'^',    true};
    _matrix[8]  = {'i',     'I',	'&',    true};
    _matrix[9]  = { 0 ,	     0 ,    0 ,     0 };
    _matrix[10] = {KB_SHIFT, KB_SHIFT,KB_SHIFT , false };
    _matrix[11] = {'a',     'A',	'|',    true};
    _matrix[12] = {'s',     'S',	'#',    true};
    _matrix[13] = {'d',     'D',	',',    true};
    _matrix[14] = {'f',     'F',	'?',    true};
    _matrix[15] = {'g',     'G',	'\'',   true};
    _matrix[16] = {'h',     'H',	':',    true};
    _matrix[17] = {'j',     'J',    ';',    true};
    _matrix[18] = {'k',     'K',	'{',    true};
    _matrix[19] = { 0 ,	    0 ,	    0 ,     0 };
    _matrix[20] = {KB_F1,   KB_F1,  KB_F1,  false};
    _matrix[21] = {'z',     'Z',	' ',    true};
    _matrix[22] = {'x',     'X',	'\\',   true};
    _matrix[23] = {'c',     'C',	'~',    true};
    _matrix[24] = {' ',     ' ',	' ' ,   true};
    _matrix[25] = {' ',     ' ',	' ' ,   true};
    _matrix[26] = {'v',     'V',	'<',    true};
    _matrix[27] = {'b',     'B',	'>',    true};
    _matrix[28] = {'n',     'N',	'[',    true};
    _matrix[29] = { 0 ,	    0 ,	    0 ,     0 };
    _matrix[30] = {'=',     '=',    '=',    true };
    _matrix[31] = {'.',     '.',    '.',    true};
    _matrix[32] = {'0',     '0',    '0',    true};
    _matrix[33] = {'+',     '+',    '+',    true};
    _matrix[34] = {KB_UP,   KB_UP,  KB_UP,  false };
    _matrix[35] = {KB_RGHT, KB_RGHT,KB_RGHT,false};
    _matrix[36] = {KB_RET,  KB_RET, KB_RET, false};
    _matrix[37] = {KB_RET,  KB_RET, KB_RET, false};
    _matrix[38] = {'m',     'M',	']',    true};
    _matrix[39] = { 0 ,	    0 ,	    0 ,     0 };
    _matrix[40] = {'3',     '3',	'3',    true};
    _matrix[41] = {'2',     '2',    '2',    true};
    _matrix[42] = {'1',     '1',    '1',    true};
    _matrix[43] = {'-',     '-',    '-',    true};
    _matrix[44] = {KB_LEFT, KB_LEFT,KB_LEFT,false};
    _matrix[45] = {KB_DOWN, KB_DOWN,KB_DOWN,false};
    _matrix[46] = {KB_ESC,  KB_ESC, KB_ESC, false};
    _matrix[47] = {KB_RET,  KB_RET, KB_RET, false};
    _matrix[48] = {'l',     'L',	'}',    true};
    _matrix[49] = { 0 ,	    0 ,	    0 ,     0 };
    _matrix[50] = {'6',     '6',    '6',    true};
    _matrix[51] = {'5',     '5',    '5',    true};
    _matrix[52] = {'4',     '4',    '4',    true};
    _matrix[53] = {'*',     '*',    '*',    true};
    _matrix[54] = {KB_F10,  KB_F10, KB_F10, false};
    _matrix[55] = {KB_F11,  KB_F11, KB_F11, false};
    _matrix[56] = {KB_BSP,  KB_BSP, KB_BSP, false};
    _matrix[57] = {'p',     'P',    'p',    true};
    _matrix[58] = {'o',     'O',    'o',    true};
    _matrix[59] = { 0 ,	    0 ,	    0 ,     0 };
    _matrix[60] = {'9',     '9',    '9',    true};
    _matrix[61] = {'8',     '8',    '8',    true};
    _matrix[62] = {'7',     '7',    '7',    true};
    _matrix[63] = {'/',     '/',    '/',    true};    
\

    
}
void CL32_keyboard::keyboardInit(){
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

byte CL32_keyboard::keyboardRead(){
    
  byte bCount, bEvent, bEventCode, bStatus;
  bool bEventStat;
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
      if(_matrix[bEventCode].lower==KB_SHIFT){
        if(_shift==UNPRESSED){//we are not at lock yet
            _shift=ONEPRESS;
        }
        else if(_shift==ONEPRESS){
            _shift=LOCKPRESS;
        }
        else{//its locked, a further press will reset
            _shift=UNPRESSED;
        }
      }
      else if(_matrix[bEventCode].lower==KB_F1){
        if(_fn==UNPRESSED){//we are not at lock yet
            _fn=ONEPRESS;
        }
        else if(_fn==ONEPRESS){
            _fn=LOCKPRESS;
        }
        else{//if its locked then a further press will reset
            _fn=UNPRESSED;
        }
      }
      else{
        //add the event to the global register
        if(_fn!=UNPRESSED){
            KB_eventLog[KB_eventCount] = {_matrix[bEventCode].func,bEventStat,_matrix[bEventCode].isChar};
            if(_fn==ONEPRESS){//we are not at lock yet
                _fn==UNPRESSED;
            }
        }
        else if(_shift!=UNPRESSED){
            KB_eventLog[KB_eventCount] = {_matrix[bEventCode].upper,bEventStat,_matrix[bEventCode].isChar};
            if(_shift==ONEPRESS){//we are not at lock yet
                _shift=UNPRESSED;
            }
        }
        else{
            KB_eventLog[KB_eventCount] = {_matrix[bEventCode].lower,bEventStat,_matrix[bEventCode].isChar};
        }
        KB_eventCount++;
      }
    }//reset the fact there have been keys pressed
    Wire.beginTransmission(KB_ADDRESS);
    Wire.write(0x02);//0x02 is the interrupt register
    Wire.write(0xff);//setting a bit to 1 clears its interrupt 11111111
    Wire.endTransmission();
    //need to trigger the callback function for whatever app is live
    _action();
  }
}
void CL32_keyboard::add_callback(CL32_KB_t act){
    _action = act;
}