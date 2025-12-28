#ifndef CL32_h
#define CL32_h
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "CL32_file.h"
#include "CL32_keyboard.h"
#include "CL32_power.h"
#include "CL32_test.h"
#include "CL32_RTC.h"
#include "CL32_screen.h"
#include <Preferences.h>

extern CL32_screen _screen;
extern CL32_keyboard _keys;
extern CL32_rtc _time;
extern CL32_file _code;
extern CL32_power _batt;
extern CL32_test _test;

enum MenuState {ON,OFF,SUB};      
extern MenuState isMenu;          

enum AppList {EDIT,CALC,TODO,CAL,AUTH,READ,SET};      
extern AppList currentApp, newApp;         
extern const String appNames[];  

extern int iFontH;
extern int iFontW;

//callback function for deciding what the menu does with the keyboard events
void menu_keys();
//function for drawing the menu
void draw_menu(bool goFast);
//callback function for deciding what to do with keyboard events
void editor_keys();
//function for drawing the editor screen
void draw_editor(bool goFast);
//callback function for deciding what to do with keyboard events
void browser_keys();
//function for drawing the file browser screen
void draw_browser(bool goFast);
//callback function for deciding what to do with keyboard events
void calc_keys();
//function for drawing the file browser screen
void draw_calc(bool goFast);
//callback function for deciding what to do with keyboard events
void todo_keys();
//function for drawing the file todo screen
void draw_todo(bool goFast);
//callback function for deciding what to do with keyboard events
void cal_keys();
//function for drawing the file calendar screen
void draw_cal(bool goFast);
//callback function for deciding what to do with keyboard events
void twofa_keys();
//function for drawing the file 2fa screen
void draw_twofa(bool goFast);
//callback function for deciding what to do with keyboard events
void reader_keys();
//function for drawing the file todo screen
void draw_reader(bool goFast);




//callback function for deciding what to do with keyboard events
void set_keys();
//function for drawing the file settings screen
void draw_set(bool goFast);
//make the loading of settings global
void load_set();

void prompt_keys();


//some system settings
extern byte saverTime;
extern bool doSleep;
extern bool fastAppSwitch;
extern bool blockSleep;

extern Preferences CL32_settings;

#endif