#include <Arduino.h>
#include "CL32.h"

int refreshCount;
bool i2c_int;

void letsGo(){
  i2c_int = true;
}

void setInterrupt(){
  //reset the correct interrupt after we have woken from sleep
  if(doSleep){
    if (esp_sleep_enable_ext0_wakeup(GPIO_NUM_3, 0) != ESP_OK) {
        _screen.showMsg("GPIO interrupt failed");
    }
  }
  else {
    i2c_int = false;
    attachInterrupt(CL32_int ,letsGo, FALLING );
  }
}

void setup() {
    _screen.init();
    _keys.init();
    _time.init();
    _batt.init();
    _keys.add_callback(menu_keys);
    //Serial.begin(115200);
    draw_menu(fastAppSwitch);
    refreshCount = 0;
    if (esp_sleep_enable_timer_wakeup(60000000) != ESP_OK) {
        _screen.showMsg("TIMER interrupt failed");
    }
    setInterrupt();
    i2c_int = false;//force the non sleep interrupt for the first run (we dont sleep until something happens)
    attachInterrupt(CL32_int ,letsGo, FALLING );
    load_set();
}

void loop() {
  if(i2c_int || esp_sleep_get_wakeup_cause()==ESP_SLEEP_WAKEUP_EXT0){
    //the interrupt is shared, so we need to see who is shouting
    _keys.read();
    _time.read();
    _batt.read();
    if(doSleep){
      if(saverTime>0 && refreshCount>=saverTime){//re-enable the timer
        if ( esp_sleep_enable_timer_wakeup(60000000) != ESP_OK) {
            _screen.showMsg("TIMER interrupt failed");
        }
      }
      esp_light_sleep_start();
      setInterrupt();
    } 
    refreshCount = 0;
  }
  if(esp_sleep_get_wakeup_cause()==ESP_SLEEP_WAKEUP_TIMER){
    if(saverTime>0 && refreshCount>=saverTime){
      _screen.drawSleep();
      esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);//stop the wakey wakey
    }
    else{
      _screen.refreshStatus();
    }
    esp_light_sleep_start(); //no need to check the variable here, this will only happen on timer wake which wont happen if sleep never happens
    refreshCount++;
  }
}