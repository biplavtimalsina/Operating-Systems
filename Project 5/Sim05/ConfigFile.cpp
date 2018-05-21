#include "ConfigFile.h"

using namespace std;

//Constractor
ConfigFile:: ConfigFile(string filepath,int processor, int monitor, int harddrive,int printer, int keyboard, int memory,
 int mouse, int speaker, int logged,string logfilepath)
{
     setmdfFilepath ( filepath);
     setProcessor (processor);
     setMonitor (monitor);
     setHardDrive (harddrive);
     setPrinter (printer);
     setKeyboard (keyboard);
     setMemory (memory);
     setMouse (mouse);
     setSpeaker (speaker);
     setLogged (logged);
     setlogFilePath (logfilepath);
}

//Constractor which takes the the input file name from commadline as an argunment and reads all the data from cofiguration file 
ConfigFile:: ConfigFile(string Filename)
{
		fstream file;
		string line;
		vector<string> v;
    int counter=0;

  setmdfFilepath ( "a");
     setProcessor (0);
     setMonitor (0);
     setHardDrive (0);
     setPrinter (0);
     setKeyboard (0);
     setMemory (0);
     setMouse (0);
     setSpeaker (0);
     setLogged (0);
     setlogFilePath ("a");
     setProcessorQuantum(0);
     setmemoryBlock(0);
     setScheduling(0);

		file.open(Filename.c_str());//check for empty config file
    //cout<<"Filname:"<< Filename<<endl;
		if ( file.peek() == std::ifstream::traits_type::eof() )
		{
   		cout<<"Program Stopped"<<endl;
	    cout<<"Config file is empty!!"<<endl;
      exit(0);
		}
    //Ignoring the first two line of configuration file
    //cout<<"*here*"<<endl;
		getline (file,line);
    getline (file,line);
		//cout<<"**"<<endl;
    
    //reading remaining lines in dataLinesVector
    if (file.is_open())
    {
		   	while ( getline (file,line) )
        {
			      readData(line);
            counter++;
			  }
		}
    else
    {//terminates if the config file path is not correct
        cout<<"Cannot Open file"<<endl;
        exit(0);
    }
    //cout<<"before close"<<endl;
		file.close();
	  if (counter<12)
    {
       cout<<"******Some lines are missing in config files******"<<endl;
       cout<<"Program ended"<<endl;
	     exit(0);
	  }
}

//this functions takes each line splits it and forwards the result to update final vector function

void ConfigFile::readData(string s)
{
   // cout<<"string:="<< s<<endl;
    vector<string> ofLine;
    split(s, ':', ofLine);
    updateValues(ofLine);
}

