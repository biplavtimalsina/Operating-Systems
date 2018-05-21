//#include "Process.h"
#include "configFile.h"
using namespace std;

void split(const string& s, char c, vector<string>& v);
void readMetadataFile(ConfigFile Configfile);
vector<string> tokenizeMyString(const string &s, const string &del);
string delUnnecessary(string &str);
void operate(string token, string readMetadataFile, int log, ConfigFile configfile);
