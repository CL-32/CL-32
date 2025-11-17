#include <Arduino.h>
#include "CL32.h"


void setup() {
    _screen.init();
    _keys.init();
    _time.init();
    _keys.add_callback(menu_keys);
    Serial.begin(115200);
    draw_menu();
}

void loop() {

  if(not digitalRead(CL32_int)){
    _keys.read();

  }
}