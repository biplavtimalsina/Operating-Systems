#ifndef process_H
#define Process_H
#include <queue>
#include "Task.h"
#include <time.h>
#include "ConfigFile.h"
using namespace std;
class Process
{
    int processNumber;
    double startTime, endTime;
    queue<Task> processTaskqueue;
    int status;
    int ioTask;
  public:
        Process(int number,double start,double end,queue<Task> taskQueue, int i,int ioTask);
        void setProcessNumber (int number);
        void setProcessqueue(queue<Task> taskqueue);
        void setStartTime (double t);
        void setEndTime (double t);
        void setStatus(int i);    
        void printData(void);
        void setioTask(int t);
       
        int getProcessNumber () {return this->processNumber;}
        double getStartTime ()   {return this->startTime;}
        double getEndTime ()   {return this->endTime;}
        queue<Task> getTaskQueue(){return this->processTaskqueue;}
        int getStatus(){return this->status;}
        int getioTask(){return ioTask;}
        int gettotalTask(){return this->processTaskqueue.size();}
};

#endif
