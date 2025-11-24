#include <Arduino.h>
#include <Wire.h>
#include "CL32_logo.h"
#include "CL32_screen.h"
#include "CL32.h"


GxEPD2_BW<GxEPD2_290_GDEY029T71H, GxEPD2_290_GDEY029T71H::HEIGHT> display(GxEPD2_290_GDEY029T71H(/*CS=D8*/ CL32_epd_cs, /*DC=D3*/ CL32_dc, /*RST=D4*/ CL32_rst, /*BUSY=D2*/ CL32_bsy)); // GDEY029T71H 168x384, SSD1685, (FPC-H004 22.03.24)

void CL32_screen::init(){
    hspi.begin(CL32_sck, CL32_miso, CL32_mosi, CL32_epd_cs); // remap hspi for EPD (swap pins)
    display.epd2.selectSPI(hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
    display.init(115200); 
    display.setRotation(3);
    display.setTextWrap(false);
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    display.drawBitmap((display.width()/2) -148,(display.height()/2)-64,image_data_cl32_logo,296,128,GxEPD_WHITE,GxEPD_BLACK);
    display.display(true); // full update
    display.hibernate();
}

void CL32_screen::clearScreen(bool isBlack, bool goFast){
    if(goFast){
        display.setPartialWindow(0,0,display.width(),display.height());
    }
    else{
        display.setFullWindow();
    }
    display.fillScreen(isBlack ? GxEPD_BLACK:GxEPD_WHITE);
}


void CL32_screen::setFont(byte fontSize, bool isBold, bool isItalic){
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
}

void CL32_screen::addText(char *textIn, int posX, int posY, bool isBlack){
    display.setCursor(posX,posY);
    display.setTextColor(isBlack ? GxEPD_BLACK:GxEPD_WHITE);
    display.print(textIn);

}
void CL32_screen::addText(String textIn, int posX, int posY, bool isBlack){
    display.setCursor(posX,posY);
    display.setTextColor(isBlack ? GxEPD_BLACK:GxEPD_WHITE);
    display.print(textIn);

}
void CL32_screen::addLine(int startX, int startY, int endX, int endY, bool isBlack){
    display.drawLine(startX,startY,endX,endY,isBlack ? GxEPD_BLACK:GxEPD_WHITE);

}
void CL32_screen::addBox(int startX, int startY, int sizeX, int sizeY, bool isBlack, bool isFilled){
    if(isFilled){
        display.fillRect(startX,startY,sizeX,sizeY,isBlack ? GxEPD_BLACK:GxEPD_WHITE);
    }
    else{
        display.drawRect(startX,startY,sizeX,sizeY,isBlack ? GxEPD_BLACK:GxEPD_WHITE);
    }
    

}

void CL32_screen::addHead(String title){
   
    display.setFont(&FreeMono9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(0,10);
    display.print(title);
    display.setCursor((display.width()/2)-(12*2.5),10);
    display.print(_time.timeText);
    display.setCursor(display.width()-50,10);
    if (_keys._shift==UNPRESSED){
      display.print("abc");
    }
    else if (_keys._shift==ONEPRESS){
      display.print("Abc");
    }
    else{
      display.print("ABC");
    }
    display.setCursor(display.width()-85,10);
    if (_keys._fn==LOCKPRESS){
        display.setTextColor(GxEPD_WHITE);
        display.fillRect(display.width()-85,0,23,12,GxEPD_BLACK);
        display.print("fn");
    }
    else if (_keys._fn==ONEPRESS){
        display.setTextColor(GxEPD_BLACK);
        display.print("fn");
    }
    display.drawLine(display.width()-11,0,display.width()-1,0,GxEPD_BLACK);
    display.drawLine(display.width()-11,12,display.width()-1,12,GxEPD_BLACK);
    display.drawLine(display.width()-11,0,display.width()-11,12,GxEPD_BLACK);
    display.drawLine(display.width()-1,0,display.width()-1,12,GxEPD_BLACK);
    display.drawLine(display.width()-12,2,display.width()-12,12-2,GxEPD_BLACK);
    display.drawLine(display.width()-13,2,display.width()-13,12-2,GxEPD_BLACK);
    //battery bars
    int iVolt = _batt.getVoltage();
    if (iVolt > 3300){
    display.drawLine(display.width()-3,2,display.width()-3,iFontH-2,GxEPD_BLACK); //3.3v
    }
    if (iVolt > 3300){
      display.drawLine(display.width()-4,2,display.width()-4,iFontH-2,GxEPD_BLACK); //3.5v
    }
    if (iVolt > 3700){
      display.drawLine(display.width()-5,2,display.width()-5,iFontH-2,GxEPD_BLACK); //3.7v
    }
    if (iVolt > 3800){
      display.drawLine(display.width()-6,2,display.width()-6,iFontH-2,GxEPD_BLACK); //3.8v
    }
    if (iVolt > 3900){
      display.drawLine(display.width()-7,2,display.width()-7,iFontH-2,GxEPD_BLACK); //3.9v
    }
    if (iVolt > 4000){
      display.drawLine(display.width()-8,2,display.width()-8,iFontH-2,GxEPD_BLACK); //4.0v
    }
    if (iVolt > 4100){
      display.drawLine(display.width()-9,2,display.width()-9,iFontH-2,GxEPD_BLACK); //4.1v+
    }
    display.drawLine(0,12+2,display.width()-1,12+2,GxEPD_BLACK);
     
}

void CL32_screen::showMsg(char *textIn){
    int16_t x, y, iPad = 10;
    uint16_t w, h;
    display.setFont(&FreeMono12pt7b);
    display.getTextBounds(textIn,0,50,&x,&y,&w,&h);
    x = (display.width() - w)/2;
    y = (display.height() - h)/2;
    display.setPartialWindow(0,0,display.width(),display.height());
    display.fillRect(x-iPad,y-iPad,w+(iPad*2),h+(iPad*2),GxEPD_BLACK);
    display.fillRect(x-(iPad/2),y-(iPad/2),w+iPad,h+iPad,GxEPD_WHITE);
    display.setCursor(x,y+((h+iPad)/2));
    display.print(textIn);
    display.display(true);
    display.hibernate();
}

void CL32_screen::show(bool goFast){
    display.display(goFast); // full update
    display.hibernate();
}

int CL32_screen::width(){
    return display.width();
}

int CL32_screen::height(){
    return display.height();
}