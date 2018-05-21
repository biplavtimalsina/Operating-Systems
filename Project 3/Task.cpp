#include "Task.h"
using namespace std;

Task:: Task(string name, int start,int end)
{  
     setName ( name);
     setStartTime ( start);
     setEndTime ( end);   
}



void Task:: setName (string name)
{
this->taskName=name;
}

void Task:: setStartTime(int t)
{
this->startTime=t;
}

void Task:: setEndTime(int t)
{
this->endTime=t;
}


void Task:: printData(void)
{
cout<<"Meta-Data Metrics"<<endl;
cout<<this->taskName<<" "<<this->startTime<<" - " <<this->endTime<<"  ms" <<endl;
}
