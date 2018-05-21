#include <unistd.h>
#include "ConfigFile.h"
#include "Process.h"
#include "MemoryFunction.h"
#include <pthread.h>
#include <stdint.h>
#include <semaphore.h>
#include <algorithm>
#include <vector>
//#include "mutex.h"


using namespace std;
//definitions of the functions
void Filereading ();
void readMetadataFile(ConfigFile Configfiledata);
void split(const string& s, char c, vector<string>& v);
void printvector(vector<string>& v,ConfigFile Configfiledata);
vector<string> tokenizeMyString(const string &s, const string &del);
void setQueue(const string& s1, const string& s2,ConfigFile Configfiledata);
void iterateProcessQueue(int logto, string filepath, ConfigFile Configfiledata);
void printQueue(int logto, string filepath,Process P);
void processExecutation(Process P);
int getNumber(const string& s);
void myWait(int cycles);
void *ThreadProcessing(void *t);
string delUnnecessary(string &str);
void *timerThreadProcessing(void *t);
void checkFinish();
void myWait(int cycles);
void myWaitTimer(int cycles);
  int allocateHardDrive();
    int allocatePrinter();
    
