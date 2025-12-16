#include "CL32.h"
#include <Arduino.h> 
#include <Preferences.h>

byte iCol;
struct tm setTime;
enum subMenu {LOAD,SAVE};
subMenu currentSet;
String SetMenuNames[] = { "Reload", "Save" }; 
Preferences CL32_settings;

void load_set(){
    CL32_settings.begin("CL32",false);
    if(CL32_settings.isKey("SaverTime")){
        saverTime = CL32_settings.getInt("SaverTime");
    }
    if(CL32_settings.isKey("DoSleep")){
        doSleep = CL32_settings.getBool("DoSleep");
    }
    if(CL32_settings.isKey("FastAppSwitch")){
        fastAppSwitch = CL32_settings.getBool("FastAppSwitch");
    }
    CL32_settings.end();
}

void save_set(){
    CL32_settings.begin("CL32",false);
    CL32_settings.putInt("SaverTime",saverTime);
    CL32_settings.putBool("DoSleep",doSleep);
    CL32_settings.putBool("FastAppSwitch",fastAppSwitch);
    CL32_settings.end();
}

void move_menu_set(bool increment){
    if(increment){
        switch (currentSet) {
            case LOAD: currentSet =  SAVE; break;
            case SAVE: currentSet = LOAD;
        }
    }
    else{
        switch (currentSet) {
            case SAVE: currentSet =  LOAD; break;
            case LOAD: currentSet = SAVE;
        }
    }
}

void change_value(byte stepSize,bool adding){
    if(iCol==0){
        if(adding){
            setTime.tm_hour += stepSize;
            if(setTime.tm_hour>23){
                setTime.tm_hour=0;
            }
        }
        else{
            setTime.tm_hour -= stepSize;
            if(setTime.tm_hour<0){
                setTime.tm_hour=23;
            }
        }
    }
    else if(iCol==1){
        if(adding){
            setTime.tm_min += stepSize;
            if(setTime.tm_min>59){
                setTime.tm_min=0;
            }
        }
        else{
            setTime.tm_min -= stepSize;
            if(setTime.tm_min<0){
                setTime.tm_min=59;
            }
        }
    }
    else if(iCol==2){
        if(adding){
            setTime.tm_year += stepSize;
        }
        else{
            setTime.tm_year -= stepSize;
        }
    }
    else if(iCol==3){
        if(adding){
            setTime.tm_mon += stepSize;
            if(setTime.tm_mon>12){
                setTime.tm_mon=1;
            }
        }
        else{
            setTime.tm_mon -= stepSize;
            if(setTime.tm_mon<1){
                setTime.tm_mon=12;
            }
        }
    }
    else if(iCol==4){
        if(adding){
            setTime.tm_mday += stepSize;
            if(setTime.tm_mday>31){
                setTime.tm_mday=1;
            }
        }
        else{
            setTime.tm_mday -= stepSize;
            if(setTime.tm_mday<1){
                setTime.tm_mday=31;
            }
        }
    }
    else if(iCol==5){
        if(adding){
            saverTime += stepSize;
        }
        else{
            saverTime-= stepSize;
        }
        if(saverTime<0){
            saverTime=0;
        }
    }
    else if(iCol==6){
        doSleep = !doSleep;
    }
    else if(iCol==7){
        fastAppSwitch = !fastAppSwitch;
    }
}

