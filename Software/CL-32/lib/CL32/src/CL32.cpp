#include "CL32.h"
#include "Arduino.h" 

CL32_screen _screen;
CL32_keyboard _keys;
CL32_rtc _time;
CL32_file _code;
CL32_power _batt;
//enum AppList {EDIT,CALC,TODO,CAL,AUTH,READ,SET};    
const String appNames[] = { "Editor", "Calculator", "To-Do List","Calendar","2FA Auth","e-Reader", "Settings" }; 
MenuState isMenu = ON;              
AppList currentApp = EDIT;          
AppList newApp = EDIT;        

int iFontH = 12;
int iFontW = 12;

//some system settings
byte saverTime = 5;
bool doSleep = true;
bool fastAppSwitch = false;