#ifndef CL32_file_h
#define CL32_file_h
#include "Arduino.h" 
#include <SD.h>
#include <SPI.h>

//store the 'window' of code in an array. the struct will let me store more info about each char like the position in the
//full file and maybe even formatting data for bold and italic later maybe
struct CharData {
  char val;
  unsigned int pos;
};

const byte windowW = 34;
const byte windowH = 12;

//each line has a start and end value, i was just grabbing the next line to get the end of the last one, but that
//will error if you try that on the last line (or at leas behave strangely) so lets store the start and end for each line
struct LineData {
  unsigned int start;
  unsigned int end;
  unsigned int len;
};

struct FolderData {
    FolderData *parent;
    byte layer;
    char name[30];
};

class CL32_file{
public:
	CL32_file();
    void listFile();
    void saveFolder(File dir, int depth, FolderData* parent);
    void listFolder();
    void getLines();
    void readFile();
    void getPath(FolderData* input);
    void getWindow();
    void putChar(char charIn,unsigned int charPos);
    void moveCursor(byte distance,char direction);
    char* getFilename();
    char* windowChar(u_int16_t x, u_int16_t y);
    int iRow, iCol;
    unsigned int iWindowX, iWindowY;
    int iFolders, iFiles, iFol, iFil, iPage;
    char fileName[20] = {""};
    char filePath[50] = {"/"};
    char sFileList[20][50] = {""};
    FolderData FolderList[50];
private:
    //this array stores the 'window' of code, it will be populated based on the position of the data within the file
    CharData _codeLines[windowH][windowW] ;
    //the file could be 50 lines of 100 char of text, or could be 1000 lines with 5 chars of text, so it might be handy to have a
    //list of where in the file each line of the text is. therefore we need an array to store that too
    LineData _lineNumbers[1000];
    //lets make a massive array of space to store the files, that will mean some files are too big to open, but
    //will also mean that the memory usage will be consistant
    char _fileBuffer[50000];
    //and some variables to store how big things are...
    unsigned int _fileSize, _lineCount;
    int _lineLength;
    //does the file have windows style carrage return and line feed?
    bool _bCRLF;
};
#endif