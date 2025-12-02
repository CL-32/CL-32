#include "CL32.h"
#include <Arduino.h> 

enum subMenu {RUN,OPEN,SAVE,NEW};
subMenu current;
String menuNames[] = { "Run File", "Open File", "Save File", "New File" }; 
bool askSave = false, saveYesNo = false;
bool askName = false, askDone = false;
char nameTemp[50];

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
                        if(askSave){
                            if(saveYesNo){
                                _code.saveFile();
                                askSave = false;
                                saveYesNo = false;
                            }
                            else{//if the user said no, we need to pretend there are no changes so the browser opens fine
                                _code.bModified = false;
                                askSave = false;
                                saveYesNo = false;
                            }
                        }
                        if(askName){
                            sprintf(_code.fileName,"%s",nameTemp);
                            askDone = true;
                            askName = false;
                        }
                        subRun = true;
                    }
                    else if(eTemp.keyData==KB_LEFT&&askSave){
                        saveYesNo = !saveYesNo;
                    }
                    else if(eTemp.keyData==KB_RGHT&&askSave){
                        saveYesNo = !saveYesNo;
                    }
                    else if(eTemp.keyData==KB_ESC){
                        //cancel all the sub menu stuff
                        subRun = false;
                        isMenu = OFF;
                        askSave = false;
                        askName = false;
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
                        _code.putChar(10,_code.codeLines[_code.iRow - _code.iWindowY][_code.iCol - _code.iWindowX].pos);
                        if(_code.bCRLF){
                            _code.putChar(13,_code.codeLines[_code.iRow - _code.iWindowY][_code.iCol - _code.iWindowX].pos);
                        }
                        //move down a line
                        _code.moveCursor(1,'S');
                        //then we need to be at the start of that line, in case we just broke a full line
                        _code.moveCursor(999,'W');
                    }
                    else if(eTemp.keyData==KB_BSP){
                        if(_code.iCol==0){//tack this line onto the one above
                            _code.moveCursor(1,'N');
                            _code.moveCursor(999,'E');
                        }
                        else{
                            _code.moveCursor(1,'W');
                        }
                        _code.putChar(KB_BSP,_code.codeLines[_code.iRow - _code.iWindowY][_code.iCol - _code.iWindowX].pos);
                    }
                }
            }
            else{
                if(isMenu==OFF){
                    _code.putChar(eTemp.keyData,_code.codeLines[_code.iRow - _code.iWindowY][_code.iCol - _code.iWindowX].pos);
                    _code.getWindow();
                    _code.moveCursor(1,'E');
                }
                else{
                    sprintf(nameTemp,"%s%c",nameTemp,eTemp.keyData);
                }
            }
        }
    }
    if(subRun){
        if(current==RUN){
            isMenu = OFF;//take it out of menu mode
         
            //some code here to run the codez 

            _screen.showMsg("Coming Soon....");
            delay(500);
            draw_editor(true);
        }
        else if(current==OPEN){
            if(_code.bModified){//ask to save
                if(!askSave){
                    askSave = true;
                    saveYesNo = false;
                }
                draw_editor(true);
            }
            else{
                isMenu = OFF;//take it out of menu mode
                _keys.add_callback(browser_keys);
                draw_browser(false);
            }
        }
        else if(current==SAVE){
            isMenu = OFF;//take it out of menu mode
            _code.saveFile();
            _screen.showMsg("Saved");
            delay(500);
            draw_editor(true);
        }
        else if(current==NEW){
            if(_code.bModified){//ask to save
                if(!askSave){
                    askSave = true;
                    saveYesNo = false;
                    askDone = false;
                }
                draw_editor(true);
            }
            else{ 
                if(!askDone){ 
                    if(!askName){
                        askName = true;
                        sprintf(nameTemp,"");
                    }
                    draw_editor(true);
                }
                else{
                    isMenu = OFF;
                    if(_code.fileExsist()){
                        _screen.showMsg("File Already Exsists");
                        sprintf(_code.fileName,"");
                        delay(500);
                    }
                    else{
                        _code.readFile();
                    }
                    draw_editor(true);
                }
            }
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
    if(askSave){
        _screen.yesNoDialog("Save Changes??",saveYesNo);
    }
    else if(askName){
        _screen.inputDialog("Enter a Filename",nameTemp);
    }
    else{
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
}