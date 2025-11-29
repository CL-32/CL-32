#include <Arduino.h>
#include "CL32.h"
#include "CL32_file.h"
#include <SD.h>
#include <SPI.h>


CL32_file::CL32_file() {
  iPage = 10;  
  iFil = 0;
  iFol = 0;  
}

void CL32_file::listFile(){
  iFiles = 0;
  _screen.showMsg("Getting Files");
  //use the non standard spi pin connection to start an sd card
  if(SD.begin(CL32_sd_cs,hspi)){
    File curFile = SD.open(filePath);
    while (true) {
      File entry =  curFile.openNextFile();
      if (! entry) {
        // no more files
        break;
      }
      if (entry.isDirectory() == false) {
        sprintf(sFileList[iFiles], "%s", entry.name());
        iFiles++;
        if(iFiles>=250){
          //if there are too mant files in a folder it will over run the array and corrupt the data, and break everything
          break;
        }
      }
      entry.close();
    } 
  }
  else{
    iFiles = -1;
    _screen.showMsg("SD Card Fail");
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
      FolderList[iFolders].layer = depth;
      sprintf(FolderList[iFolders].name,"%s" ,entry.name());
      FolderList[iFolders].parent = parent;
      iFolders++;//increment it here so the funcion call has the corect value for a new entry
      saveFolder(entry, depth + 1,&FolderList[iFolders-1]);//folders count has already been incremented, so we need to knock it back one
    }
    entry.close();
  }
}

void CL32_file::listFolder(){
  iFolders = 0;
  _screen.showMsg("Getting Folders");
  //use the non standard spi pin connection to start an sd card
  if(SD.begin(CL32_sd_cs,hspi)){
    File curFile = SD.open("/");
    //need to save the root folder
    FolderList[iFolders].layer = 0;
    sprintf(FolderList[iFolders].name,"/");
    FolderList[iFolders].parent = NULL;
    iFolders++;
    saveFolder(curFile,1,&FolderList[iFolders-1]);
  }
  else{
    iFolders = -1;
  }
  SD.end();
}

void CL32_file::getLines(){
  //process the file to re-populate the line data
  Serial.println("GetLines");
  _lineLength=_lineCount=0;
  bCRLF = false;
  //store the first char pos as the 
  _lineNumbers[_lineCount].start = 0;
  for(unsigned int i=0;i<_fileSize;i++){
    //lets see if this char is a newline char windows files have 0D 0A linux files just have 0A
    if(_fileBuffer[i]==13){
      //its a windows style carrage return
      bCRLF = true;//flag windows stylee
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
  sprintf(fullFileName,"%s/%s",filePath,fileName);
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
          iWindowX=iWindowY=0;//reset
          getLines();
        }
        curFile.close();   
        bModified = false;       
      }
    }
    else{
      _fileSize = 0;//so we can check later
    }
  }
  else{
    _screen.showMsg("SD Card Fail");
  }
  SD.end();
}

void CL32_file::getPath(FolderData* input){
  if(input->parent!=NULL){
    getPath(input->parent);
  }
  sprintf(filePath,"%s/%s",filePath,input->name);
}

