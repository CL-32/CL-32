#ifndef CL32_screen_h
#define CL32_screen_h
#include <Arduino.h> 
#include "CL32_pins.h"
#include <Adafruit_GFX.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoOblique9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMonoOblique12pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

class CL32_screen{
public:
	//CL32_screen();
	void init();
    void clearScreen(bool isBlack, bool goFast);
    void setFont(byte fontSize, bool isBold, bool isItalic);
    void addText(char *textIn, int posX, int posY, bool isBlack);
    void addText(String textIn, int posX, int posY, bool isBlack);
    void addLine(int startX, int startY, int endX, int endY, bool isBlack);
    void addBox(int startX, int startY, int sizeX, int sizeY, bool isBlack, bool isFilled);
    void addHead(String title);
    void showMsg(char *textIn);
    void yesNoDialog(String prompt, bool yesNo);
    void inputDialog(String prompt, String userInput);
    void show(bool goFast);
    int width();
    int height();
private:
    struct tm _CL32time;
};
#endif