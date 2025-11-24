#include <Arduino.h>
#include "CL32.h"

int iLoop;

void setup() {
    _screen.init();
    _keys.init();
    _time.init();
    _batt.init();
    _keys.add_callback(menu_keys);
    Serial.begin(115200);
    draw_menu(false);
    iLoop = 0;
}

void loop() {

  if(not digitalRead(CL32_int)){
    _keys.read();

  }
  if(iLoop==5000){
    //refresh the time on a timer style counter
    _time.loadTime();
  }
  if(iLoop==10000){
    //refresh the battery info on a timer style counter
    _batt.loadPower();
    iLoop = 0;//reset the counter so we dont overflow
  }
}