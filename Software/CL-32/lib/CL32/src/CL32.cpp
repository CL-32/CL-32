#include "CL32.h"
#include "Arduino.h" 

CL32_screen _screen;
CL32_keyboard _keys;
CL32_rtc _time;
CL32_file _code;

const String appNames[] = { "Editor", "Calculator", "Beeper", "Settings" }; 
MenuState isMenu = ON;              
AppList currentApp = EDIT;          
AppList newApp = EDIT;        

int iFontH = 12;
int iFontW = 12;