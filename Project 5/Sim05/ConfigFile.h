#ifndef CONFIGFILE_H
#define CONFIGFILE_H
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <sstream>
#include <stdlib.h>
//header file for the the configuration file
using namespace std;

extern pthread_t timerThread;
extern pthread_attr_t timerAttr;
extern int globalFullCycle;
extern bool tBool;
extern int whichThreadReading;
extern string filenameee;
extern int isRR;

class ConfigFile 
{
    int Processor, quantumNumber,Monitor,HardDrive,scheduling, Printer,Keyboard,Memory, Mouse,Speaker, Logged,Projector,Scanner,sysMemory,printerQty, hardDriveQty, memoryBlock, projectorQty, processorQuantum;
    string mdfFilePath,logFilePath;
    vector<string> dataLinesList;
public:
    ConfigFile(string fileName);
    ConfigFile(string mdffilepath,int processor, int Monitor, int harddrive,int printer, int keyboard, int memory,
         int mouse, int speaker, int logged,string logfilepath);
    void setProcessor(int time);
    void setMonitor(int time);
    void setHardDrive(int time);
    void setPrinter(int time);
    void setKeyboard(int time);
    void setMouse(int time);
    void setQuantumNumber(int time);
    void setSpeaker(int time);
    void setLogged(int time);
    void setMemory(int time);
    void setProjector(int time);
    void setScanner(int time);
    void setsysMemory(int size);
    void setmdfFilepath(string mdffilePath);
    void setlogFilePath(string logfilePath);
    void setScheduling(int scheduling);
    void setprinterQty(int size);
    void sethardDriveQty(int size);
    void setmemoryBlock(int size);
    void setprojectorQty(int size);
    void setProcessorQuantum(int size);

    int getQuantumNumber(){return this->quantumNumber;}
    int getProcessor () {return this->Processor;}
    int getMonitor ()   {return this->Monitor;}
    int getHardDrive () {return this->HardDrive ;}
    int getPrinter ()   {return  this->Printer;}
    int getKeyboard ()  {return this->Keyboard;}
    int getMemory ()    {return  this->Memory; }
    int getMouse ()     {return  this->Mouse; }
    int getSpeaker ()   {return this->Speaker;}
    int getLogged ()    {return this->Logged; }
    int getScanner(){return this->Scanner;}
    int getProjector(){return this->Projector;}
     int getsysMemory(){return this->sysMemory;}
    string getmdfFilepath(){return this->mdfFilePath;}
    string getlogFilePath(){return this->logFilePath;}
    int getScheduling(){return this->scheduling;}
    void split(const string& s, char c,vector<string>& v);
    void printData(void);
    void readData(string s); 
   void updateValues(vector<string>& v);
   void printdataLinesList();

    int getprinterQty(){return this->printerQty;}
   int gethardDriveQty(){return this->hardDriveQty;}
   int getmemoryBlock(){return this->memoryBlock;}
   int getprojectorQty(){return this->projectorQty;}
   int getProcessorQuantum(){return this->processorQuantum;}
};


#endif
