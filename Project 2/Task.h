#ifndef task_H
#define TASK_H
#include "ConfigFile.h"
using namespace std;

class Task {
     string taskName;
    int startTime, endTime;
  public:
    Task(string name, int start,int end);
    void setName (string name);
    void setStartTime (int t);
    void setEndTime (int t);    
    void printData(void);
    string getName () {return this->taskName;}
    int getStartTime ()   {return this->startTime;}
    int getEndTime ()   {return this->endTime;}
};

#endif

