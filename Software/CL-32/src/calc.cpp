#include "CL32.h"
#include <Arduino.h> 


//callback function for deciding what to do with keyboard events
void calc_keys(){
    for(byte i = _keys.eventCount();i>0;i--){
        Event eTemp = _keys.getKey();
        if(eTemp.keyDown){
            if(!eTemp.isChar){

            }
            else{

            }
        }
    }
    if(isMenu==ON){
        _keys.add_callback(menu_keys);
        draw_menu(false);
    }
    else{
        draw_calc(true);
    }


}
//function for drawing the file browser screen
void draw_calc(bool goFast){
    _screen.clearScreen(false,goFast);
    _screen.addHead("Calculator");

    //draw some stuff
    
    _screen.show(goFast);

}