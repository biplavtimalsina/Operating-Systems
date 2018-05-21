#include "Process.h"
using namespace std;

Process:: Process(int number,double start,double end,queue<Task> taskQueue,int i,int ioTask)
{
    
     
     setProcessNumber ( number);
     setStartTime ( start);
     setEndTime ( end);
     setProcessqueue(taskQueue);
     setStatus(i);
     setioTask(ioTask);
     setTotalTask(taskQueue.size());
     setRemainingTask(taskQueue.size());

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

void Process:: setioTask(int t)
{
	this->ioTask=t;
}

void Process::setTotalTask(int t)
{
	this->totalTask=t;
}


void Process::decreaseRemainingTask()
{
	if(this->remainingTask>0)
		this->remainingTask--;
}

void Process::setRemainingTask(int t)
{
	this->remainingTask=t;
}

void Process::setInterrupted(bool t)
{
     this->isInterrupted=t;
}
