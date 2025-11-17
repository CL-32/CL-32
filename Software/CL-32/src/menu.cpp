#include <Arduino.h> 
#include "CL32.h"

void change_app(bool increment){
    if(increment){
        switch (currentApp) {
            case EDIT: currentApp =  CALC; break;
            case CALC: currentApp = BEEP; break;
            case BEEP: currentApp =  SET; break;
            case SET: currentApp = EDIT;
        }
    }
    else{
        switch (currentApp) {
            case EDIT: currentApp =  SET; break;
            case CALC: currentApp = EDIT; break;
            case BEEP: currentApp =  CALC; break;
            case SET: currentApp = BEEP;
        }
    }
}

//callback function for deciding what the menu does with the keyboard evenrs
void menu_keys(){
    for(byte i = _keys.eventCount();i>0;i--){
        Event eTemp = _keys.getKey();
        if(eTemp.keyDown){
            if(!eTemp.isChar){
                if(eTemp.keyData==KB_DOWN){
                    change_app(true);
                }
                else if(eTemp.keyData==KB_UP){
                    change_app(false);
                }
                else if(eTemp.keyData==KB_F11||eTemp.keyData==KB_RET){
                    //selecting a menu item is kinda the same as pressing the menu button agagin
                    isMenu = OFF;
                }
            }
            //dont really need an else here, if its not the 3 char's we aee interested in, then we dont neeed that key
        }
    }
    if(isMenu==OFF){        
        if(currentApp==EDIT){
            _keys.add_callback(editor_keys);
            draw_editor();
        }
    }
    else{
        draw_menu();
    }
} 

//function for drawing the menu
void draw_menu(){
    _screen.clearScreen(false);
    _screen.setFont(12,true,false);
    _screen.addHead("Menu");
    for(byte i = 0;i<4;i++){
        if(i==currentApp){
            _screen.setFont(12,true,false);
        }
        else{
            _screen.setFont(12,false,false);
        }
        _screen.addText(appNames[i],20,40+(i*20),true);
    }
    _screen.show(false);
}