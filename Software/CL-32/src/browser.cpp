#include "CL32.h"
#include <Arduino.h> 

byte iTab;


//callback function for deciding what to do with keyboard events
void browser_keys(){
    bool goBack = false;
    for(byte i = _keys.eventCount();i>0;i--){
        Event eTemp = _keys.getKey();
        if(eTemp.keyDown){
            if(!eTemp.isChar){
                if(eTemp.keyData==KB_DOWN){
                    if(iTab%2==0){
                        _code.iFol++;
                        if(_code.iFol>_code.iFolders-1){
                            _code.iFol=_code.iFolders-1;
                        }
                        Serial.println(_code.iFol);
                        sprintf(_code.filePath,"%s","");
                        Serial.println(_code.FolderList[_code.iFol].name);
                        _code.getPath(&_code.FolderList[_code.iFol]);
                        _code.listFile();
                    }
                    else{
                        _code.iFil++;
                        if(_code.iFil>_code.iFiles-1){
                            _code.iFil=_code.iFiles-1;
                        }
                    }
                }
                else if(eTemp.keyData==KB_UP){
                    if(iTab%2==0){
                        _code.iFol--;
                        if(_code.iFol<0){
                            _code.iFol=0;
                        }
                        sprintf(_code.filePath,"%s","");
                        _code.getPath(&_code.FolderList[_code.iFol]);
                        _code.listFile();
                    }
                    else{
                        _code.iFil--;
                        if(_code.iFil<0){
                            _code.iFil=0;
                        }
                    }
                }
                else if(eTemp.keyData==KB_LEFT){
                    iTab = 0;
                }
                else if(eTemp.keyData==KB_RGHT){
                    iTab = 1;
                }
                else if(eTemp.keyData==KB_RET){
                    if(iTab%2==0){
                        iTab = 1;
                    }
                    else{
                        sprintf(_code.fileName,"%s",_code.sFileList[_code.iFil]);
                        _code.readFile();
                        goBack=true;
                    }
                }
                else if(eTemp.keyData==KB_ESC){
                    goBack=true;
                }
            }
        }
    }
    if(goBack){
        if(currentApp==EDIT){
            _keys.add_callback(editor_keys);
            draw_editor(false);
        }
    }
    else{
        draw_browser(true);
    }
}
//function for drawing the file browser screen
void draw_browser(bool goFast){
    if(_code.iFolders==0){
        _code.listFolder();
        sprintf(_code.filePath,"%s","/");
        _code.getPath(&_code.FolderList[_code.iFol]);
        _code.listFile();
    }
    _screen.clearScreen(false,goFast);
    _screen.addHead("File Browser");
    if(_code.iFolders==-1&&_code.iFiles==-1){
        _screen.addText("SD Card Fail!!",100,100,true);
    }
    else{
        _screen.addLine(_screen.width()/2,iFontH+2,_screen.width()/2,_screen.height()-1,GxEPD_BLACK);
        _screen.setFont(9,true,false);
        if(iTab%2==0){
          _screen.addText("> Folders <",5,(iFontH*2.5) + 6,true);
        }
        else{
          _screen.addText("  Folders  ",5,(iFontH*2.5) + 6,true);
        }
        if(iTab%2==0){
          _screen.addText("  Files  ",(_screen.width()/2)+5,(iFontH*2.5) + 6,true);
        }
        else{
          _screen.addText("> Files <",(_screen.width()/2)+5,(iFontH*2.5) + 6,true);
        }
        int iMax, iMin;
        if(_code.iFol<(_code.iPage/2)){
          //we are still on the first page
          iMin = 0;
          iMax = min(_code.iPage,_code.iFolders);
        }
        else if(_code.iFol>(_code.iFolders-(_code.iPage/2))){
          //we are in the last page
          iMin = max(_code.iFolders-_code.iPage,0);//if there were 9 folders on a page big enough for 10, this would give us a negative
          iMax = _code.iFolders;
        }
        else{
          //we are somewhere in the middle
          iMin = _code.iFol - (_code.iPage/2);
          iMax = _code.iFol + (_code.iPage/2);
        }

        for(int f = iMin;f<iMax;f++){
            Serial.println(_code.FolderList[f].name);
          _screen.setFont(9,_code.iFol==f,false);
          if(strlen(_code.FolderList[f].name)>13){
            char shorter[15];
            sprintf(shorter,"%.13s..",_code.FolderList[f].name);
            _screen.addText(shorter,10+(iFontW*_code.FolderList[f].layer),(iFontH*4) + 6 + (iFontH * (f-iMin)),true);
          }
          else{
            _screen.addText(_code.FolderList[f].name,10+(iFontW*_code.FolderList[f].layer),(iFontH*4) + 6 + (iFontH * (f-iMin)),true);
          }
          if(_code.FolderList[f].layer>0){
            _screen.addLine(iFontW*_code.FolderList[f].layer,(iFontH*4) + (iFontH * (f-iMin)),iFontW*_code.FolderList[f].layer,(iFontH*4) + 4 + (iFontH * (f-iMin)),true);
            _screen.addLine(iFontW*_code.FolderList[f].layer,(iFontH*4) + 4 + (iFontH * (f-iMin)),4+(iFontW*_code.FolderList[f].layer),(iFontH*4) + 4 + (iFontH * (f-iMin)),true);
          }
        }
        if(_code.iFil<(_code.iPage/2)){
          //we are still on the first page
          iMin = 0;
          iMax = min(_code.iPage,_code.iFiles);
        }
        else if(_code.iFil>(_code.iFiles-(_code.iPage/2))){
          //we are in the last page
          iMin = max(_code.iFiles-_code.iPage,0);//if there were 9 files in a 10 file page, the number would be negative
          iMax = _code.iFiles;
        }
        else{
          //we are somewhere in the middle
          iMin = _code.iFil - (_code.iPage/2);
          iMax = _code.iFil + (_code.iPage/2);
        }
        for(int f = iMin;f<iMax;f++){
          _screen.setFont(9,_code.iFil==f,false);
          if(strlen(_code.sFileList[f])>15){
            char shorter[17];
            sprintf(shorter,"%.15s..",_code.sFileList[f]);
            _screen.addText(shorter,10+(_screen.width()/2),(iFontH*4) + 6 + (iFontH * (f-iMin)),true);
          }
          else{
            _screen.addText(_code.sFileList[f],10+(_screen.width()/2),(iFontH*4) + 6 + (iFontH * (f-iMin)),true);
          }
        }      
    }
    _screen.show(goFast);
}