//This function assigns the values to all the respective class variables from the config file
void ConfigFile:: updateValues(vector<string>& v)
{    
  	const std::string process = v[0];
    //cout<<"v[0]="<<v[0]<<"   v[1]:"<<v[1]<<endl;
	  string search,s;
	  int temp;    
    if("File Path"==process)
    {
        search = "File Path";
        s=v[1];
	      setmdfFilepath(s.substr( s.find_first_not_of(' '),s.find_last_not_of(' ')));
		}
	  else if("Processor cycle time {msec}"==process)
    {
        search = "Processor";
        istringstream buffer(v[1]);
        buffer>>temp;
        if (temp>0)
        {
           setProcessor (temp);
        }
        else
        {
           cout<<"Programm Stopped"<<endl;
           cout<<"The Processor cycle time is negative"<<endl<<endl; 
           exit(0);
        }
    }
    else if("Monitor display time {msec}"==process)   
    {  
        search = "Monitor";
        istringstream buffer(v[1]);
        buffer>>temp;
        if (temp>0)
        {
           setMonitor (temp);
        }
        else
        {
           cout<<"Programm Stopped"<<endl;
           cout<<"The Monitor display cycle time is negative"<<endl<<endl; 
           exit(0);
        }     
    }
    else if("Hard drive cycle time {msec}"==process)   
    {
        search = "Hard Drive";
         istringstream buffer(v[1]);
         buffer>>temp;
         if (temp>0){
         setHardDrive (temp);
          }
         else{
           cout<<"Programm Stopped"<<endl;
           cout<<"The Hard drive cycle time is negative"<<endl<<endl; 
          exit(0);
            } 
    }    
     else if("System memory {kbytes}"==process)   
    {
         search = "System Memory";
         istringstream buffer(v[1]);
         buffer>>temp;
         if (temp>0)
         {
              setsysMemory(temp);
         }
         else
         {
             cout<<"Programm Stopped"<<endl;
             cout<<"The system memory is negative"<<endl<<endl; 
             exit(0);
         } 
    }  
    else if("Printer cycle time {msec}"==process)   
    {
       search = "Printer";
       istringstream buffer(v[1]);
       buffer>>temp;
       if (temp>0)
       {
          setPrinter (temp);
       }
       else
       {
          cout<<"Programm Stopped"<<endl;
          cout<<"The Printer cycle time is negative"<<endl<<endl; 
          exit(0);
       } 
	   }
     else if("Keyboard cycle time {msec}"==process)   
     { 
	     search = "Keyboard";
       istringstream buffer(v[1]);
       buffer>>temp;
       if (temp>0)
       {
           setKeyboard (temp);
       }
       else
       {
           cout<<"Programm Stopped"<<endl;
           cout<<"The Keyboard cycle time is negative"<<endl<<endl; 
           exit(0);
       }  
     }
     else if("Memory cycle time {msec}"==process)   
     {
          search = "Memory";
	        istringstream buffer(v[1]);
          buffer>>temp;
          if (temp>0)
          {
             setMemory (temp);
          }
          else
          {
             cout<<"Programm Stopped"<<endl;
             cout<<"The Memory cycle time is negative"<<endl<<endl; 
             exit(0);
          }        
	     }
  
      else if("Mouse cycle time {msec}"==process)   
      {
          search = "Mouse";
	        istringstream buffer(v[1]);
          buffer>>temp;
          if (temp>0)
          {
             setMouse (temp);
          }
          else
          {
             cout<<"Programm Stopped"<<endl;
             cout<<"The Mouse cycle time is negative"<<endl<<endl; 
             exit(0);
          }  
      }
       else if("Projector cycle time {msec}"==process)   
      {
          search = "Projector";
          istringstream buffer(v[1]);
          buffer>>temp;
          if (temp>0)
          {
             setProjector (temp);
          }
          else
          {
             cout<<"Programm Stopped"<<endl;
             cout<<"The projector cycle time is negative"<<endl<<endl; 
             exit(0);
          }  
      }
      else if("Speaker cycle time {msec}"==process)   
      {
           search = "Speaker";
	         istringstream buffer(v[1]);
           buffer>>temp;
           if (temp>0)
           {
              setSpeaker (temp);
           }
           else
           {
              cout<<"Programm Stopped"<<endl;
              cout<<"The Speaker cycle time is negative"<<endl<<endl; 
              exit(0);
           }   
       }
       else if("Memory block size {kbytes}"==process)   
       {
          search = "Memory Block";
          istringstream buffer(v[1]);
          buffer>>temp;
          if (temp>0)
          {
             setmemoryBlock (temp);
          }
          else
          {
             cout<<"Programm Stopped"<<endl;
             cout<<"The Memory block size is negative"<<endl<<endl; 
             exit(0);
          }  
        }
        else if("Printer quantity"==process)   
        {
            search = "Printer Quantity";
            istringstream buffer(v[1]);
            buffer>>temp;
            if (temp>0)
            {
                setprinterQty (temp);
            }
            else
            {
                cout<<"Programm Stopped"<<endl;
                cout<<"The Printer quantity is negative"<<endl<<endl; 
                exit(0);
            }   
          }
        else if("Hard drive quantity"==process)   
        {
            search = "Hard Drive Quantity";
            istringstream buffer(v[1]);
            buffer>>temp;
            if (temp>0)
            {
                sethardDriveQty (temp);
            }
            else
            {
                cout<<"Programm Stopped"<<endl;
                cout<<"The Hard drive quantity is negative"<<endl<<endl; 
                exit(0);
            }   
        }
        else if("Projector quantity"==process)   
        {
            //cout<<"yes"<<endl;
            search = "Projector Quantity";
            istringstream buffer(v[1]);
            buffer>>temp;
            if (temp>0)
            {
                setprojectorQty (temp);
            }
            else
            {
                cout<<"Programm Stopped"<<endl;
                cout<<"The projector quantity is negative"<<endl<<endl; 
                exit(0);
            }   
        }
        else if("Scanner cycle time {msec}"==process)   
        {
          //cout<<"yesss"<<endl;
            search = "Scanner";
            //cout<<"***process"<<process<<endl;
            istringstream buffer(v[1]);

            buffer>>temp;
            //cout<<"temp++"<<temp<<endl;
            if (temp>0)
            {
                setScanner(temp);
            }
            else
            {
                cout<<"Programm Stopped"<<endl;
                cout<<"The scanner cycle time is negative"<<endl<<endl; 
                exit(0);
            }   
        }
    else if("Quantum Number {msec}"==process)   
        {
          //cout<<"yesss"<<endl;
            search = "Quantum Number";
            //cout<<"***process"<<process<<endl;
            istringstream buffer(v[1]);

            buffer>>temp;
            //cout<<"temp++"<<temp<<endl;
            if (temp>0)
            {
                setScanner(temp);
            }
            else
            {
                cout<<"Programm Stopped"<<endl;
                cout<<"The scanner cycle time is negative"<<endl<<endl; 
                exit(0);
            }   
        }

        else if("Processor Quantum Number"==process)   
        {
            search = "Quantum";
            istringstream buffer(v[1]);
            buffer>>temp;
            if (temp>0)
            {
                setProcessorQuantum(temp);
            }
            else
            {
                cout<<"Programm Stopped"<<endl;
                cout<<"The processor quantum number is negative"<<endl<<endl; 
                exit(0);
            }   
        }
         else if("CPU Scheduling Code"==process)   
        {
            search = "Scheduling";
            s=v[1]; 
            if ("FIFO"==s.substr(s.find_first_not_of(' '),s.find_last_not_of(' ')))
            {
                setScheduling(1);
            }
            else if ("SJF"==s.substr(s.find_first_not_of(' '),s.find_last_not_of(' ')))
            {
                setScheduling(3);
            }
            else if ("PS"==s.substr(s.find_first_not_of(' '),s.find_last_not_of(' ')))
            {
                setScheduling(2);
            }
            else if ("STR"==s.substr(s.find_first_not_of(' '),s.find_last_not_of(' ')))
            {
                setScheduling(4);
            }
            else if ("RR"==s.substr(s.find_first_not_of(' '),s.find_last_not_of(' ')))
            {
                setScheduling(5);
            }
            else
            {
                cout<<"Programm Stopped"<<endl;
                cout<<"The CPU scheduling type is not known"<<endl<<endl; 
                exit(0);
            }   
        }
       else if("Log"==process)   
       {	
           search = "Logged to";
           s=v[1];  
          if("Log to File"==s.substr( s.find_first_not_of(' '),s.find_last_not_of(' ')))
          {
	           setLogged(1);
          }
          else if("Log to Both"==s.substr( s.find_first_not_of(' '),s.find_last_not_of(' ')))
          {
	           setLogged(2);
          }
          else 
             setLogged(3);
			  }
        else if("Log File Path"==process)   
        {
             search = "Log File Path";
             s=v[1];
             setlogFilePath(s.substr( s.find_first_not_of(' '),s.find_last_not_of(' ')));
	      }
        else if(process.find("End Simulator Configuration File") != std::string::npos)
        {
            return; 
	       }
      this->dataLinesList.push_back(search);
}


