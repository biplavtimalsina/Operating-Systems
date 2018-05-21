#include "Functions.h"
//queue to store the processes in the mdf
queue<Task> myTaskqueue;
queue<Process>myProcessQueue;
//to store the loging information
ostringstream logStream;
string logString;
//variable to store the process number
int processnumber=0;
//size of the an arry used to store delimeters top spilit the lines in MDF file
const int dictSize = 256; 
const long int toSeconds=1000000;
//Pthreads
pthread_t thread;
pthread_attr_t attr;
int processTime=0;
bool tBool=false;


//Reading the data from MDF file


void readMetadataFile(ConfigFile Configfiledata)
{
		fstream file;
		string line,filename;
    vector<string> v;
    filename=Configfiledata.getmdfFilepath();
    split(filename, '.', v);
		if (v[1].compare("mdf") != 0)
    {
		    cout<<"Meta data File Extension is not correct"<<endl;
        exit(0);
		}
 		file.open(filename.c_str());
	  
    if ( file.peek() == std::ifstream::traits_type::eof() )
		{
   	  	cout<<"Programm Stopped"<<endl;
	      cout<<"MDF file is empty or Missing!"<<endl;
        exit(0);
		}

		if (file.is_open())
    {
		   	while ( getline (file,line) )
        {
		        if (line!="Start Program Meta-Data Code:"&& line!="End Program Meta-Data Code.")
		        {   
		      	     v=tokenizeMyString(line, ".!,;:");
                 //cout<<"v[1]"<<v[1]<< "   V[2]"<< v[2]<<endl;
                 printvector(v,Configfiledata);
            }
        }
		}
		else
    {
		  	cout<<"MDF file cannot be opened"<<endl;
        exit(0);
    }
	  file.close();
}

//Seperating the process name and cycles after picking it from the line and spiliting them based on the delimeters
void printvector(vector<string>& v,ConfigFile Configfiledata)
{
		string name, cycle;
 		vector<string> v1;
		int counter=0;
		for ( unsigned int x = 0; x != v.size(); ++x)
    {
	        split(v[x], '}', v1);
          name=v1[counter];
          counter++;
		    	cycle=v1[counter];
          counter++;
          if (cycle=="")
			    {
			         cout<<"Program terminated"<<endl;
 		           cout<<"Process cycle are missing in Meta data file"<<endl<<endl;	
               exit(0);			
			    }	
          setQueue(name, cycle,Configfiledata);
		}
}

