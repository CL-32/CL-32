#include "CL32.h"
#include <Arduino.h> 

enum subMenu {RUN,OPEN,SAVE,NEW};
subMenu current;
String menuNames[] = { "Run File", "Open File", "Save File", "New File" }; 

void move_menu(bool increment){
    if(increment){
        switch (current) {
            case RUN: current =  OPEN; break;
            case OPEN: current = SAVE; break;
            case SAVE: current =  NEW; break;
            case NEW: current = RUN;
        }
    }
    else{
        switch (current) {
            case RUN: current =  NEW; break;
            case OPEN: current = RUN; break;
            case SAVE: current =  OPEN; break;
            case NEW: current = SAVE;
        }
    }
}

//callback function for deciding what to do with keyboard events
void editor_keys(){
    bool subRun = false;
    for(byte i = _keys.eventCount();i>0;i--){
        Event eTemp = _keys.getKey();
        if(eTemp.keyDown){
            if(!eTemp.isChar){
                if(isMenu==SUB){
                    if(eTemp.keyData==KB_DOWN){
                        move_menu(true);
                    }
                    else if(eTemp.keyData==KB_UP){
                        move_menu(false);
                    }
                    else if(eTemp.keyData==KB_RET){
                        subRun = true;
                    }
                }
                else{//not the submenu, lets edit
                    if(eTemp.keyData==KB_DOWN){
                        _code.moveCursor(1,'S');
                    }
                    else if(eTemp.keyData==KB_UP){
                        _code.moveCursor(1,'N');
                    }
                    if(eTemp.keyData==KB_LEFT){
                        _code.moveCursor(1,'W');
                    }
                    else if(eTemp.keyData==KB_RGHT){
                        _code.moveCursor(1,'E');
                    }
                    if(eTemp.keyData==KB_PG_DN){
                        _code.moveCursor(12,'S');
                    }
                    else if(eTemp.keyData==KB_PG_UP){
                        _code.moveCursor(12,'N');
                    }
                    if(eTemp.keyData==KB_HOME){
                        _code.moveCursor(999,'W');
                    }
                    else if(eTemp.keyData==KB_END){
                        _code.moveCursor(999,'E');
                    }
                    else if(eTemp.keyData==KB_RET){
                        _code.putChar(10,_code.codeLines[_code.iRow][_code.iCol].pos);
                        if(_code.bCRLF){
                            _code.putChar(13,_code.codeLines[_code.iRow][_code.iCol].pos);
                        }
                    }
                    else if(eTemp.keyData==KB_BSP){
                        //_code.moveCursor(1,'W');
                        _code.putChar(KB_BSP,_code.codeLines[_code.iRow][_code.iCol].pos-1);
                    }
                }
            }
            else{
                if(isMenu==OFF){
                    _code.putChar(eTemp.keyData,_code.codeLines[_code.iRow][_code.iCol].pos);
                    _code.getWindow();
                    _code.moveCursor(1,'E');
                }
            }
        }
    }
    if(subRun){
        isMenu = OFF;//take it out of menu mode
        if(current==OPEN){
            _keys.add_callback(browser_keys);
            draw_browser(false);
        }
    }
    else if(isMenu==ON){
        _keys.add_callback(menu_keys);
        draw_menu(false);
    }
    else{
        draw_editor(true);
    }
}

//function for drawing the editor screen
void draw_editor(bool goFast){
    if(isMenu==SUB){
        _screen.addBox(1,16,140,90,false,true);
        _screen.addBox(1,16,140,90,true,false);
        for(byte i = 0;i<4;i++){
            if(i==current){
                _screen.setFont(12,true,false);
            }
            else{
                _screen.setFont(12,false,false);
            }
            _screen.addText(menuNames[i],8,36+(i*20),true);
        }
    }
    else{
        _screen.clearScreen(false,goFast);
        _screen.setFont(12,true,false);
        if(strlen(_code.getFilename())==0){
            _screen.addHead("Editor");
            _screen.addText("Please Open A File To Edit",40,100,true);
        }
        else{
            _screen.addHead(_code.getFilename());
            //reload the current view from the big file
            _code.getWindow();
            //draw the edit screen
            for(int y = 0;y<windowH;y++){
                for(int x = 0;x<windowW;x++){
                    _screen.setFont(9,false,false);
                    if(x==_code.iCol-_code.iWindowX && y==_code.iRow-_code.iWindowY){
                        //we are on the same position as the cursor, lets do things different
                        _screen.addBox(x*iFontW,(iFontH) + (y * iFontH) + 5,iFontW,iFontH,true,true);
                        _screen.addText(_code.windowChar(x,y),x*iFontW,(iFontH*2) + (y * iFontH) + 4,false);
                    }
                    else{
                        //normal text
                        _screen.addText(_code.windowChar(x,y),x*iFontW,(iFontH*2) + (y * iFontH) + 4,true);
                    }
                }
            }
        }
    }
    _screen.show(goFast);
}