void CL32_file::getWindow(){
  //we need to grab the lines to fit on the screen
  //there is an x/y value for the top left corner of the 'window' we can use that to
  //count the lines we are reading, and also use that to offset the x in the future too
  //the scroll of will nudge the window down as you scroll to the end of the window, and back up
  //in the same fashion
  unsigned int thisLine = iWindowY;
  unsigned int thisChar;
  for(int y = 0;y<windowH;y++){
    thisChar = _lineNumbers[thisLine].start+iWindowX;
    for(int x = 0;x<windowW;x++){
      if (thisLine > _lineCount){
        //if this happens, we have run out of lines in the file. just fill the space with nothing
        codeLines[y][x].val = 0;
        codeLines[y][x].pos = 0;
      }
      else{
        //the line is valid, lets check to see if the current x pos has a valid char in it
        if(thisChar>_lineNumbers[thisLine].end){
          //the next char in the array is greater than the end of the line, just pad with 0
          codeLines[y][x].val = 0;
          codeLines[y][x].pos = 0;
        }
        else{
          //should be good to go, lets gos
          codeLines[y][x].val = _fileBuffer[thisChar];
          codeLines[y][x].pos = thisChar;
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
  if(charIn==KB_BSP){
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
  bModified = true;
}

void CL32_file::moveCursor(byte distance,char direction){
  //because the available cursor positions are now limited by the code on the screen
  //its best to have a universal function to decide where the cursor can move, and
  //what to do if it cant move there
  //basically you send how far you want to move (normally 1) and direction N/E/S/W
  switch (direction) {
    case 'N': iRow=iRow-distance; break;
    case 'S': iRow=iRow+distance; break;
    case 'W': iCol=iCol-distance; break;
    case 'E': iCol=iCol+distance;
  }
  //we need to sanity check those values, lets start with the column
  if (iCol < 0){//we cant be any further left than the start of the line
    if(distance==1&&direction=='W'){//if you press left on most editors, it moves the cursor to the end of the line above
      iRow--;
      iCol = _lineNumbers[iRow].len;
    }
    else{
      iCol = 0;
    }
  }
  if (iCol > _lineNumbers[iRow].len){//we cant be any further right than the end of the line
    if(distance==1&&direction=='E'){//if you press left on most editors, it moves the cursor to the end of the line above
      iRow++;
      iCol = 0;
    }
    else{
      iCol = _lineNumbers[iRow].len;
    }
  }
  //and now to check the row
  if (iRow < 0){//we cant be any further up than the start of the file
    iRow = 0;
  }
  if (iRow > _lineCount-1){//we cant be any further down than the end of the file
    iRow = _lineCount-1;
  }
  //we need to tweak the window to try and keep the cursor in the window, lets try the X
  if(iCol < windowW/2 || _lineNumbers[iRow].len < windowW){//the cursor wouldnt break the first half of the width, so lets just pop it to 0
    iWindowX = 0;         //if the line is shorter than the window, then we shouldtn try to scroll
  }
  else if(iCol > _lineNumbers[iRow].len - (windowW/2)){//the cursor is on the last half of the window width on the line. so lets move the window as far over as possible 
    iWindowX =  _lineNumbers[iRow].len - windowW + 3;
  }
  else{//the column is somewhere in the middle, lets try and move that window so the cursor is about in the middle too
    iWindowX = iCol - (windowW/2);
  }
  //lets do the Y now...
  if(iRow < windowH/2 || _lineCount < windowH){//the cursor wouldnt break the first half of the height, so lets just pop it to 0
    iWindowY = 0;         //if the file is shorter than the window we dont want to try and move the window
  }
  else if(iRow > _lineCount - (windowH/2)){//the cursor is on the last half of the window height on the file. so lets move the window as far down as possible 
    iWindowY =  _lineCount - windowH + 1;
  }
  else{//the column is somewhere in the middle, lets try and move that window so the cursor is about in the middle too
    iWindowY = iRow - (windowH/2);
  }
}

char* CL32_file::getFilename(){
  return fileName;
}

char* CL32_file::windowChar(u_int16_t x, u_int16_t y){
  return &codeLines[y][x].val;
}

void CL32_file::saveFile(){
  //local file read variable
  File curFile;
  char fullFileName[200];
  sprintf(fullFileName,"%s/%s",filePath,fileName);
  //use the non standard spi pin connection to start an sd card
  if(SD.begin(CL32_sd_cs,hspi)){
    curFile = SD.open(fullFileName,FILE_WRITE);
    if(curFile){
      for(unsigned int i=0;i<_fileSize;i++){
           curFile.write(_fileBuffer[i]);         // Read the file into the buffer.
      }
      curFile.close();  
      bModified = false;        
    }
  }
  else{
    _screen.showMsg("SD Card Fail");
  }
  SD.end();

}
