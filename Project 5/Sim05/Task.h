#ifndef task_H
#define TASK_H
#include "ConfigFile.h"
using namespace std;

class Task {
    string taskName;
    int startTime, endTime;
    int timeToCompletion;
    bool isInterrupted,isCompleted;

  public:
    Task(string name, int start,int end);
    void setToCompletion(int time);
    void setName (string name);
    void setStartTime (int t);
    void setEndTime (int t);    
    void setInterrupted(bool t);
    void setCompleted(bool t);
    void printData(void);
    string getName () {return this->taskName;}
    int getStartTime ()   {return this->startTime;}
    int getEndTime ()   {return this->endTime;}
    int getTimeToCompletion(){return this->timeToCompletion;}
    bool getInterrupted(){return this->isInterrupted;}
    bool getCompleted(){return this->isCompleted;}

};

#endif

