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
  iLoop++;
  if(not digitalRead(CL32_int)){
    //the interrupt is shared, so we need to see who is shouting
    _keys.read();
    _time.read();
    _batt.read();
  }
  if(iLoop==5000){//stagger so it doesnt cause slowdown
    //refresh the time on a timer style counter
    _time.loadTime();
  }
  if(iLoop==10000){//stagger so it doesnt cause slowdown
    //refresh the battery info on a timer style counter
    _batt.loadPower();
    iLoop = 0;//reset the counter so we dont overflow
  }
}