#ifndef CL32_keyboard_h
#define CL32_keyboard_h
#include "Arduino.h" 

const byte KB_ADDRESS = 0x34;

struct Key {
  int lower;
  int upper;
  int func;
  bool isChar;
};

//state for shift and fn usage
enum toggleState {UNPRESSED, ONEPRESS, LOCKPRESS}; 

//some keyboard things mapping to make things readable
const byte KB_SHIFT = 225;
const byte KB_RET = 40;
const byte KB_ESC = 41;
const byte KB_BSP = 42;
const byte KB_TAB = 43;
const byte KB_F1 = 58;
const byte KB_F10 = 67;
const byte KB_F11 = 68;
const byte KB_RGHT = 79;
const byte KB_LEFT = 80;
const byte KB_DOWN = 81;
const byte KB_UP = 82;

class CL32_keyboard {
	using CL32_KB_t = void (*)(void);
public:
	CL32_keyboard();
	void keyboardInit();
	byte keyboardRead();
    void add_callback(CL32_KB_t act);
private:
	toggleState _shift;
	toggleState _fn;
	Key _matrix[80];
	CL32_KB_t _action;
};
#endif