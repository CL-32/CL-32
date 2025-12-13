#include "CL32.h"
#include <Arduino.h> 


//callback function for deciding what to do with keyboard events
void todo_keys(){
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
        draw_menu(fastAppSwitch);
    }
    else{
        draw_todo(true);
    }


}
//function for drawing the file browser screen
void draw_todo(bool goFast){
    _screen.clearScreen(false,goFast);
    _screen.addHead("To-Do List");

    //draw some stuff
    
    _screen.show(goFast);

}