//assigning the names and values to the process and its cycles and putting it in a queue
void setQueue(const  string& name,const  string& cycle,ConfigFile Configfiledata)
{
    int num;
    string c=cycle;
  // cout<<"name"<<name<<endl;
    if("S{start" ==name ||"s{start"==name)
    {
	  }
    else if ("S{end" ==name ||"s{end" ==name)
    {
	  }
    else if("A{start"== name || "a{start" ==name)
    {
          processnumber=processnumber+1;
          myTaskqueue.push(Task("A{start}",0,0));         
	  }
    else if("A{end"== name || "a{end" ==name)
    {
          myTaskqueue.push(Task("A{end}",0,0));         
          myProcessQueue.push( Process(processnumber,0.0,0.0,myTaskqueue,0));
          myTaskqueue=queue<Task>();
	  }
    else if("P{run"== name ||"P{run" ==name)
    {
	        num=getNumber(c);
          myTaskqueue.push(Task("P{run}",num,num*Configfiledata.getProcessor()));
         // cout<<"start="<<num<< "   end time="<<num*Configfiledata.getProcessor()<<endl;
    }
    else if("O{monitor" == name ||"o{monitor" ==name)
    {
          num=getNumber(c);
          myTaskqueue.push(Task("O{monitor}",num,num*Configfiledata.getMonitor()));
    }
    else if("I{hard drive" == name || "i{hard drive" == name)
    {
	        num=getNumber(c);
          myTaskqueue.push(Task("I{hard drive}",num,num*Configfiledata.getHardDrive()));
    }
	  else if("O{hard drive" == name || "o{hard drive" ==name)
    {
           num=getNumber(c);
           myTaskqueue.push(Task("O{hard drive}",num,num*Configfiledata.getHardDrive()));
    }
    else if("O{printer"== name || "o{printer" ==name)
    {
	         num=getNumber(c);
           myTaskqueue.push(Task("O{printer}",num,num*Configfiledata.getPrinter()));
    }
     else if("O{projector"== name || "o{projector" ==name)
    {
           num=getNumber(c);
           myTaskqueue.push(Task("O{projector}",num,num*Configfiledata.getProjector()));
    }
 	  else if("I{keyboard" == name || "i{keyboard" ==name)
    {
           num=getNumber(c);
           myTaskqueue.push(Task("I{keyboard}",num,num*Configfiledata.getKeyboard()));
    }
    else if("M{allocate" == name || "m{allocate" ==name)
    {
	         num=getNumber(c);
           myTaskqueue.push(Task("M{allocate}",num,num*Configfiledata.getMemory()));
    }
    else if("M{block" == name || "m{block" ==name)
    {
           num=getNumber(c);
           myTaskqueue.push(Task("M{block}",num,num*Configfiledata.getMemory()));
    }
    else if("I{mouse" == name || "i{mouse" ==name)
    {
	         num=getNumber(c);
           myTaskqueue.push(Task("I{mouse}",num,num*Configfiledata.getMouse()));
    }
    else if("O{speaker" == name || "O{speaker" ==name)
    {
	         num=getNumber(c);
           myTaskqueue.push(Task("O{speaker}",num,num*Configfiledata.getSpeaker()));
    }
    else
    {
		       cout<<"*Program terminated"<<endl;
 	         cout<<"Process name is missing in Meta data file"<<endl<<endl;	
		       exit(0);
	  } 
	
}

//This function Reads the queue of processes, send them for execution and log the output
void iterateProcessQueue(int logto, string filepath)
{        
    	  logStream<<(double)clock() / toSeconds <<" - Simulator program starting"<<endl;     
        while(!myProcessQueue.empty())
        {    
              Process P = myProcessQueue.front();
              processExecutation(P);
	            myProcessQueue.pop();		
	     	}
        //logStream<<(double)clock() / toSeconds <<" - Simulator program starting ending "<<endl;
	      logString=logStream.str();
        ofstream myfile;
        if(logto==1 ||logto==2)
        {
             myfile.open (filepath.c_str(), ios::app);
             if(logto==1)
             {
                  myfile<<logString<<endl;
             }
             if(logto==2)
             {
                  myfile<<logString<<endl;
                  cout<<logString<<endl;
             }
             myfile.close();
        }
        else if (logto==3)
        {
             cout<<logString<<endl;
        }
}


