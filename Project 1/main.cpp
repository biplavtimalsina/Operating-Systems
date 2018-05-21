
#include "functions.h"

using namespace std;


int main(int argc, char *argv[])
{
	vector<string> filename;
	split(argv[1],'.',filename);
	if(filename[1].compare("conf")!=0){
		cout<<"File extension is incorrect"<<endl;
		cout<<"reached 1"<<endl;
		exit(0);
	}

	ConfigFile configfile(argv[1]);
	configfile.printData();
	//cout<<"main file=="<<configfile.getmdfFilepath()<<endl;
	//cout<<"here1"<<endl;
	readMetadataFile(configfile);
	


	return 0;
}

