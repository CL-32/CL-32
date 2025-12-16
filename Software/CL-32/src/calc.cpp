#include "CL32.h"
#include <Arduino.h> 

String calcInput = "";

struct history{
    String theSum;
    float theAnswer;
};

history sumList[5];
bool sumDone = false;


float doMath(String sumString){
    byte symbPos;
    //using BODMAS
    //Brackts ToDo
    //Subtract bringing up the rear
    if(sumString.indexOf('-')!=-1){//we have a negative
        symbPos = sumString.indexOf('-');
        return doMath(sumString.substring(0,symbPos)) - doMath(sumString.substring(symbPos+1));
    }
    //Addition now
    else if(sumString.indexOf('+')!=-1){//we have a addition
        symbPos = sumString.indexOf('+');
        return doMath(sumString.substring(0,symbPos)) + doMath(sumString.substring(symbPos+1));
    }
    //Mulitply is next
    else if(sumString.indexOf('*')!=-1){//we have a multiply
        symbPos = sumString.indexOf('*');
        return doMath(sumString.substring(0,symbPos)) * doMath(sumString.substring(symbPos+1));
    }
    //Divide things
    else if(sumString.indexOf('/')!=-1){//we have a divide
        symbPos = sumString.indexOf('/');
        return doMath(sumString.substring(0,symbPos)) / doMath(sumString.substring(symbPos+1));
    }
    //Of for the power of
    else if(sumString.indexOf('^')!=-1){//we have a power
        symbPos = sumString.indexOf('^');
        return pow(doMath(sumString.substring(0,symbPos)),doMath(sumString.substring(symbPos+1)));
    } 
    else{
        return sumString.toFloat();
    }

}

void equals(){
    //log the history
    for(int i=0;i<4;i++){
        sumList[i] = sumList[i+1];
    }
    sumList[4].theSum = calcInput;
    //process the sum
    sumList[4].theAnswer = doMath(calcInput);
    //show the answer
    char sTemp[30];
    sprintf(sTemp,"%g",sumList[4].theAnswer);
    calcInput = sTemp;
    sumDone = true;
}


//callback function for deciding what to do with keyboard events
void calc_keys(){
    for(byte i = _keys.eventCount();i>0;i--){
        Event eTemp = _keys.getKey();
        if(eTemp.keyDown){
            if(!eTemp.isChar){
                if(eTemp.keyData==KB_RET){
                    equals();
                }
                else if(eTemp.keyData==KB_BSP){
                    if(calcInput.length()>0){
                        calcInput = calcInput.substring(0,calcInput.length()-1);
                    }
                }
            }
            else{
                if(eTemp.keyData=='='){
                    equals();
                }
                else{
                    if(sumDone){//we have an answer on the screen, we need to do things differently
                        if(eTemp.keyData=='+'||eTemp.keyData=='-'||eTemp.keyData=='*'||eTemp.keyData=='/'||eTemp.keyData=='^'){
                            //if we have pressed a symbol, we can add it on to what is already there
                            calcInput.concat(char(eTemp.keyData));
                        }
                        else{
                            //we are typing a number, lets add that to a fresh sum..
                            calcInput = char(eTemp.keyData);
                        }
                        sumDone = false;
                    }
                    else{//just add more...
                        calcInput.concat(char(eTemp.keyData));
                    }
                }
            }
        }
    }
    if(isMenu==ON){
        _keys.add_callback(menu_keys);
        draw_menu(fastAppSwitch);
    }
    else{
        draw_calc(true);
    }


}
//function for drawing the file browser screen
void draw_calc(bool goFast){
    _screen.clearScreen(false,goFast);
    _screen.addHead("Calculator");
    char dispTemp[50];
    for(byte i = 0;i<5;i++){
        _screen.setFont(12,false,false);
        if(sumList[i].theSum.length() > 0){
            sprintf(dispTemp,"%s = %g",sumList[i].theSum,sumList[i].theAnswer);
        }
        else{
            sprintf(dispTemp,"");
        }
        _screen.addText(dispTemp,8,36+(i*20),true);
    }
    _screen.setFont(18,true,false);
    _screen.addText(calcInput,10,160,true);
    
    _screen.show(goFast);

}