//This function Reads the queue of tasks in each procee and execute the processes
void processExecutation(Process P)
{
    P.setStatus(1);
    int Pnum=P.getProcessNumber();
    myTaskqueue=P.getTaskQueue();
    string S;
    ostringstream strs,strs2;
    while(!myTaskqueue.empty())
    {
        Task T = myTaskqueue.front();
       // cout<<"T.getName=="<<T.getName()<<endl;
        if("A{start}"== T.getName())
        {
            P.setStatus(2);
   		      logStream<<(double)clock() /toSeconds<<" - OS: preparing process "<<Pnum<<"\n";
        }
        else if("A{end}"== T.getName())
        {
            logStream<<(double)clock() /toSeconds<<" - OS: removing process "<<Pnum<<"\n";
            P.setStatus(4);			 
        }
        else if("P{run}"== T.getName())
        {
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": start Processsing action"<<"\n";
            P.setStatus(3);
            //globalFullCycle=(T.getEndTime()-T.getStartTime());
            globalFullCycle=(T.getEndTime());
            checkFinish();
            P.setStatus(2);
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": end Processsing action"<<"\n";
        }
        else if("O{monitor}"== T.getName())
        {
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": start Monitor output"<<"\n";
            P.setStatus(3);
            pthread_attr_init(&attr);
            //pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
            pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()) );
            pthread_join(thread, NULL);
            P.setStatus(2);
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": end Monitor output"<<"\n";
        }
        else if("I{hard drive}"== T.getName())
        {
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": start hard drive Input"<<"\n";
            P.setStatus(3);
            pthread_attr_init(&attr);
            //pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
            pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()) );

            pthread_join(thread, NULL);
            P.setStatus(2);
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": end hard drive Input"<<"\n";
        }
         else if("O{projector}"== T.getName())
        {
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": start projector output"<<"\n";
            P.setStatus(3);
            pthread_attr_init(&attr);
            //cout<<"T.getEndTime=="<<T.getEndTime()<<endl;
            //pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
            pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()) );

            pthread_join(thread, NULL);
            P.setStatus(2);
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": end projector output"<<"\n";
        }
        else if("O{hard drive}"== T.getName())
        {
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": start hard drive output"<<"\n";			
            P.setStatus(3);
            pthread_attr_init(&attr);
            //pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
            pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()) );

            pthread_join(thread, NULL);
            P.setStatus(2);               
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": end hard drive output"<<"\n";
        }
        else if("O{printer}"== T.getName())
        {
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": start printer output"<<"\n";
            P.setStatus(3);
            pthread_attr_init(&attr);
          //  pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
                        pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()) );

            pthread_join(thread, NULL);
            P.setStatus(2);
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": end printer output"<<"\n";               
        }
        else if("I{keyboard}"== T.getName())
        {
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": start keyboard Input"<<"\n";
            P.setStatus(3);
            pthread_attr_init(&attr);
            //pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
                        pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()) );

            pthread_join(thread, NULL);
            P.setStatus(2);
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": end keyboard Input"<<"\n";
        }
        else if("M{allocate}"== T.getName())
        {
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": allocating memory"<<"\n";
            P.setStatus(3);
            //globalFullCycle=(T.getEndTime()-T.getStartTime());
            globalFullCycle=(T.getEndTime());
            checkFinish();
            P.setStatus(2);
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": memory allocated at 0X"<<std::hex <<allocateMemory(1000000011)<<"\n";
        }
        else if("M{block}"== T.getName())
        {
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": start memory blocking"<<"\n";
            P.setStatus(3);
            //globalFullCycle=(T.getEndTime())-T.getStartTime();
            globalFullCycle=(T.getEndTime());
            checkFinish();
            P.setStatus(2);
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": end memory blocking"<<"\n";
        }
        else if("I{mouse}"== T.getName())
        {
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": start mouse Input"<<"\n";
            P.setStatus(3);
            pthread_attr_init(&attr);
           // pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
                        pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()) );

            pthread_join(thread, NULL);
            P.setStatus(2);
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": end mouse Input"<<"\n";
        }
        else if("O{speaker}"== T.getName())
        {
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": start speaker Input"<<"\n";
            P.setStatus(3);
            pthread_attr_init(&attr);
          // pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
                        pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()) );

            pthread_join(thread, NULL);
            P.setStatus(2);
            logStream<<(double)clock() /toSeconds<<" - Process "<<Pnum <<": end speaker Input"<<"\n";
        }
        else
        {
            cout<<"***Program terminated"<<endl;
            cout<<"Process name is missing in Meta data file"<<endl<<endl;	
            exit(0);
	      } 
    myTaskqueue.pop();		
    }
}


//Thread runner function
void *ThreadProcessing(void *t)
{
     int cycles=*((int*)(&t));
     myWait(cycles);
	   pthread_exit(NULL);
}

