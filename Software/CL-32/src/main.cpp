#include <Arduino.h>
#include "CL32.h"

int refreshCount;

void setup() {
    _screen.init();
    _keys.init();
    _time.init();
    _batt.init();
    _keys.add_callback(menu_keys);
    Serial.begin(115200);
    draw_menu(fastAppSwitch);
    refreshCount = 0;
    esp_err_t result;
    result = esp_sleep_enable_ext0_wakeup(GPIO_NUM_3, 0); 
    if (result != ESP_OK) {
        _screen.showMsg("GPIO interrupt failed");
    }
    result = esp_sleep_enable_timer_wakeup(60000000);//60 seconds
    if (result != ESP_OK) {
        _screen.showMsg("TIMER interrupt failed");
    }
    load_set();
}

void loop() {
  if(not digitalRead(CL32_int)){
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