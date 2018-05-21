#include "Functions.h"
pthread_t timerThread;
pthread_attr_t timerAttr;
int globalFullCycle=600;


int main(int argc, char *argv[])
{
  pthread_attr_init(&timerAttr);
  pthread_create(&timerThread, &timerAttr, timerThreadProcessing, (void*)globalFullCycle);

  vector<string> filename;
  split(argv[1], '.', filename);
  if (filename[1].compare("conf") != 0)
  {
	   cout<<"File Extension is not correct"<<endl;
     exit (0);
	}
  ConfigFile Configfiledata(argv[1]);
  //cout<<"clock static:"<< ((float)startTime)/1000000<<"*****"<<endl;


  //cout<<"hi"<<endl;
  //Configfiledata.printData();
  //cout<<"hello"<<endl;
  //cout<<"main file=="<<configfile.getmdfFilepath()<<endl;
  //cout<<"here1"<<endl;

  readMetadataFile(Configfiledata);
iterateProcessQueue(Configfiledata.getLogged(),Configfiledata.getlogFilePath(), Configfiledata);
  return 0;
}