//Thread for timer
void *timerThreadProcessing(void *t)
{
  while(1){
  //int cycles=*((int*)(&t));
  int cycles=globalFullCycle;
  for(cycles;cycles>0;cycles--)
  {
    myWaitTimer(1);
  }
  if(cycles==0) 
    {
      tBool=true;
    //cout<<"here"<<endl;
    cycles=-1;
  }
 }

}
void myWaitTimer(int cycles)
{ 
    long k=cycles*1000;
    for (long i=k;i>0;i--);
}

//my Sleep function to stop the execution while processing a task
void myWait(int cycles)
{
     int k=cycles*1000;
     for (int i=0;i<k;i++)
     {
      }
}

void checkFinish()
{
 while(1){
  myWaitTimer(100);

  if(tBool==true){
    tBool=false;
      break;
  }
 }
}


//logging the data to file and /or monitor based on the input in config file
void printQueue(int logto,string filepath,Process P)
{
      int Pnum=P.getProcessNumber();
      myTaskqueue=P.getTaskQueue();
      ofstream myfile;
      if(logto==1 ||logto==2)
      {
           myfile.open (filepath.c_str(), ios::app);
           if(logto==2 )
           {
                cout<<"Meta-Data Metrics"<<endl;
                myfile<<"Logged to = "<<"Monitor and " << filepath<<endl<<endl;	
            }
      
            if(logto==1)
            {
                myfile<<"Logged to = "<<filepath<<endl<<endl;	
            }
            myfile<<endl<<"Meta-Data Metrics"<<endl;
            while(!myTaskqueue.empty())
            {
                Task T = myTaskqueue.front();
                myfile<<T.getName()<<T.getStartTime()<<" - ";
                myfile<<T.getEndTime()<<"  ms"<<" Process "<<Pnum <<endl;
        
                if(logto==2)
                {
                    cout<<T.getName()<<T.getStartTime()<<" - ";
                    cout<<T.getEndTime()<<"  ms"<<" Process "<< Pnum<<endl;
                }
             myTaskqueue.pop();
	          }  
            myfile.close();
      }
}


//converting the cycles read as a string to int and some mendatory checks
int getNumber(const string& s)
{
  	int temp;
    string s1=s;		
    istringstream buffer(s);
    buffer>>temp;
    if (temp>=0)
        return temp;
    else
    {
        cout<<"The cycle time is in negative"<<endl;
        exit(0);
    }
}



//split the line based on the single delimeter
void split(const string& s, char c,vector<string>& v)
{
     //cout<<"string="<<s<<endl;
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
		  else
      {
       		cout<<"Input is not correct for the Split function"<<endl;
          exit(0);                
		  }
}



//spiliting the lines based on multiple values
vector<string> tokenizeMyString(const string &s, const string &del)
{	  
    static bool dict[dictSize] = {false};
    vector<string> res;
    for ( unsigned int i = 0; i < del.size(); ++i) 
    {      
        dict[(unsigned char)del[i]] = true;
    }
    string token("");
   	for ( i : s) 
    {
     		if ((unsigned char) dict[i]) 
        {
        	 if (!token.empty()) 
           {  
      					token=delUnnecessary(token);			
      					res.push_back(token);
                token.clear();
            }           
        }
        else
        {
         		token += i;
        }
    }
   	if (!token.empty()) 
    {
    		if (token!=" ")
      	{
			      token=delUnnecessary(token);
  			    res.push_back(token);	
				}
  	}
  	return res;
}


//Removing the un necessary spaces in the begning and the ending of a file
 string delUnnecessary(string &str)
{
    int size = str.length();
    for(int j = 0; j<=size; j++)
    {
        for(int i = 0; i <=j; i++)
        {
            if(str[i] == ' ' && str[i+1] == ' ')
            {
                str.erase(str.begin() + i);
            }
            else if(str[0]== ' ')
            {
                str.erase(str.begin());
            }
            else if(str[i] == '\0' && str[i-1]== ' ')
            {
                str.erase(str.end() - 1);
            }
        }
    }
    return str;
}
