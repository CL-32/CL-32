#include <Arduino.h>
#include <Wire.h>
#include "CL32_logo.h"
#include "CL32_screen.h"
#include "CL32.h"

//U8G2_ST7305_168X384_F_4W_SW_SPI u8g2(U8G2_R3, CL32_sck, CL32_mosi, CL32_epd_cs, CL32_dc , CL32_rst);
U8G2_ST7305_168X384_F_4W_HW_SPI u8g2(U8G2_R3, CL32_epd_cs, CL32_dc , CL32_rst);

GxEPD2_BW<GxEPD2_290_GDEY029T71H, GxEPD2_290_GDEY029T71H::HEIGHT> display(GxEPD2_290_GDEY029T71H(/*CS=D8*/ CL32_epd_cs, /*DC=D3*/ CL32_dc, /*RST=D4*/ CL32_rst, /*BUSY=D2*/ CL32_bsy)); // GDEY029T71H 168x384, SSD1685, (FPC-H004 22.03.24)

void CL32_screen::init(){
    hspi.begin(CL32_sck, CL32_miso, CL32_mosi, CL32_epd_cs); // remap hspi for EPD (swap pins)
    if (CL32_screen_type==EPD){ 
        display.epd2.selectSPI(hspi, SPISettings(60000000, MSBFIRST, SPI_MODE0));
        display.init(115200); 
        display.setRotation(3);
        display.setTextWrap(false);
        display.setFullWindow();
        display.fillScreen(darkMode?GxEPD_BLACK:GxEPD_WHITE);
        display.drawBitmap((display.width()/2) -148,(display.height()/2)-64,image_data_CL32_logo,296,128,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.display(true); // full update
        display.hibernate();
    }
    else{//rlcd
        SPI = hspi;         
        u8g2.setBusClock(80000000);
        u8g2.begin(); 
        u8g2.clearDisplay();
        u8g2.clearBuffer();
        u8g2.setDrawColor(darkMode?1:0);
        u8g2.drawBox(0,0,u8g2.getDisplayWidth(),u8g2.getDisplayHeight());
        u8g2.setBitmapMode(1);
        u8g2.setDrawColor(darkMode?0:1);
        u8g2.drawXBM((u8g2.getDisplayWidth()/2) -148,(u8g2.getDisplayHeight()/2)-64,296,128,CL32_u8g2);
        u8g2.sendBuffer();
        delay(1000);//we dont have the EPD lag to leave the splash there, so we have to fake it
    }
}

void CL32_screen::clearScreen(bool isBlack, bool goFast){
    if(darkMode){
        isBlack=!isBlack;
    }
    if (CL32_screen_type==EPD){
        if(goFast){
            display.setPartialWindow(0,0,display.width(),display.height());
        }
        else{
            display.setFullWindow();
        }
        display.fillScreen(isBlack ? GxEPD_BLACK:GxEPD_WHITE);
    }
    else{//u8g2
        u8g2.clearBuffer();
        u8g2.setDrawColor(isBlack?1:0);
        u8g2.drawBox(0,0,u8g2.getDisplayWidth(),u8g2.getDisplayHeight());
        u8g2.setDrawColor(!isBlack?1:0);
    }
}


void CL32_screen::setFont(byte fontSize, bool isBold, bool isItalic){
    if (CL32_screen_type==EPD){
        if(fontSize==9){
            if (isBold){
                display.setFont(&FreeMonoBold9pt7b);
            }
            else if (isItalic){
                display.setFont(&FreeMonoOblique9pt7b);
            }
            else{
                display.setFont(&FreeMono9pt7b);
            }
        }
        else if(fontSize==12){
            if (isBold){
                display.setFont(&FreeMonoBold12pt7b);
            }
            else if (isItalic){
                display.setFont(&FreeMonoOblique12pt7b);
            }
            else{
                display.setFont(&FreeMono12pt7b);
            }
        }
        else if(fontSize==18){
            if (isBold){
                display.setFont(&FreeMonoBold18pt7b);
            }
            else if (isItalic){
                display.setFont(&FreeMonoOblique18pt7b);
            }
            else{
                display.setFont(&FreeMono18pt7b);
            }
        }
    }
    else{
        if(fontSize==9){
            if (isBold){
                u8g2.setFont(u8g2_font_courB12_tf);
            }
            else if (isItalic){
                u8g2.setFont(u8g2_font_courR12_tf);
            }
            else{
                u8g2.setFont(u8g2_font_courR12_tf);
            }
        }
        else if(fontSize==12){
            if (isBold){
                u8g2.setFont(u8g2_font_courB18_tf);
            }
            else if (isItalic){
                u8g2.setFont(u8g2_font_courR18_tf);
            }
            else{
                u8g2.setFont(u8g2_font_courR18_tf);
            }
        }
        else if(fontSize==18){
            if (isBold){
                u8g2.setFont(u8g2_font_courB24_tf);
            }
            else if (isItalic){
                u8g2.setFont(u8g2_font_courR24_tf);
            }
            else{
                u8g2.setFont(u8g2_font_courR24_tf);
            }
        }
    }
}

void CL32_screen::addText(char *textIn, int posX, int posY, bool isBlack){
    if(darkMode){
        isBlack=!isBlack;
    }
    if (CL32_screen_type==EPD){
        display.setCursor(posX,posY);
        display.setTextColor(isBlack ? GxEPD_BLACK:GxEPD_WHITE);
        display.print(textIn);
    }
    else{//u8g2
        u8g2.setDrawColor(isBlack?1:0);
        u8g2.drawStr(posX,posY,textIn);
        u8g2.setDrawColor(!isBlack?1:0);
    }

}
void CL32_screen::addText(String textIn, int posX, int posY, bool isBlack){
    if(darkMode){
        isBlack=!isBlack;
    }
    if (CL32_screen_type==EPD){
        display.setCursor(posX,posY);
        display.setTextColor(isBlack ? GxEPD_BLACK:GxEPD_WHITE);
        display.print(textIn);
    }
    else{//u8g2
        char sTemp[200];
        sprintf(sTemp,"%s",textIn);
        u8g2.setDrawColor(isBlack?1:0);
        u8g2.drawStr(posX,posY,sTemp);
        u8g2.setDrawColor(!isBlack?1:0);
    }
}
void CL32_screen::addLine(int startX, int startY, int endX, int endY, bool isBlack){
    if(darkMode){
        isBlack=!isBlack;
    }
    if (CL32_screen_type==EPD){
        display.drawLine(startX,startY,endX,endY,isBlack ? GxEPD_BLACK:GxEPD_WHITE);
    }
    else{//u8g2
        u8g2.setDrawColor(isBlack?1:0);
        u8g2.drawLine(startX,startY,endX,endY);
        u8g2.setDrawColor(!isBlack?1:0);
    }

}
void CL32_screen::addBox(int startX, int startY, int sizeX, int sizeY, bool isBlack, bool isFilled){
    if(darkMode){
        isBlack=!isBlack;
    }
    if (CL32_screen_type==EPD){
        if(isFilled){
            display.fillRect(startX,startY,sizeX,sizeY,isBlack ? GxEPD_BLACK:GxEPD_WHITE);
        }
        else{
            display.drawRect(startX,startY,sizeX,sizeY,isBlack ? GxEPD_BLACK:GxEPD_WHITE);
        }
    }
    else{//u8g2
        u8g2.setDrawColor(isBlack?1:0);
        if(isFilled){
            u8g2.drawBox(startX,startY,sizeX,sizeY);
        }
        else{
            u8g2.drawFrame(startX,startY,sizeX,sizeY);
        }
        u8g2.setDrawColor(!isBlack?1:0);
    }

}

void CL32_screen::addHead(String title){
    if (CL32_screen_type==EPD){
        display.setFont(&FreeMono9pt7b);
        display.setTextColor(darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.setCursor(0,10);
        display.print(title);
        display.setCursor((display.width()/2)-(12*2.5),10);
        display.print(_time.getTimeText());
        display.setCursor(display.width()-50,10);
        if (_CL32._shift==UNPRESSED){
        display.print("abc");
        }
        else if (_CL32._shift==ONEPRESS){
        display.print("Abc");
        }
        else{
        display.print("ABC");
        }
        display.setCursor(display.width()-85,10);
        if (_CL32._fn==LOCKPRESS){
            display.setTextColor(darkMode?GxEPD_BLACK:GxEPD_WHITE);
            display.fillRect(display.width()-85,0,23,12,darkMode?GxEPD_WHITE:GxEPD_BLACK);
            display.print("fn");
        }
        else if (_CL32._fn==ONEPRESS){
            display.setTextColor(darkMode?GxEPD_WHITE:GxEPD_BLACK);
            display.print("fn");
        }
        display.setCursor(0,display.width()-30);
        if(_CL32.getUSB()){
            display.print("U");
        }
        display.setCursor(0,display.width()-20);
        if(_CL32.getCharging()){
            display.print("C");
        }
        display.drawLine(display.width()-11,0,display.width()-1,0,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.drawLine(display.width()-11,12,display.width()-1,12,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.drawLine(display.width()-11,0,display.width()-11,12,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.drawLine(display.width()-1,0,display.width()-1,12,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.drawLine(display.width()-12,2,display.width()-12,12-2,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.drawLine(display.width()-13,2,display.width()-13,12-2,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        //battery bars
        int iVolt = _CL32.getVoltage();
        if (iVolt > 3300){
        display.drawLine(display.width()-3,2,display.width()-3,iFontH-2,darkMode?GxEPD_WHITE:GxEPD_BLACK); //3.3v
        }
        if (iVolt > 3300){
        display.drawLine(display.width()-4,2,display.width()-4,iFontH-2,darkMode?GxEPD_WHITE:GxEPD_BLACK); //3.5v
        }
        if (iVolt > 3700){
        display.drawLine(display.width()-5,2,display.width()-5,iFontH-2,darkMode?GxEPD_WHITE:GxEPD_BLACK); //3.7v
        }
        if (iVolt > 3800){
        display.drawLine(display.width()-6,2,display.width()-6,iFontH-2,darkMode?GxEPD_WHITE:GxEPD_BLACK); //3.8v
        }
        if (iVolt > 3900){
        display.drawLine(display.width()-7,2,display.width()-7,iFontH-2,darkMode?GxEPD_WHITE:GxEPD_BLACK); //3.9v
        }
        if (iVolt > 4000){
        display.drawLine(display.width()-8,2,display.width()-8,iFontH-2,darkMode?GxEPD_WHITE:GxEPD_BLACK); //4.0v
        }
        if (iVolt > 4100){
        display.drawLine(display.width()-9,2,display.width()-9,iFontH-2,darkMode?GxEPD_WHITE:GxEPD_BLACK); //4.1v+
        }
        display.drawLine(0,12+2,display.width()-1,12+2,darkMode?GxEPD_WHITE:GxEPD_BLACK);
    } 
    else{//u8g2        
        u8g2.setFont(u8g2_font_courR12_tf);
        u8g2.setCursor(0,11);
        u8g2.print(title);
        u8g2.setCursor((u8g2.getDisplayWidth()/2)-(12*2.5),11);
        u8g2.print(_time.getTimeText());
        u8g2.setCursor(u8g2.getDisplayWidth()-50,11);
        if (_CL32._shift==UNPRESSED){
        u8g2.print("abc");
        }
        else if (_CL32._shift==ONEPRESS){
        u8g2.print("Abc");
        }
        else{
        u8g2.print("ABC");
        }
        u8g2.setCursor(u8g2.getDisplayWidth()-85,11);
        if (_CL32._fn==LOCKPRESS){
            u8g2.drawBox(u8g2.getDisplayWidth()-85,0,23,12);
            u8g2.setDrawColor(darkMode?0:1);
            u8g2.print("fn");
            u8g2.setDrawColor(darkMode?1:0);
        }
        else if (_CL32._fn==ONEPRESS){
            u8g2.print("fn");
        }
        u8g2.setCursor(0,u8g2.getDisplayWidth()-30);
        if(_CL32.getUSB()){
            u8g2.print("U");
        }
        u8g2.setCursor(0,u8g2.getDisplayWidth()-20);
        if(_CL32.getCharging()){
            u8g2.print("C");
        }
        u8g2.drawLine(u8g2.getDisplayWidth()-11,0,u8g2.getDisplayWidth()-1,0);
        u8g2.drawLine(u8g2.getDisplayWidth()-11,12,u8g2.getDisplayWidth()-1,12);
        u8g2.drawLine(u8g2.getDisplayWidth()-11,0,u8g2.getDisplayWidth()-11,12);
        u8g2.drawLine(u8g2.getDisplayWidth()-1,0,u8g2.getDisplayWidth()-1,12);
        u8g2.drawLine(u8g2.getDisplayWidth()-12,2,u8g2.getDisplayWidth()-12,12-2);
        u8g2.drawLine(u8g2.getDisplayWidth()-13,2,u8g2.getDisplayWidth()-13,12-2);
        //battery bars
        int iVolt = _CL32.getVoltage();
        if (iVolt > 3300){
        u8g2.drawLine(u8g2.getDisplayWidth()-3,2,u8g2.getDisplayWidth()-3,iFontH-2); //3.3v
        }
        if (iVolt > 3300){
        u8g2.drawLine(u8g2.getDisplayWidth()-4,2,u8g2.getDisplayWidth()-4,iFontH-2); //3.5v
        }
        if (iVolt > 3700){
        u8g2.drawLine(u8g2.getDisplayWidth()-5,2,u8g2.getDisplayWidth()-5,iFontH-2); //3.7v
        }
        if (iVolt > 3800){
        u8g2.drawLine(u8g2.getDisplayWidth()-6,2,u8g2.getDisplayWidth()-6,iFontH-2); //3.8v
        }
        if (iVolt > 3900){
        u8g2.drawLine(u8g2.getDisplayWidth()-7,2,u8g2.getDisplayWidth()-7,iFontH-2); //3.9v
        }
        if (iVolt > 4000){
        u8g2.drawLine(u8g2.getDisplayWidth()-8,2,u8g2.getDisplayWidth()-8,iFontH-2); //4.0v
        }
        if (iVolt > 4100){
        u8g2.drawLine(u8g2.getDisplayWidth()-9,2,u8g2.getDisplayWidth()-9,iFontH-2); //4.1v+
        }
        u8g2.drawLine(0,12+2,u8g2.getDisplayWidth()-1,12+2);
    }
}

void CL32_screen::showMsg(char *textIn){
    int16_t x, y, iPad = 10;
    uint16_t w, h;
    if (CL32_screen_type==EPD){
        display.setFont(&FreeMono12pt7b);
        display.getTextBounds(textIn,0,50,&x,&y,&w,&h);
        x = (display.width() - w)/2;
        y = (display.height() - h)/2;
        display.setPartialWindow(0,0,display.width(),display.height());
        display.fillRect(x-iPad,y-iPad,w+(iPad*2),h+(iPad*2),darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.fillRect(x-(iPad/2),y-(iPad/2),w+iPad,h+iPad,darkMode?GxEPD_BLACK:GxEPD_WHITE);
        display.setCursor(x,y+((h+iPad)/2));
        display.print(textIn);
        display.display(true);
        display.hibernate();
    }
    else{//u8g2
        u8g2.setFont(u8g2_font_courB12_tf);
        w = u8g2.getStrWidth(textIn);
        h = u8g2.getMaxCharHeight() ;
        x = (u8g2.getDisplayWidth() - w)/2;
        y = (u8g2.getDisplayHeight() - h)/2;
        u8g2.drawBox(x-iPad,y-iPad,w+(iPad*2),h+(iPad*2));
        u8g2.setDrawColor(darkMode?0:1);
        u8g2.drawBox(x-(iPad/2),y-(iPad/2),w+iPad,h+iPad);
        u8g2.setDrawColor(darkMode?1:0);
        u8g2.setCursor(x,y+((h+iPad)/2));
        u8g2.print(textIn);
        u8g2.sendBuffer();
    }
}


void CL32_screen::yesNoDialog(char *prompt, bool yesNo){
    int16_t x, y, iPad = 10;
    uint16_t w, h;
    if (CL32_screen_type==EPD){
        display.setFont(&FreeMonoBold12pt7b);
        display.getTextBounds(prompt,0,50,&x,&y,&w,&h);
        h *= 4;
        x = (display.width() - w)/2;
        y = (display.height() - h)/2;
        display.setPartialWindow(0,0,display.width(),display.height());
        display.fillRect(x-iPad,y-iPad,w+(iPad*2),h+(iPad*2),darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.fillRect(x-(iPad/2),y-(iPad/2),w+iPad,h+iPad,darkMode?GxEPD_BLACK:GxEPD_WHITE);
        display.setCursor(x,y+(iPad*2));
        display.print(prompt);
        //lets show the yes/no option
        display.getTextBounds(yesNo?"Yes":"No",0,50,&x,&y,&w,&h);
        display.setCursor((display.width()-w)/2,display.height()/2+(iPad*2));
        display.print(yesNo?"Yes":"No");
        display.setFont(&FreeMono12pt7b);
        display.getTextBounds(!yesNo?"Yes":"No",0,50,&x,&y,&w,&h);
        display.setCursor((display.width()/2)+(yesNo?40:-40-w),display.height()/2+(iPad*2));
        display.print(!yesNo?"Yes":"No");
        display.display(true);
        display.hibernate();
    }
    else{//u8g2
        u8g2.setFont(u8g2_font_courB12_tf);
        w = u8g2.getStrWidth(prompt);
        h = u8g2.getMaxCharHeight() * 4;
        x = (u8g2.getDisplayWidth() - w)/2;
        y = (u8g2.getDisplayHeight() - h)/2;
        u8g2.drawBox(x-iPad,y-iPad,w+(iPad*2),h+(iPad*2));
        u8g2.setDrawColor(darkMode?0:1);
        u8g2.drawBox(x-(iPad/2),y-(iPad/2),w+iPad,h+iPad);
        u8g2.setDrawColor(darkMode?1:0);
        u8g2.setCursor(x,y+(iPad*2));
        u8g2.print(prompt);
        //lets show the yes/no option
        w = u8g2.getStrWidth(yesNo?"Yes":"No");
        u8g2.setCursor((u8g2.getDisplayWidth()-w)/2,u8g2.getDisplayHeight()/2+(iPad*2));
        u8g2.print(yesNo?"Yes":"No");
        u8g2.setFont(u8g2_font_courR12_tf);
        w = u8g2.getStrWidth(!yesNo?"Yes":"No");
        u8g2.setCursor((u8g2.getDisplayWidth()/2)+(yesNo?40:-40-w),u8g2.getDisplayHeight()/2+(iPad*2));
        u8g2.print(!yesNo?"Yes":"No");
        u8g2.sendBuffer();
    }
}
void CL32_screen::inputDialog(char *prompt, char *userInput){
    int16_t x, y, iPad = 10;
    uint16_t w, h;
    if (CL32_screen_type==EPD){
        display.setFont(&FreeMonoBold12pt7b);
        display.getTextBounds(prompt,0,50,&x,&y,&w,&h);
        h *= 4;
        x = (display.width() - w)/2;
        y = (display.height() - h)/2;
        display.setPartialWindow(0,0,display.width(),display.height());
        display.fillRect(x-iPad,y-iPad,w+(iPad*2),h+(iPad*2),darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.fillRect(x-(iPad/2),y-(iPad/2),w+iPad,h+iPad,darkMode?GxEPD_BLACK:GxEPD_WHITE);
        display.setCursor(x,y+(iPad*2));
        display.print(prompt);
        //lets show the yes/no option
        display.setFont(&FreeMono12pt7b);
        display.getTextBounds(userInput,0,50,&x,&y,&w,&h);
        x = (display.width() - w)/2;
        y = ((display.height() - h)/2)+(iPad*2);
        display.setCursor(x,y);
        display.print(userInput);
        display.display(true);
        display.hibernate();
    }
    else{//u8g2
        u8g2.setFont(u8g2_font_courB12_tf);
        w = u8g2.getStrWidth(prompt);
        h = u8g2.getMaxCharHeight() * 4;
        x = (u8g2.getDisplayWidth() - w)/2;
        y = (u8g2.getDisplayHeight() - h)/2;
        u8g2.drawBox(x-iPad,y-iPad,w+(iPad*2),h+(iPad*2));
        u8g2.setDrawColor(darkMode?0:1);
        u8g2.drawBox(x-(iPad/2),y-(iPad/2),w+iPad,h+iPad);
        u8g2.setDrawColor(darkMode?1:0);
        u8g2.setCursor(x,y+(iPad*2));
        u8g2.print(prompt);
        //lets show the yes/no option
        u8g2.setFont(u8g2_font_courR12_tf);
        w = u8g2.getStrWidth(userInput);
        h = u8g2.getMaxCharHeight();
        x = (u8g2.getDisplayWidth() - w)/2;
        y = ((u8g2.getDisplayHeight() - h)/2)+(iPad*2);
        u8g2.setCursor(x,y);
        u8g2.print(userInput);
        u8g2.sendBuffer();
    }
}

void CL32_screen::drawCkeckbox(int x, int y, bool isTicked){   
    if (CL32_screen_type==EPD){
        display.fillRoundRect(x,y,18,18,2,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.fillRoundRect(x+2,y+2,14,14,2,darkMode?GxEPD_BLACK:GxEPD_WHITE);
        if(isTicked){
            display.setFont(&FreeMonoBold9pt7b);
            display.setCursor(x+4,y+13);
            display.print("X");
        }
    }
    else{//u8g2 
        u8g2.drawRBox(x,y,18,18,2);
        u8g2.setDrawColor(darkMode?0:1);
        u8g2.drawRBox(x+2,y+2,14,14,2);
        u8g2.setDrawColor(darkMode?1:0);
        if(isTicked){
            u8g2.setFont(u8g2_font_courR12_tf);
            u8g2.setCursor(x+4,y+13);
            u8g2.print("X");
        }
    }
}


void CL32_screen::setProgMax(int progMax){
    progressMax = progMax;
}
void CL32_screen::drawProgress(int progress){
    uint16_t s, w, x, y, h, iPad;
    iPad = 10;
    h = 10;
    s = CL32_screen_type==EPD?display.width():u8g2.getDisplayWidth();
    s /= 2;
    s -= 100;
    x = s;
    y = CL32_screen_type==EPD?display.height():u8g2.getDisplayHeight();
    y /= 2;
    y += h/2;
    if(progressMax==0){//aimless scrolling
        s += currProgress*2;
        w = 20;
        if(currProgress>=90){
            currProgress=0;
        }
        else{
            currProgress++;
        }
    }
    else{
        w = (progressMax/progress) * 200;
    }
    if(CL32_screen_type==EPD){

    }
    else{//u8g2
        u8g2.setDrawColor(darkMode?0:1);
        u8g2.drawBox(x-iPad,y-iPad,200+(iPad*2),h+(iPad*2));
        u8g2.setDrawColor(darkMode?1:0);
        u8g2.drawBox(x-(iPad/2),y-(iPad/2),200+iPad,h+iPad);
        u8g2.setDrawColor(darkMode?0:1);
        u8g2.drawBox(s,y,w,h);
        u8g2.sendBuffer();
    }
}

void CL32_screen::show(bool goFast){
    if (CL32_screen_type==EPD){
        display.display(goFast); // full update
        display.hibernate();
    }
    else{//u8g2
        u8g2.sendBuffer();
    }
}

void CL32_screen::drawSleep(){
    if (CL32_screen_type==EPD){
        display.fillRect(0,0,display.width(),14,darkMode?GxEPD_BLACK:GxEPD_WHITE);
        display.setFont(&FreeMono9pt7b);
        display.setTextColor(darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.setCursor((display.width()/2)-(12*11),10);
        display.print("Shhh, I am Sleeping....");
        display.drawBitmap((display.width()/2) -148,30,image_data_CL32_logo,296,128,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.display(true);
        display.hibernate();
    }
    else{//u8g2
        u8g2.setDrawColor(darkMode?1:0);
        u8g2.drawBox(0,0,u8g2.getDisplayWidth(),14);
        u8g2.setDrawColor(darkMode?0:1);
        u8g2.setFont(u8g2_font_courR12_tf);
        u8g2.setCursor((u8g2.getDisplayWidth()/2)-(12*11),10);
        u8g2.print("Shhh, I am Sleeping....");
        u8g2.drawXBM((u8g2.getDisplayWidth()/2) -148,30,296,128,CL32_u8g2);
        u8g2.sendBuffer();
    }
}

void CL32_screen::refreshStatus(){
    if (CL32_screen_type==EPD){
        display.fillRect((display.width()/2)-(12*2.5),0,(display.width()/2)+(12*2.5),14,darkMode?GxEPD_BLACK:GxEPD_WHITE);
        display.setFont(&FreeMono9pt7b);
        display.setTextColor(darkMode?GxEPD_WHITE:GxEPD_BLACK);
        _time.loadTime();
        display.setCursor((display.width()/2)-(12*2.5),10);
        display.print(_time.getTimeText());
        display.drawLine(display.width()-11,0,display.width()-1,0,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.drawLine(display.width()-11,12,display.width()-1,12,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.drawLine(display.width()-11,0,display.width()-11,12,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.drawLine(display.width()-1,0,display.width()-1,12,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.drawLine(display.width()-12,2,display.width()-12,12-2,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.drawLine(display.width()-13,2,display.width()-13,12-2,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        //battery bars
        int iVolt = _CL32.getVoltage();
        if (iVolt > 3300){
        display.drawLine(display.width()-3,2,display.width()-3,iFontH-2,darkMode?GxEPD_WHITE:GxEPD_BLACK); //3.3v
        }
        if (iVolt > 3300){
        display.drawLine(display.width()-4,2,display.width()-4,iFontH-2,darkMode?GxEPD_WHITE:GxEPD_BLACK); //3.5v
        }
        if (iVolt > 3700){
        display.drawLine(display.width()-5,2,display.width()-5,iFontH-2,darkMode?GxEPD_WHITE:GxEPD_BLACK); //3.7v
        }
        if (iVolt > 3800){
        display.drawLine(display.width()-6,2,display.width()-6,iFontH-2,darkMode?GxEPD_WHITE:GxEPD_BLACK); //3.8v
        }
        if (iVolt > 3900){
        display.drawLine(display.width()-7,2,display.width()-7,iFontH-2,darkMode?GxEPD_WHITE:GxEPD_BLACK); //3.9v
        }
        if (iVolt > 4000){
        display.drawLine(display.width()-8,2,display.width()-8,iFontH-2,darkMode?GxEPD_WHITE:GxEPD_BLACK); //4.0v
        }
        if (iVolt > 4100){
        display.drawLine(display.width()-9,2,display.width()-9,iFontH-2,darkMode?GxEPD_WHITE:GxEPD_BLACK); //4.1v+
        }   
        display.display(true);
        display.hibernate();
    }
    else{//u8g2
        u8g2.setDrawColor(darkMode?1:0);
        u8g2.drawBox((u8g2.getDisplayWidth()/2)-(12*2.5),0,(u8g2.getDisplayWidth()/2)+(12*2.5),14);
        u8g2.setDrawColor(darkMode?0:1);
        u8g2.setFont(u8g2_font_courR12_tf);
        _time.loadTime();
        u8g2.setCursor((u8g2.getDisplayWidth()/2)-(12*2.5),11);
        u8g2.print(_time.getTimeText());
        u8g2.drawLine(u8g2.getDisplayWidth()-11,0,u8g2.getDisplayWidth()-1,0);
        u8g2.drawLine(u8g2.getDisplayWidth()-11,12,u8g2.getDisplayWidth()-1,12);
        u8g2.drawLine(u8g2.getDisplayWidth()-11,0,u8g2.getDisplayWidth()-11,12);
        u8g2.drawLine(u8g2.getDisplayWidth()-1,0,u8g2.getDisplayWidth()-1,12);
        u8g2.drawLine(u8g2.getDisplayWidth()-12,2,u8g2.getDisplayWidth()-12,12-2);
        u8g2.drawLine(u8g2.getDisplayWidth()-13,2,u8g2.getDisplayWidth()-13,12-2);
        //battery bars
        int iVolt = _CL32.getVoltage();
        if (iVolt > 3300){
        u8g2.drawLine(u8g2.getDisplayWidth()-3,2,u8g2.getDisplayWidth()-3,iFontH-2); //3.3v
        }
        if (iVolt > 3300){
        u8g2.drawLine(u8g2.getDisplayWidth()-4,2,u8g2.getDisplayWidth()-4,iFontH-2); //3.5v
        }
        if (iVolt > 3700){
        u8g2.drawLine(u8g2.getDisplayWidth()-5,2,u8g2.getDisplayWidth()-5,iFontH-2); //3.7v
        }
        if (iVolt > 3800){
        u8g2.drawLine(u8g2.getDisplayWidth()-6,2,u8g2.getDisplayWidth()-6,iFontH-2); //3.8v
        }
        if (iVolt > 3900){
        u8g2.drawLine(u8g2.getDisplayWidth()-7,2,u8g2.getDisplayWidth()-7,iFontH-2); //3.9v
        }
        if (iVolt > 4000){
        u8g2.drawLine(u8g2.getDisplayWidth()-8,2,u8g2.getDisplayWidth()-8,iFontH-2); //4.0v
        }
        if (iVolt > 4100){
        u8g2.drawLine(u8g2.getDisplayWidth()-9,2,u8g2.getDisplayWidth()-9,iFontH-2); //4.1v+
        }   
        u8g2.sendBuffer();

    }
}
void CL32_screen::drawOff(){
    if (CL32_screen_type==EPD){
        display.setFullWindow();
        display.fillScreen(darkMode?GxEPD_BLACK:GxEPD_WHITE);
        display.setFont(&FreeMono9pt7b);
        display.setTextColor(darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.setCursor((display.width()/2)-(12*6),20);
        display.print("Powered Off!");
        display.drawBitmap((display.width()/2) -148,30,image_data_CL32_logo,296,128,darkMode?GxEPD_WHITE:GxEPD_BLACK);
        display.display(false);
        display.powerOff();
    }
    else{//u8g2
        //there isnt anything to do here, if we draw anything on the screen, it will be gone when the power goes
    }

}

int CL32_screen::width(){
    if (CL32_screen_type==EPD){
        return display.width();
    }
    else{//u8g2
        return u8g2.getDisplayWidth();
    }
}

int CL32_screen::height(){
    if (CL32_screen_type==EPD){
        return display.height();
    }
    else{//u8g2
        return u8g2.getDisplayHeight();
    }
}