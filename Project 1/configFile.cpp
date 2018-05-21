#define _GLIBCXX_USE_CXX11ABI 0
#include "configFile.h"

using namespace std;

//Constructor
ConfigFile::ConfigFile(std::string mdfFilePath,int Monitor, int Processor,int HardDrive,int Keyboard,int Mouse, int Speaker, int Logged, std::string logFilePath)
{
	setmdfFilePath(mdfFilePath);
	setMonitor(Monitor);
	setProcessor(Processor);
	setHardDrive(HardDrive);
	setKeyboard(Keyboard);
	setMouse(Mouse);
	setSpeaker(Speaker);
	setLogged(Logged);
	setlogFilePath(logFilePath);
}

ConfigFile:: ConfigFile(string Filename)
{
	fstream file;
	
	string line;
	
	file.open(Filename.c_str());
	int count; 	//to track the number of lines read

	//checking to see if config file is empty
	if ( file.peek() == std::ifstream::traits_type::eof() )
		{
   		cout<<"Program Stopped"<<endl;
	       cout<<"Config file is empty!!"<<endl;
               exit(0);
		}

	//ignore first two lines of configuration files because they are not useful
		//cout<<"*here*"<<endl;
	getline (file,line);
		getline(file,line);

	//cout<<"**"<<endl;
	//reading remaining lines in dataLinesVector
	if(file.is_open())
	{
		while(getline(file,line))
		{

			dataLinesList.push_back(line);
			//cout<<"***";
			
			//cout<<"______line_____"<<line<<endl;
			extract(line);
			count++;
			//cout<<"count"<<count<<endl;
		}
	}
	else
	{
		cout<<"Cannot Open file"<<endl;
		exit(0);
	}
	//cout<<"before close"<<endl;
	file.close();
	//cout<<"here"<<endl;
	if(count<11) 
	{
		cout<<"******Some lines are missing in config files******"<<endl;
		cout<<"Program ended"<<endl;
		exit(0);
	}


}

void ConfigFile:: extract(string s)
{
	vector<string> ofLine;
	//cout<<"after vector"<<endl;
	if(s!="End Simulator Configuration File")
	{
		split(s,':',ofLine);

	//cout<<"oh yeah";
	updateValues(ofLine);		
	//cout<<"no"<<endl;
	}
}

void ConfigFile::updateValues(vector<string>& v)
{
	const std::string id=v[0];
	//cout<<"id="<<id<<endl;
	string tempString;
	int temp;
	if("File Path"==id)
	{
		
		stringstream value(v[1]);
		//cout<<"File Path="<<v[1]<<endl;
		setmdfFilePath(v[1].substr(v[1].find_first_not_of(' '),v[1].find_last_not_of(' ')));
	}
	else if ("Processor cycle time {msec}"==id)
	{
		//cout<<"Processor"<<endl;
		stringstream geek(v[1]);
		geek>>temp;
		setProcessor(temp);
	}
	else if ("Monitor display time {msec}"==v[0])
	{
		//cout<<"Monitor"<<endl;
		stringstream geek(v[1]);
		geek>>temp;		
		setMonitor(temp);
	}
	else if("Hard drive cycle time {msec}"==v[0])
	{
		//cout<<"Harddrive"<<endl;
		stringstream geek(v[1]);
		geek>>temp;
		setHardDrive(temp);
	}
	else if("Printer cycle time {msec}"==v[0])
	{
		//cout<<"PRinter"<<endl;
		stringstream geek(v[1]);
		geek>>temp;
		setPrinter(temp);
	}
	else if("Scanner cycle time {msec}"==v[0])
	{
		//cout<<"Scanner"<<endl;
		stringstream geek(v[1]);
		geek>>temp;
		setScanner(temp);
	}
	else if("Projector cycle time {msec}"==v[0])
	{
		//cout<<"Projector"<<endl;
		stringstream geek(v[1]);
		geek>>temp;
		setProjector(temp);
	}
	else if("Keyboard cycle time {msec}"==v[0])
	{
		//cout<<"Keyboard"<<endl;
		stringstream geek(v[1]);
		geek>>temp;
		setKeyboard(temp);
	}
	else if("Memory cycle time {msec}"==v[0])
	{
		//cout<<"Memory"<<endl;
		stringstream geek(v[1]);
		geek>>temp;
		setMemory(temp);
	}
	else if("Mouse cycle time {msec}"==v[0])
	{
		//cout<<"Mouse"<<endl;
		stringstream geek(v[1]);
		geek>>temp;
		setMouse(temp);
	}
	else if("Speaker cycle time {msec}"==v[0])
	{
		//cout<<"Speaker"<<endl;
		stringstream geek(v[1]);
		geek>>temp;
		setSpeaker(temp);
	}
	else if("Log"==v[0])
	{
		//cout<<"Log"<<endl;
		tempString=v[1];
		//cout<<"tempString="<<tempString<<endl;
		tempString=tempString.substr(tempString.find_first_not_of(' '),tempString.find_last_not_of(' '));
		//cout<<"tempstring="<<tempString<<endl;
		if("Log to File"==tempString)
		{
			setLogged(1);
			//cout<<"logged to file"<<endl;
		}
		else if("Log to Both"==tempString or "log to both"==tempString)
		{
			setLogged(2);
			//cout<<"logged to both"<<endl;
		}
		else
		{
			setLogged(3);
			//cout<<"logged to other"<<endl;
		}
		

	}
	else if ("Log File Path"==v[0])
	{
		//cout<<"Log File path"<<endl;
		setlogFilePath(v[1].substr(id.find_first_not_of(' '),id.find_last_not_of(' ')));
		//cout<<"second last line"<<endl;
	}
	else if(v[0].find("End Simulator Configuration File") != std::string::npos){
       cout<<"End Simulator Configuration"<<endl;
         return; 
	}	
	

}

