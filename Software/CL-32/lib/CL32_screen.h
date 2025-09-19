#ifndef CL32_screen_h
#define CL32_screen_h
#include "Arduino.h" 
#include "CL32.h"
#include <GxEPD2_BW.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoOblique9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMonoOblique12pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

GxEPD2_BW<GxEPD2_290_GDEY029T71H, GxEPD2_290_GDEY029T71H::HEIGHT> display(GxEPD2_290_GDEY029T71H(/*CS=D8*/ CL32_epd_cs, /*DC=D3*/ CL32_dc, /*RST=D4*/ CL32_rst, /*BUSY=D2*/ CL32_bsy)); // GDEY029T71H 168x384, SSD1685, (FPC-H004 22.03.24)

SPIClass hspi(HSPI);

class CL32_screen{
public:
	CL32_screen();
	void screenInit();
    void clearScreen(bool isBlack);
    void setFont(byte fontSize, bool isBold, bool isItalic);
    void addText(char *textIn, int posX, int posY, bool isBlack);
    void addLine(int startX, int startY, int endX, int endY, bool isBlack);
    void addBox(int startX, int startY, int sizeX, int sizeY, bool isBlack, bool isFilled);
private:
    struct tm _CL32time;
};
#endif