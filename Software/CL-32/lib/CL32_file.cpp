#include <Arduino.h>
#include "CL32.h"
#include "CL32_file.h"
#include <SD.h>
#include <SPI.h>


CL32_file::CL32_file() {
    
}

void CL32_file::listFile(){
  _iFiles = 0;
  //use the non standard spi pin connection to start an sd card
  if(SD.begin(CL32_sd_cs,hspi)){
    File curFile = SD.open(_filePath);
    while (true) {
      File entry =  curFile.openNextFile();
      if (! entry) {
        // no more files
        break;
      }
      if (entry.isDirectory() == false) {
        sprintf(_sFileList[_iFiles], "%s", entry.name());
        _iFiles++;
      }
      entry.close();
    } 
  }
  else{
    _iFiles = -1;
  }
  SD.end();
}


void CL32_file::saveFolder(File dir, int depth, FolderData* parent) {

  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    if (entry.isDirectory()) {
      _FolderList[_iFolders].layer = depth;
      sprintf(_FolderList[_iFolders].name,"%s" ,entry.name());
      _FolderList[_iFolders].parent = parent;
      _iFolders++;//increment it here so the funcion call has the corect value for a new entry
      saveFolder(entry, depth + 1,&_FolderList[_iFolders-1]);//folders count has already been incremented, so we need to knock it back one
    }
    entry.close();
  }
}

void CL32_file::listFolder(){
  _iFolders = 0;
  //use the non standard spi pin connection to start an sd card
  if(SD.begin(CL32_sd_cs,hspi)){
    File curFile = SD.open("/");
    //need to save the root folder
    _FolderList[_iFolders].layer = 0;
    sprintf(_FolderList[_iFolders].name,"%s","/");
    _FolderList[_iFolders].parent = NULL;
    _iFolders++;
    saveFolder(curFile,1,&_FolderList[_iFolders-1]);
  }
  else{
    _iFolders = -1;
  }
  SD.end();
}

void CL32_file::getLines(){
  //process the file to re-populate the line data
  Serial.println("GetLines");
  _lineLength=_lineCount=0;
  _bCRLF = false;
  //store the first char pos as the 
  _lineNumbers[_lineCount].start = 0;
  for(unsigned int i=0;i<_fileSize;i++){
    //lets see if this char is a newline char windows files have 0D 0A linux files just have 0A
    if(_fileBuffer[i]==13){
      //its a windows style carrage return
      _bCRLF = true;//flag windows stylee
    } 
    if(_fileBuffer[i]==10){
      //its a line feed, so the next line starts at the next char...
      _lineNumbers[_lineCount].end = i;//log the end of the last line
      _lineNumbers[_lineCount].len = _lineNumbers[_lineCount].end-_lineNumbers[_lineCount].start;
      if(_lineNumbers[_lineCount].len>_lineLength){
        _lineLength=_lineNumbers[_lineCount].len;
      }
      _lineCount++;
      _lineNumbers[_lineCount].start = i + 1;//log this as the start of a new line
    }
  }
  _lineNumbers[_lineCount].end = _fileSize;//log the end of the last line
  _lineNumbers[_lineCount].len = _lineNumbers[_lineCount].end-_lineNumbers[_lineCount].start;
  if(_lineNumbers[_lineCount].len>_lineLength){
    _lineLength=_lineNumbers[_lineCount].len;
  }
}

void CL32_file::readFile(){

  //local file read variable
  File curFile;
  char fullFileName[200];
  sprintf(fullFileName,"%s/%s",_filePath,_fileName);
  //use the non standard spi pin connection to start an sd card
  if(SD.begin(CL32_sd_cs,hspi)){
    if(SD.exists(fullFileName)){
      curFile = SD.open(fullFileName,FILE_READ);
      if(curFile){
        _fileSize = curFile.size();  // Get the file size.
        //sanity check
        if(_fileSize < sizeof(_fileBuffer)){
          for(unsigned int i=0;i<_fileSize;i++){
            _fileBuffer[i] = curFile.read();         // Read the file into the buffer.
          }
          _windowX=_windowY=0;//reset
          getLines();
        }
        curFile.close();          
      }
    }
    else{
      _fileSize = 0;//so we can check later
    }
  }
  else{
    Serial.print("SD Card Fail");
  }
  SD.end();
}

void CL32_file::getPath(FolderData* input){
  if(input->parent!=NULL){
    getPath(input->parent);
  }
  sprintf(_filePath,"%s/%s",_filePath,input->name);
}