void ConfigFile::printData(void)
{
	vector<string> v;
	string outputFile;
	ofstream myfile;
	//cout<<"filename="<<this->logFilePath<<endl;
	outputFile=this->logFilePath;
	//cout<<"logged value="<<this->getLogged()<<endl;

	if(this->Logged==1 )
	{
		//cout<<"*"<<endl;
	      myfile.open (outputFile.c_str());
		  myfile<<"Configuration File Data"<<endl;
		  myfile<<"Monitor = " << this->Monitor<<" ms/cycle"<<endl;
		  myfile<< "Processor = "<< this->Processor <<" ms/cycle" <<endl;
		  myfile<< "Scanner = "<<this->Scanner <<" ms/cycle"<<endl;
		  myfile<< "Hard Drive ="<< this->HardDrive<<" ms/cycle"<< endl;
		  myfile<<"Keyboard = " << this->Keyboard<<" ms/cycle"<<endl;
		  myfile<<"Memory = " <<this->Memory<<" ms/cyce"<<endl;
		  myfile<<"Projector = "<<this->Projector<<" ms/cycle"<<endl;
		  myfile<<"Logged to = " << this->logFilePath<<endl<<endl;
  		  myfile.close();		
	}
	if (this->Logged==2) 
	{
			//	cout<<"**"<<endl;

		  myfile.open (outputFile.c_str());
		  myfile<<"Configuration File Data"<<endl;
		  myfile<<"Monitor = " << this->Monitor<<" ms/cycle"<<endl;
		  myfile<< "Processor = "<< this->Processor <<" ms/cycle" <<endl;
		  myfile<< "Scanner = "<<this->Scanner <<" ms/cycle"<<endl;
		  myfile<< "Hard Drive ="<< this->HardDrive<<" ms/cycle"<< endl;
		  myfile<<"Keyboard = " << this->Keyboard<<" ms/cycle"<<endl;
  		  myfile<<"Memory = " <<this->Memory<<" ms/cyce"<<endl;
		  myfile<<"Projector = "<<this->Projector<<" ms/cycle"<<endl;
		  myfile<<"Logged to = "<<"Monitor and " << this->logFilePath<<endl<<endl;
	      myfile.close();		

		  
		  cout<<"Configuration File Data"<<endl;
		  cout<<"Monitor = " << this->Monitor<<" ms/cycle"<<endl;
		  cout<< "Processor = "<< this->Processor <<" ms/cycle" <<endl;
		  cout<< "Scanner = "<<this->Scanner <<" ms/cycle"<<endl;
		  cout<< "Hard Drive ="<< this->HardDrive<<" ms/cycle"<< endl;
		  cout<<"Keyboard = " << this->Keyboard<<" ms/cycle"<<endl;
  		  cout<<"Memory = " <<this->Memory<<" ms/cyce"<<endl;
		  cout<<"Projector = "<<this->Projector<<" ms/cycle"<<endl;
		  cout<<"Logged to = "<<"Monitor and " << this->logFilePath<<endl<<endl;
	}

	if(this->Logged==3)
	{
			//	cout<<"***"<<endl;

		  cout<<"Configuration File Data"<<endl;
		  cout<<"Monitor = " << this->Monitor<<" ms/cycle"<<endl;
		  cout<< "Processor = "<< this->Processor <<" ms/cycle" <<endl;
		  cout<< "Scanner = "<<this->Scanner <<" ms/cycle"<<endl;
		  cout<< "Hard Drive ="<< this->HardDrive<<" ms/cycle"<< endl;
		  cout<<"Keyboard = " << this->Keyboard<<" ms/cycle"<<endl;
  		  cout<<"Memory = " <<this->Memory<<" ms/cyce"<<endl;
		  cout<<"Projector = "<<this->Projector<<" ms/cycle"<<endl;
		  cout<<"Logged to = "<<"Monitor" <<endl;
	}

}
void ConfigFile:: split(const string& s, char c,vector<string>& v)//This function takes a string and splits it based on the delimeter and stores the values in the vector which is passed by reference
{
   string::size_type i = 0;
   string::size_type j = s.find(c);
 	if(strchr(s.c_str(), c))
	{
		while (j != string::npos)
		{
      		v.push_back(s.substr(i, j-i));
      		i = ++j;
      		j = s.find(c, j);
      		if (j == string::npos)
        		v.push_back(s.substr(i, s.length()));
   		}
    
	}
}

void ConfigFile:: setmdfFilePath (string mdffilepath)
{
	//cout<<"inside func="<<mdffilepath<<endl;
	this->mdfFilePath=mdffilepath;
}

void ConfigFile:: setProcessor (int processor)
{
this->Processor=processor;
}

void ConfigFile:: setMonitor (int monitor)
{
this->Monitor=monitor;
}


void ConfigFile:: setHardDrive (int harddrive)
{
this->HardDrive=harddrive;
}

void ConfigFile:: setPrinter(int printer)
{
this->Printer=printer;
}

void ConfigFile:: setKeyboard(int keyboard)
{
this->Keyboard=keyboard;
}

void ConfigFile:: setMemory(int memory)
{
this->Memory=memory;
}

void ConfigFile:: setMouse( int mouse)
{
this->Mouse=mouse;
}

void ConfigFile:: setSpeaker(int speaker)
{
this->Speaker=speaker;
}

void ConfigFile:: setLogged(int logged)
{
this->Logged=logged;
}

void ConfigFile:: setlogFilePath (string logfilePath)
{
this->logFilePath=logfilePath;
}

void ConfigFile::setProjector(int time)
{
	this->Projector=time;
}
void ConfigFile::setScanner(int time)
{
	this->Scanner=time;
}