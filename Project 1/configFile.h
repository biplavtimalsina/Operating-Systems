#ifndef CONFIGFILE_H
#define CONFIGFILE_H
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<stdlib.h>
#include<string.h>
#include<map>
#include<list>

using namespace std;

class ConfigFile{
	int Monitor,Processor, HardDrive,Projector,Scanner,Printer,Keyboard,Mouse,Speaker, Logged, Memory;
	string logFilePath,mdfFilePath;
	std::list<string> dataLinesList;

public:
	ConfigFile(string filename);
	ConfigFile(string mdfFilePath,int Monitor, int Processor, int HardDrive,int Keyboard,int Mouse, int Speaker, int Logged, string logFilePath);
	void setProcessor(int time);
	void setMonitor(int time);
	void setHardDrive(int time);
	void setPrinter(int time);
	void setKeyboard(int time);
	void setMouse(int time);
	void setSpeaker(int time);
	void setLogged(int time);
	void setMemory(int time);
	void setProjector(int time);
	void setScanner(int time);
	void setmdfFilePath(string mdfFilePath);
	void setlogFilePath(string setlogFilePath);

	int getProcessor(){return this->Processor;}
	int getMonitor ()   {return this->Monitor;}
    int getHardDrive () {return this->HardDrive ;}
    int getPrinter ()   {return  this->Printer;}
    int getKeyboard ()  {return this->Keyboard;}
    int getMouse ()     {return  this->Mouse; }
    int getSpeaker ()   {return this->Speaker;}
    int getLogged ()    {return this->Logged; }
    int getMemory(){return this->Memory;}
    int getScanner(){return this->Scanner;}
    int getProjector(){return this->Projector;}
    string getmdfFilepath(){return this->mdfFilePath;}
    string getlogFilePath(){return this->logFilePath;}
    void printData(void);
    void extract(string s);
    void updateValues(vector<string>& v);
    void split(const string& s, char c,vector<string>& v);



};

#endif