void CL32_file::getWindow(){
  //we need to grab the lines to fit on the screen
  //there is an x/y value for the top left corner of the 'window' we can use that to
  //count the lines we are reading, and also use that to offset the x in the future too
  //the scroll of will nudge the window down as you scroll to the end of the window, and back up
  //in the same fashion
  unsigned int thisLine = _windowY;
  unsigned int thisChar;
  for(int y = 0;y<windowH;y++){
    thisChar = _lineNumbers[thisLine].start+_windowX;
    for(int x = 0;x<windowW;x++){
      if (thisLine > _lineCount){
        //if this happens, we have run out of lines in the file. just fill the space with nothing
        _codeLines[y][x].val = 0;
        _codeLines[y][x].pos = 0;
      }
      else{
        //the line is valid, lets check to see if the current x pos has a valid char in it
        if(thisChar>_lineNumbers[thisLine].end){
          //the next char in the array is greater than the end of the line, just pad with 0
          _codeLines[y][x].val = 0;
          _codeLines[y][x].pos = 0;
        }
        else{
          //should be good to go, lets gos
          _codeLines[y][x].val = _fileBuffer[thisChar];
          _codeLines[y][x].pos = thisChar;
        }
      }
      thisChar++;
    }
    thisLine++;
  }

}

void CL32_file::putChar(char charIn,unsigned int charPos){
  //so this lets you modify the file in memory, it basically will start at the end of the array
  //and move each char along one until we hit charPos. then insert the new char in there
  //if the char you pass in is the backspace char, then it works the other way, it moves everything
  //forward in the array...
  if(charIn==8){
    //delete a char
    for(unsigned int x = charPos;x<_fileSize;x++){
      _fileBuffer[x]=_fileBuffer[x+1];
    }
    _fileSize--;
  }
  else{
    //add a char
    _fileSize++;
    for(unsigned int x = _fileSize;x>charPos;x--){
      _fileBuffer[x]=_fileBuffer[x-1];
    }
    _fileBuffer[charPos]=charIn;
  }
  getLines();
}

void CL32_file::moveCursor(byte distance,char direction){
  //because the available cursor positions are now limited by the code on the screen
  //its best to have a universal function to decide where the cursor can move, and
  //what to do if it cant move there
  //basically you send how far you want to move (normally 1) and direction N/E/S/W
  if(direction=='N'){
    iRow=iRow-distance;
    if(iRow<0){
      //gone too far, step back
      iRow = 0;
      _windowY = 0;
    }
    else if(iRow<(_windowY)){
      //if we have dropped off the bottom of the window, lets move the window
      _windowY--;
    }
  }
  if(direction=='S'){
    iRow=iRow+distance;
    if(iRow>_lineCount){
      //gone too far, step back
      iRow = _lineCount;
      if(_lineCount<windowH){
        //there are less lines than the height of the window, revert to 0
        _windowY = 0;
      }
      else{
        _windowY = _lineCount-windowH;
      }
    }
    else if(iRow>(_windowY+windowH-1)){
      //if we have dropped off the bottom of the window, lets move the window
      _windowY++;
    }
  }
  if(direction=='E'){
    iCol=iCol+distance;
    if(iCol > _lineLength-1){
      iCol=_lineLength-1;
      if(_lineLength-2<windowW){
        _windowX=0;
      }
      else{
        _windowX = _lineLength-windowW+2;
      }
    }
    else if(iCol>(_windowX+windowW-3)){
      _windowX++;
    }
  }
  if(direction=='W'){
    iCol=iCol-distance;
    if(iCol < 0){
      iCol=0;
      _windowX=0;
    }
    else if(iCol<(_windowX)){
      //if we have dropped off the bottom of the window, lets move the window
      _windowX--;
    }
  }
  //the cursor could now be in no mans land at the end of a line that is
  //sorter than the current pos. lets move left until we find a valid char
  // for(byte x = iCol;x>0;x--){
  //   //check to see if there is a position value here, if there is, lets see if its a printable char
  //   if(codeLines[iRow][x].pos>0){
  //     //we have a position value. so this is an item from the file. lets check if its printable
  //     if(codeLines[iRow][x].val!=10&&codeLines[iRow][x].val!=13&&codeLines[iRow][x].val!=0){
  //       iCol=x;
  //       break;
  //     }
  //   }
  // }
}
