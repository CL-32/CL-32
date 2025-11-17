#include "CL32.h"
#include <Arduino.h> 

//callback function for deciding what to do with keyboard events
void editor_keys(){
    for(byte i = _keys.eventCount();i>0;i--){
        Event eTemp = _keys.getKey();
        if(eTemp.keyDown){
            if(!eTemp.isChar){
                if(eTemp.keyData==KB_F11){
                    //selecting a menu item is kinda the same as pressing the menu button agagin
                    isMenu = ON;
                }
            }
        }
    }
    if(isMenu==ON){
        _keys.add_callback(menu_keys);
        draw_menu();
    }
    else{
        draw_editor();
    }
}

//function for drawing the editor screen
void draw_editor(){
    _screen.clearScreen(false);
    _screen.setFont(12,true,false);
    _screen.addHead("Editor");
    _screen.show(false);
}