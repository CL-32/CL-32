#ifndef CL32_h
#define CL32_h
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "CL32_file.h"
#include "CL32_keyboard.h"
#include "CL32_power.h"
#include "CL32_RTC.h"
#include "CL32_screen.h"

extern CL32_screen _screen;
extern CL32_keyboard _keys;
extern CL32_rtc _time;

enum MenuState {ON,OFF,SUB};      
extern MenuState isMenu;          

enum AppList {EDIT,CALC,BEEP,SET};      
extern AppList currentApp;         
extern const String appNames[];  


//callback function for deciding what the menu does with the keyboard events
void menu_keys();
//function for drawing the menu
void draw_menu();
//callback function for deciding what to do with keyboard events
void editor_keys();
//function for drawing the editor screen
void draw_editor();


#endif