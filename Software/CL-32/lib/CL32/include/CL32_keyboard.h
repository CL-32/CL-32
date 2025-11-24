#ifndef CL32_keyboard_h
#define CL32_keyboard_h
#include "CL32_pins.h"
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
extern byte KB_SHIFT;
extern byte KB_RET;
extern byte KB_ESC;
extern byte KB_BSP;
extern byte KB_TAB;
extern byte KB_FN;
extern byte KB_FILE;
extern byte KB_MENU;
extern byte KB_RGHT;
extern byte KB_LEFT;
extern byte KB_DOWN;
extern byte KB_UP;
extern byte KB_PG_UP;
extern byte KB_PG_DN;
extern byte KB_HOME;
extern byte KB_END;

struct Event {
  int keyData;
  bool keyDown;
  bool isChar;
};

class CL32_keyboard {
	using CL32_KB_t = void (*)(void);
public:
	CL32_keyboard();
	void init();
	void read();
    void add_callback(CL32_KB_t act);
	Event getKey();
	byte eventCount();
	toggleState _shift;
	toggleState _fn;
private:
	Key _matrix[80];
	CL32_KB_t _action;
	Event _eventLog[20];
	byte _eventCount;
};
#endif