#include "CL32.h"
#include <Arduino.h> 

struct ToDoData{
    bool isTicked;
    String itemText;
};

ToDoData ToDoList[50];
byte ToDoItem = 0;

enum subMenu {LOAD,SAVE};
subMenu currentToDo;
String ToDoMenuNames[] = { "Load", "Save" }; 

void load_ToDo(){
    CL32_settings.begin("CL32",false);
    if(CL32_settings.isKey("TODO-LIST")){
        size_t lenToDo = CL32_settings.getBytesLength("TODO-LIST");
        CL32_settings.getBytes("TODO-LIST",&ToDoList,lenToDo);
    }
    CL32_settings.end();
}

void save_ToDo(){
    CL32_settings.begin("CL32",false);
    CL32_settings.putBytes("TODO-LIST",&ToDoList,sizeof(ToDoList));
    CL32_settings.end();
}
void move_menu_ToDo(bool increment){
    if(increment){
        switch (currentToDo) {
            case LOAD: currentToDo =  SAVE; break;
            case SAVE: currentToDo = LOAD;
        }
    }
    else{
        switch (currentToDo) {
            case SAVE: currentToDo =  LOAD; break;
            case LOAD: currentToDo = SAVE;
        }
    }
}


//callback function for deciding what to do with keyboard events
void todo_keys(){
    for(byte i = _keys.eventCount();i>0;i--){
        Event eTemp = _keys.getKey();
        if(eTemp.keyDown){
            if(!eTemp.isChar){
                if(isMenu==SUB){
                    if(eTemp.keyData==KB_DOWN){
                        move_menu_ToDo(true);
                    }
                    else if(eTemp.keyData==KB_UP){
                        move_menu_ToDo(false);
                    }
                    else if(eTemp.keyData==KB_RET){
                        if(currentToDo==SAVE){
                            save_ToDo();
                            _screen.showMsg("Saved");
                            delay(500);
                            isMenu=OFF;
                        }
                        else if(currentToDo==LOAD){
                            load_ToDo();
                            _screen.showMsg("Loaded");
                            delay(500);
                            isMenu=OFF;
                        }
                    }
                    else if(eTemp.keyData==KB_ESC){
                        isMenu=OFF;
                    }
                }
                else{
                    if(eTemp.keyData==KB_UP){
                        if(ToDoItem>0) ToDoItem--;
                    }
                    else if(eTemp.keyData==KB_DOWN){
                        if(ToDoItem<49) ToDoItem++;
                    }
                    else if(eTemp.keyData==KB_RET){
                        ToDoList[ToDoItem].isTicked = !ToDoList[ToDoItem].isTicked;
                    }
                    else if(eTemp.keyData==KB_BSP){
                        if(ToDoList[ToDoItem].itemText.length()>0){
                            ToDoList[ToDoItem].itemText = ToDoList[ToDoItem].itemText.substring(0,ToDoList[ToDoItem].itemText.length()-1);
                        }
                    }
                }
            }
            else{
                ToDoList[ToDoItem].itemText.concat(char(eTemp.keyData));
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
    
    if(isMenu==SUB){
        _screen.addBox(1,16,100,50,false,true);
        _screen.addBox(1,16,100,50,true,false);
        for(byte i = 0;i<2;i++){
            if(i==currentToDo){
                _screen.setFont(12,true,false);
            }
            else{
                _screen.setFont(12,false,false);
            }
            _screen.addText(ToDoMenuNames[i],8,36+(i*20),true);
        }
    }
    else{
        _screen.clearScreen(false,goFast);
        _screen.addHead("To-Do List");
        byte listOffset;
        if(ToDoItem < 4){
            listOffset = 0;
        }
        else if(ToDoItem > 46){
            listOffset = 43;
        }
        else{
            listOffset = ToDoItem - 3;
        }
        for(byte i = 0;i<7;i++){
            if(i+listOffset==ToDoItem){
                _screen.setFont(12,true,false);
                _screen.addText(">>",0,40+(i*20),true);

            }
            else{
                _screen.setFont(12,false,false);
            }
            _screen.addText(ToDoList[i+listOffset].itemText,40,40+(i*20),true);
            if(ToDoList[i+listOffset].itemText.length() > 0){
                _screen.drawCkeckbox(10,25+(i*20),ToDoList[i+listOffset].isTicked);
            }
        }
    }

    //draw some stuff
    
    _screen.show(goFast);

}