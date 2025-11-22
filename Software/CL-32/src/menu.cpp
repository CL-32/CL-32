#include <Arduino.h> 
#include "CL32.h"

void change_app(bool increment){
    if(increment){
        switch (newApp) {
            case EDIT: newApp =  CALC; break;
            case CALC: newApp = BEEP; break;
            case BEEP: newApp =  SET; break;
            case SET: newApp = EDIT;
        }
    }
    else{
        switch (newApp) {
            case EDIT: newApp =  SET; break;
            case CALC: newApp = EDIT; break;
            case BEEP: newApp =  CALC; break;
            case SET: newApp = BEEP;
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
                else if(eTemp.keyData==KB_RET){
                    currentApp = newApp;
                    isMenu = OFF;//take it out of menu mode
                }
                else if(eTemp.keyData==KB_TAB){
                    _screen.showMsg("Testing this message box");
                    delay(500);
                }
            }
            //dont really need an else here, if its not the 3 char's we aee interested in, then we dont neeed that key
        }
    }
    if(isMenu==OFF){        
        if(currentApp==EDIT){
            _keys.add_callback(editor_keys);
            draw_editor(false);
        }
    }
    else{
        draw_menu(true);
    }
} 

//function for drawing the menu
void draw_menu(bool goFast){
    _screen.clearScreen(false,goFast);
    _screen.addHead("Menu");
    for(byte i = 0;i<4;i++){
        if(i==newApp){
            _screen.setFont(12,true,false);
        }
        else{
            _screen.setFont(12,false,false);
        }
        _screen.addText(appNames[i],20,40+(i*20),true);
    }
    _screen.show(goFast);
}