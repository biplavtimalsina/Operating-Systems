#include "Process.h"
using namespace std;

Process:: Process(int number,double start,double end,queue<Task> taskQueue,int i)
{
    
     
     setProcessNumber ( number);
     setStartTime ( start);
     setEndTime ( end);
     setProcessqueue(taskQueue);
     setStatus(i);
}



void Process:: setProcessNumber (int number)
{
this->processNumber=number;
}

void Process:: setStartTime(double t)
{
this->startTime=t;
}

void Process:: setEndTime(double t)
{
this->endTime=t;
}
void Process::setProcessqueue(queue<Task> taskqueue)
{
this->processTaskqueue=taskqueue;
}

void Process:: setStatus(int i)
{
this->status=i;
}

void Process:: printData(void)
{
cout<<"Meta-Data Metrics"<<endl;
//cout<<this->taskName<<" "<<this->startTime<<" - " <<this->endTime<<"  ms" <<endl;
}
