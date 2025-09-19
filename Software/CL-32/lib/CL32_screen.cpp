#include <Arduino.h>
#include <Wire.h>
#include "CL32.h"
#include "CL32_logo.h"
#include "CL32_screen.h"


CL32_screen::CL32_screen() {
    //might not be much to do here
}

void CL32_screen::screenInit(){
    hspi.begin(CL32_sck, CL32_miso, CL32_mosi, CL32_epd_cs); // remap hspi for EPD (swap pins)
    display.epd2.selectSPI(hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
    display.init(115200); 
    display.setRotation(3);
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    display.drawBitmap((display.width()/2) -148,(display.height()/2)-64,image_data_cl32_logo,296,128,GxEPD_WHITE,GxEPD_BLACK);
    display.display(true); // full update
    display.hibernate();

}

void CL32_screen::clearScreen(bool isBlack){
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