//callback function for deciding what to do with keyboard events
void set_keys(){
    
    for(byte i = _keys.eventCount();i>0;i--){
        Event eTemp = _keys.getKey();
        if(eTemp.keyDown){
            if(!eTemp.isChar){
                if(isMenu==SUB){
                    if(eTemp.keyData==KB_DOWN){
                        move_menu_set(true);
                    }
                    else if(eTemp.keyData==KB_UP){
                        move_menu_set(false);
                    }
                    else if(eTemp.keyData==KB_RET){
                        if(currentSet==SAVE){
                            _time.saveTime(setTime);
                            save_set();
                            isMenu=OFF;
                        }
                        else if(currentSet==LOAD){
                            _time.loadTime();
                            setTime=_time.getTimeStruct();
                            load_set();
                            isMenu=OFF;
                        }
                    }
                    else if(eTemp.keyData==KB_ESC){
                        isMenu=OFF;
                    }
                }
                else{
                    if(eTemp.keyData==KB_LEFT){
                        if(iCol==0){
                            iCol=7;
                        }
                        else{
                            iCol--;
                        }
                    }
                    else if(eTemp.keyData==KB_RGHT){
                        if(iCol==7){
                            iCol=0;
                        }
                        else{
                            iCol++;
                        }
                    }
                    else if(eTemp.keyData==KB_UP){
                        change_value(1,true);
                    }
                    else if(eTemp.keyData==KB_DOWN){
                        change_value(1,false);
                    }
                    else if(eTemp.keyData==KB_PG_UP){
                        change_value(10,true);
                    }
                    else if(eTemp.keyData==KB_PG_DN){
                        change_value(10,false);
                    }
                    else if(eTemp.keyData==KB_RET){
                    }
                }
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
        draw_set(true);
    }


}
//function for drawing the file browser screen
void draw_set(bool goFast){
    
    if(isMenu==SUB){
        _screen.addBox(1,16,120,50,false,true);
        _screen.addBox(1,16,120,50,true,false);
        for(byte i = 0;i<2;i++){
            if(i==currentSet){
                _screen.setFont(12,true,false);
            }
            else{
                _screen.setFont(12,false,false);
            }
            _screen.addText(SetMenuNames[i],8,36+(i*20),true);
        }
    }
    else{
        if(setTime.tm_year==0&&setTime.tm_mon==0&&setTime.tm_mday==0){
            setTime=_time.getTimeStruct();
        }
        _screen.clearScreen(false,goFast);
        _screen.addHead("Settings");
        _screen.addText("Hour",30,30,true);
        _screen.addText("Minute",90,30,true);
        _screen.addText("Year",172,30,true);
        _screen.addText("Month",240,30,true);
        _screen.addText("Day",305,30,true);
        _screen.addText("Screensaver",10,100,true);
        _screen.addText("Do Sleep",150,100,true);
        _screen.addText("Fast Refresh",250,100,true);
        char dateTime[50];
        sprintf(dateTime, "%02d : %02d  %04d  %02d  %02d", setTime.tm_hour, setTime.tm_min, setTime.tm_year+1900, setTime.tm_mon, setTime.tm_mday);
        _screen.setFont(12,true,false);
        _screen.addText(dateTime,40,60,true);
        sprintf(dateTime,"%d min",saverTime);
        _screen.addText(dateTime,35,130,true);
        _screen.addText(doSleep?"Yes":"No",170,130,true);
        _screen.addText(fastAppSwitch?"Yes":"No",300,130,true);
        int x, y1,y2;
        switch (iCol){
            case 0: x=50;y1=40;y2=70; break;
            case 1: x=120;y1=40;y2=70; break;
            case 2: x=190;y1=40;y2=70; break;
            case 3: x=260;y1=40;y2=70; break;
            case 4: x=315;y1=40;y2=70; break;
            case 5: x=50;y1=110;y2=140; break;
            case 6: x=180;y1=110;y2=140; break;
            case 7: x=310;y1=110;y2=140; break;
        }
        _screen.addLine(x,y1,x+5,y1-5,true);
        _screen.addLine(x+5,y1-5,x+10,y1,true);
        _screen.addLine(x,y1,x+10,y1,true);
        
        _screen.addLine(x,y2,x+5,y2+5,true);
        _screen.addLine(x+5,y2+5,x+10,y2,true);
        _screen.addLine(x,y2,x+10,y2,true);

        sprintf(dateTime, "%04dmv - %02d%% - %s", _batt.getVoltage(),_batt.getPercent(),_time.getUptimeText());
        _screen.setFont(9,false,false);
        _screen.addText(dateTime,50,165,true);
        
    }
    _screen.show(goFast);
}