void ConfigFile:: split(const string& s, char c,vector<string>& v)//This function takes a string and splits it based on the delimeter and stores the values in the vector which is passed by reference
 {
   string::size_type i = 0;
   string::size_type j = s.find(c);
 if(strchr(s.c_str(), c))
{
while (j != string::npos) {
      v.push_back(s.substr(i, j-i));
      i = ++j;
      j = s.find(c, j);
      if (j == string::npos)
         v.push_back(s.substr(i, s.length()));
   }
    
}
else
 v.push_back(s);
}

//set functions for the class veriables
void ConfigFile:: setmdfFilepath (string mdffilepath)
{
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

void ConfigFile:: setProjector(int projector)
{
this->Projector=projector;
}

void ConfigFile:: setLogged(int logged)
{
this->Logged=logged;
}

void ConfigFile:: setScanner(int scanner)
{
this->Scanner=scanner;
}

void ConfigFile:: setlogFilePath (string logfilePath)
{
this->logFilePath=logfilePath;
}


void ConfigFile:: printdataLinesList(void)
{

}

void ConfigFile::setsysMemory(int sysMemory)
{
    this->sysMemory=sysMemory;
}

void ConfigFile:: setprinterQty(int size)
{
  this->printerQty=size;
}

void ConfigFile:: sethardDriveQty(int size)
{
  this->hardDriveQty=size;
}

void ConfigFile:: setmemoryBlock(int size)
{
  this->memoryBlock=size;
}

void ConfigFile:: setprojectorQty(int size)
{
  this->projectorQty=size;
}

//Processor, Monitor,HardDrive, Printer,Keyboard,Memory, Mouse,Speaker, Logged
void ConfigFile::setScheduling(int scheduling)
{
  this->scheduling=scheduling;
}

void ConfigFile::setProcessorQuantum(int size)
{
  this->processorQuantum=size;
}


void ConfigFile::setQuantumNumber(int t)
{
  this->quantumNumber=t;
}

void ConfigFile::printData(void)
{
  vector<string> v;
  string outputFile;
  ofstream myfile;
  //cout<<"***"<<endl;
  //cout<<"filename="<<this->logFilePath<<endl;
  outputFile=this->logFilePath;
  //cout<<"logged value="<<this->getLogged()<<endl;

  if(this->Logged==1 )
  {
    //cout<<"*"<<endl;
      myfile.open (outputFile.c_str());
      myfile<<"Configuration File Data"<<endl;
      myfile<<"Monitor = " << this->Monitor<<" ms/cycle"<<endl;
      myfile<<"Processor = "<< this->Processor <<" ms/cycle" <<endl;
      myfile<<"Scanner = "<<this->Scanner <<" ms/cycle"<<endl;
      myfile<<"Hard Drive ="<< this->HardDrive<<" ms/cycle"<< endl;
      myfile<<"Keyboard = " << this->Keyboard<<" ms/cycle"<<endl;
      myfile<<"Memory = " <<this->Memory<<" ms/cyce"<<endl;
      myfile<<"Projector = "<<this->Projector<<" ms/cycle"<<endl;
      myfile<<"System Memory = "<<this->sysMemory<<endl;
      myfile<<"Scheduling="<<this->scheduling<<endl;
      myfile<<"Memory block size (kbytes)= "<< this->memoryBlock<<endl;
      myfile<<"Processor Quantum Number="<<this->processorQuantum<<endl;
     // myfile<<"Printer quantity: "<< this->printerQty<<endl;
      myfile<<"Hard drive quantity: "<<this->hardDriveQty<<endl;
      myfile<<"Projector quantity: "<< this->projectorQty<<endl;
      myfile<<"Logged to = " << this->logFilePath<<endl<<endl;
      myfile.close();   
  }
  if (this->Logged==2) 
  {
        //cout<<"**"<<endl;

      myfile.open (outputFile.c_str());
      myfile<<"Configuration File Data"<<endl;
      myfile<<"Monitor = " << this->Monitor<<" ms/cycle"<<endl;
      myfile<< "Processor = "<< this->Processor <<" ms/cycle" <<endl;
      myfile<< "Scanner = "<<this->Scanner <<" ms/cycle"<<endl;
      myfile<< "Hard Drive ="<< this->HardDrive<<" ms/cycle"<< endl;
      myfile<<"Keyboard = " << this->Keyboard<<" ms/cycle"<<endl;
      myfile<<"Memory = " <<this->Memory<<" ms/cyce"<<endl;
      myfile<<"Projector = "<<this->Projector<<" ms/cycle"<<endl;
      myfile<<"System Memory = "<<this->sysMemory<<endl;
      myfile<<"Scheduling="<<this->scheduling<<endl;
      myfile<<"Processor Quantum Number="<<this->processorQuantum<<endl;

      myfile<<"Memory block size (kbytes)= "<< this->memoryBlock<<endl;
      //myfile<<"Printer quantity: "<< this->printerQty<<endl;
      myfile<<"Hard drive quantity: "<<this->hardDriveQty<<endl;
      myfile<<"Projector quantity: "<< this->projectorQty<<endl;
      myfile<<"Logged to = "<<"Monitor and " << this->logFilePath<<endl<<endl;
      myfile.close();   

      
      cout<<"Configuration File Data"<<endl;
      cout<<"Monitor = " << this->Monitor<<" ms/cycle"<<endl;
      cout<< "Processor = "<< this->Processor <<" ms/cycle" <<endl;
      cout<< "Scanner = "<<this->Scanner <<" ms/cycle"<<endl;
      cout<< "Hard Drive ="<< this->HardDrive<<" ms/cycle"<< endl;
      cout<<"Keyboard = " << this->Keyboard<<" ms/cycle"<<endl;
       cout<<"Memory = " <<this->Memory<<" ms/cyce"<<endl;
      cout<<"Processor Quantum Number="<<this->processorQuantum<<endl;

      cout<<"Projector = "<<this->Projector<<" ms/cycle"<<endl;
      cout<<"System Memory = "<<this->sysMemory<<endl;
      cout<<"Memory block size (kbytes)= "<< this->memoryBlock<<endl;
      cout<<"Scheduling="<<this->scheduling<<endl;
    //cout<<"Printer quantity: "<< this->printerQty<<endl;
      cout<<"Hard drive quantity: "<<this->hardDriveQty<<endl;
      cout<<"Projector quantity: "<< this->projectorQty<<endl;
      cout<<"Logged to = "<<"Monitor and " << this->logFilePath<<endl<<endl;
  }

  if(this->Logged==3)
  {
      //cout<<"***"<<endl;

      cout<<"Configuration File Data"<<endl;
      cout<<"Monitor = " << this->Monitor<<" ms/cycle"<<endl;
      cout<< "Processor = "<< this->Processor <<" ms/cycle" <<endl;
      cout<< "Scanner = "<<this->Scanner <<" ms/cycle"<<endl;
      cout<< "Hard Drive ="<< this->HardDrive<<" ms/cycle"<< endl;
      cout<<"Keyboard = " << this->Keyboard<<" ms/cycle"<<endl;
      cout<<"Memory = " <<this->Memory<<" ms/cyce"<<endl;
      cout<<"Projector = "<<this->Projector<<" ms/cycle"<<endl;
      cout<<"System Memory = "<<this->sysMemory<<endl;
      cout<<"Scheduling="<<this->scheduling<<endl;
      cout<<"Processor Quantum Number="<<this->processorQuantum<<endl;
      cout<<"Memory block size (kbytes)= "<< this->memoryBlock<<endl;
    //cout<<"Printer quantity: "<< this->printerQty<<endl;
      cout<<"Hard drive quantity: "<<this->hardDriveQty<<endl;
      cout<<"Projector quantity: "<< this->projectorQty<<endl;
      cout<<"Logged to = "<<"Monitor" <<endl;
  }

}