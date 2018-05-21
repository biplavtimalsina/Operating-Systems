#include "Functions.h"
//queue to store the processes in the mdf
queue<Task> myTaskqueue;
queue<Task> reloadTaskqueue;
queue<Process>myProcessQueue;
queue<Process>reloadProcessQueue;
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


pthread_t quantumThread;
pthread_attr_t quantumThreadAttr;


int processTime=0;
bool tBool=false;
int ioTask=0;

float globalTimeToCompletion=0;
int isRR=0;
int quantumNumber=0;

sem_t booleanVariables;





long int totalMemory=0,usedMemory=0, blocksize=0;
int usedProjectors=0,totalProjectors=0, usedHD=0,totalHD=0;

int whichThreadReading=1; //1 for main thread, else load thread
//ConfigFile Configfiledat("reload");

bool newProcessLoaded=false;
bool isTaskInterrupted=false;
bool quantumChange=false;


void *reloadMetaDataThread(void *t)
{
  myWait(100);
  whichThreadReading=2;
  for(int i=0;i<10;i++)
  {
    ConfigFile configfiledata1(filenameee);
    readMetadataFile(configfiledata1);
    //until this point, reloadqueue is ready to be loaded in main queue
    newProcessLoaded=true;


    myWait(100);
   // newProcessLoaded=true;

  }
  newProcessLoaded=false;
  logStream<<"All processes loaded"<<endl;
}

//Allocating Memory to the Process
string allocateMemory()
  {
      ostringstream str1;
      string s;
      if(blocksize>totalMemory-usedMemory)
      {
          usedMemory=blocksize;
          str1<<std::hex <<(blocksize-usedMemory);
          s=str1.str();
          for (int i=s.length();i<8;i++)
              s="0"+s; 
          return s;
      }
      str1<<std::hex <<usedMemory;
      s=str1.str();
      for (int i=s.length();i<8;i++)
          s="0"+s;                
      usedMemory=usedMemory+blocksize;
      return s;
}
//Allocating the Hard Drive to Process
int allocateHardDrive()
{
    usedHD++;
    if (usedHD >totalHD)
    {
        usedHD=1;
        
    }
    return usedHD-1;
}
// Allocating the Printer to the Process
int allocateProjector()
{
    usedProjectors++;
    if (usedProjectors >totalProjectors)
    {
        usedProjectors=1;
    }
    return usedProjectors-1;
}
//Reading the data from MDF file
void readMetadataFile(ConfigFile Configfiledata)
{
		fstream file;
		string line,filename;
    //Configfiledata=Configfiledata1;
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
    //cout<<"Process queue size:"<< myProcessQueue.size()<<endl;
    for(int i=0;i<myProcessQueue.size();i++)
    {
        //cout<<"Process queue size:"<< myProcessQueue.get
    }
}

//assigning the names and values to the process and its cycles and putting it in a queue
void setQueue(const  string& name,const  string& cycle,ConfigFile Configfiledata)
{
    int num;
    string c=cycle;
    quantumNumber=Configfiledata.getQuantumNumber();
    //cout<<"name"<<name<<endl;
    if("S{begin" ==name ||"s{begin"==name)
    {
	  }
    else if ("S{finish" ==name ||"s{finish" ==name)
    {
	  }
    else if("A{begin"== name || "a{begin" ==name)
    {
          processnumber=processnumber+1;
          //cout<<"Processnumber:"<<processnumber<<endl;
          if(whichThreadReading==1)
            myTaskqueue.push(Task("A{start}",0,0));         
	        else
            reloadTaskqueue.push(Task("A{start}",0,0)); 

    }
    else if("A{finish"== name || "a{finish" ==name)
    {
          if(whichThreadReading==1)
          {
            myTaskqueue.push(Task("A{end}",0,0));         
            myProcessQueue.push( Process(processnumber,0.0,0.0,myTaskqueue,0,ioTask));
            myTaskqueue=queue<Task>();
          }else
          {
            reloadTaskqueue.push(Task("A{end}",0,0));         
            reloadProcessQueue.push( Process(processnumber,0.0,0.0,reloadTaskqueue,0,ioTask));
            reloadTaskqueue=queue<Task>();
          }
          
          ioTask=0;
          //cout<<"myTaskQueue size: of Process"<< processnumber << " is "<< myTaskqueue.size()<<endl;
          
	  }
    else if("P{run"== name ||"p{run" ==name)
    {
          num=getNumber(c);
          if(whichThreadReading==1)
          {
            myTaskqueue.push(Task("P{run}",num,num*Configfiledata.getProcessor()));
          }else
          {
            reloadTaskqueue.push(Task("P{run}",num,num*Configfiledata.getProcessor()));
           
          }
         // cout<<"start="<<num<< "   end time="<<num*Configfiledata.getProcessor()<<endl;
    }
    else if("O{monitor" == name ||"o{monitor" ==name)
    {
          num=getNumber(c);
          if(whichThreadReading==1)
            myTaskqueue.push(Task("O{monitor}",num,num*Configfiledata.getMonitor()));
          else
            reloadTaskqueue.push(Task("O{monitor}",num,num*Configfiledata.getMonitor()));
          ioTask++;
    }
    else if("I{hard drive" == name || "i{hard drive" == name)
    {
	        num=getNumber(c);
          if(whichThreadReading==1)
            myTaskqueue.push(Task("I{hard drive}",num,num*Configfiledata.getHardDrive()));
          else
            reloadTaskqueue.push(Task("I{hard drive}",num,num*Configfiledata.getHardDrive()));
          ioTask++;
    }
	  else if("O{hard drive" == name || "o{hard drive" ==name)
    {
           num=getNumber(c);
           if(whichThreadReading==1)
            myTaskqueue.push(Task("O{hard drive}",num,num*Configfiledata.getHardDrive()));
           else
            reloadTaskqueue.push(Task("O{hard drive}",num,num*Configfiledata.getHardDrive()));
           ioTask++;
    }
    else if("O{printer"== name || "o{printer" ==name)
    {
	         num=getNumber(c);
           if(whichThreadReading==1)
            myTaskqueue.push(Task("O{printer}",num,num*Configfiledata.getPrinter()));
           else
            reloadTaskqueue.push(Task("O{printer}",num,num*Configfiledata.getPrinter()));

           ioTask++;
    }
     else if("O{projector"== name || "o{projector" ==name)
    {
           num=getNumber(c);
           if(whichThreadReading==1)
            myTaskqueue.push(Task("O{projector}",num,num*Configfiledata.getProjector()));
           else
            reloadTaskqueue.push(Task("O{projector}",num,num*Configfiledata.getProjector()));
           ioTask++;
    }
 	  else if("I{keyboard" == name || "i{keyboard" ==name)
    {
           num=getNumber(c);
           if(whichThreadReading==1)
            myTaskqueue.push(Task("I{keyboard}",num,num*Configfiledata.getKeyboard()));
           else
            reloadTaskqueue.push(Task("I{keyboard}",num,num*Configfiledata.getKeyboard()));
           ioTask++;
    }
    else if("M{allocate" == name || "m{allocate" ==name)
    {
	         num=getNumber(c);
           if(whichThreadReading==1)
            myTaskqueue.push(Task("M{allocate}",num,num*Configfiledata.getMemory()));
           else
            reloadTaskqueue.push(Task("M{allocate}",num,num*Configfiledata.getMemory()));

    }
    else if("M{block" == name || "m{block" ==name)
    {
           num=getNumber(c);
           if(whichThreadReading==1)
            myTaskqueue.push(Task("M{block}",num,num*Configfiledata.getMemory()));
           else
            reloadTaskqueue.push(Task("M{block}",num,num*Configfiledata.getMemory()));

    }
    else if("I{mouse" == name || "i{mouse" ==name)
    {
	         num=getNumber(c);
           if(whichThreadReading==1)
            myTaskqueue.push(Task("I{mouse}",num,num*Configfiledata.getMouse()));
           else
            reloadTaskqueue.push(Task("I{mouse}",num,num*Configfiledata.getMouse()));

           ioTask++;
    }
    else if("O{speaker" == name || "O{speaker" ==name)
    {
	         num=getNumber(c);
           if(whichThreadReading==1)
            myTaskqueue.push(Task("O{speaker}",num,num*Configfiledata.getSpeaker()));
           else
             reloadTaskqueue.push(Task("O{speaker}",num,num*Configfiledata.getSpeaker()));
           ioTask++;
    }
    else
    {
		       cout<<"*Program terminated"<<endl;
 	         cout<<"Process name is missing in Meta data file"<<endl<<endl;	
		       exit(0);
	  } 
	
}

//This function Reads the queue of processes, send them for execution and log the output
void iterateProcessQueue(int logto, string filepath, ConfigFile Configfiledata)
{        
        totalMemory=Configfiledata.getsysMemory();
        blocksize=Configfiledata.getmemoryBlock();
        totalProjectors=Configfiledata.getprojectorQty();
        totalHD=Configfiledata.gethardDriveQty();
        //cout<<"out"<<endl;

        sem_init(&booleanVariables, 0, 1);    
       // cout<<"length of queue:"<< myProcessQueue.size()<<endl;

    	  //logStream<<(double)clock() / toSeconds <<" - Simulator program starting"<<endl;     

        //determining the scheduling algorithm
        //cout<<"scheduling algorith:"<< Configfiledata.getScheduling()<<endl;

        sort(Configfiledata);
        
        while(!myProcessQueue.empty())
        {    
              //logStream<<(double)clock() / toSeconds <<"NUm Process: "<< myProcessQueue.size()<<"_________________________________________________________________________"<<endl;

              Process P = myProcessQueue.front();
              //cout<<"IO task=="<< P.getioTask() <<" Total Task: "<< P.gettotalTask()<<endl;
              int x= processExecution(&P);
              sem_wait(&booleanVariables);
             // cout<<"global time to completion:"<< globalTimeToCompletion<<endl;
              
              if(isRR==1)
              {
                if(quantumChange==true)
                {
                  //cout<<"Quantum change:"<<endl;
                  //cin>>x;
                  queue<Task> veryTempTaskQueue;
                  if(isTaskInterrupted==true)
                  {
                      if(globalTimeToCompletion<=0)
                      { 
                          //cout<<"here"<<endl;
                          myTaskqueue.pop();

                      }  
                      //if   "  "  "  "   " " " " " >0, then task is not completed but interrupted
                      else
                      {
                        //cout<<"there"<<endl;
                        Task tempT=myTaskqueue.front();
                        tempT.setToCompletion(globalTimeToCompletion);
                         veryTempTaskQueue.push(tempT);
                         myTaskqueue.pop();
                        while(!myTaskqueue.empty())
                        {
                          tempT=myTaskqueue.front();
                          veryTempTaskQueue.push(tempT);
                          myTaskqueue.pop();
                        }

                        myTaskqueue=veryTempTaskQueue;
                      }
                  }  
                
                 //myTaskqueue=veryTempTaskQueue;
                 veryTempTaskQueue=queue<Task>();
                 P.setProcessqueue(myTaskqueue);
                 
                 myProcessQueue.push(P);
                 myProcessQueue.pop();
                 quantumChange=false;
                  
                }
                else if(newProcessLoaded==true )
                {
                  if(isTaskInterrupted==true)
                  {
                    queue<Process>veryTempProcessQueue;
                    //cout<<"here"<<endl;
                    //Task and process is interrupted
                    //update time to COmpletion of the interrupted task
                    queue<Task> veryTempTaskQueue;
                    Task tempT=myTaskqueue.front();
                    //logStream<<"GLoabl Time Completion: "<<globalTimeToCompletion<<endl;
                    if(globalTimeToCompletion<=0)
                    {
                      //cout<<"inside"<<endl;

                      myTaskqueue.pop();
                      //cout<<"inside"<<endl;
                    }  
                    else //if   "  "  "  "   " " " " " >0, then task is not completed but interrupted
                    {

                      Task tempT=myTaskqueue.front();
                      tempT.setToCompletion(globalTimeToCompletion);
                      veryTempTaskQueue.push(tempT);
                      myTaskqueue.pop();
                      while(!myTaskqueue.empty())
                      {
                        tempT=myTaskqueue.front();
                        veryTempTaskQueue.push(tempT);
                        myTaskqueue.pop();
                      }
                    }


                    //cout<<"far"<<endl;
                    //myTaskqueue=veryTempTaskQueue;
                    //cout<<"far**"<<endl;
                    //veryTempTaskQueue=queue<Task>();
                    //              logStream<<"GLobal Time Completion: "<<globalTimeToCompletion<<endl;
                    //assign the changed taskqueue to the process queue
                    P.setProcessqueue(veryTempTaskQueue);
                   // cout<<"veryTemp size="<< veryTempTaskQueue.size()<<endl;
                    while(!veryTempTaskQueue.empty())
                    {
                      Task t=veryTempTaskQueue.front();
                      //cout<<" "<< t.getName();
                      veryTempTaskQueue.pop();
                    }
                    //cout<<"far"<<endl;
                    //Adjustment: since p is just a copy of the front process, we need to put this changed p process in the queue
                    //queue<Process> veryTempProcessQueue;
                    veryTempProcessQueue.push(P);
                    myProcessQueue.pop();
                    //cout<<"far"<<endl;
                    while(!myProcessQueue.empty())
                    {
                      Process px=myProcessQueue.front();
                      veryTempProcessQueue.push(px);
                      myProcessQueue.pop();
                    }
                    myProcessQueue=veryTempProcessQueue;
                    veryTempProcessQueue=queue<Process>();
                    /*   queue<Task> temp=myTaskqueue;
                      // logStream<<"TempQueue tasks: ";
                      for(int i=0;i<temp.size();i++)
                      { 
                        //logStream<<temp.front().getName()<<"  ";
                        temp.pop();
                      }
                    */ //logStream<<endl<<"INterrupt: myTaskqueue size= "<<myTaskqueue.size()<<endl;
                    P.setInterrupted(true);
                    //cout<<"*"<<endl;
                  }

                  
                  //add the reload queue at the end of my myprocessqueue and then sort them
                  while(!reloadProcessQueue.empty())
                  {
                    Process x=reloadProcessQueue.front();
                    myProcessQueue.push(x);
                    reloadProcessQueue.pop();
                  }
                  
                   newProcessLoaded=false;
                 //  logStream<<(double)clock() / toSeconds <<" - New Process added and Sorted"<<endl;  
                }
                else //(newProcessLoaded!=true && quantumChange!=true)
                {
                  myProcessQueue.pop();   
                }             
              } 
              else //for STR
              {
                if(newProcessLoaded==true )
                {
                  if(isTaskInterrupted==true)
                  {
                    queue<Process>veryTempProcessQueue;
                    //cout<<"here"<<endl;
                    //Task and process is interrupted
                    //update time to COmpletion of the interrupted task
                    queue<Task> veryTempTaskQueue;
                    Task tempT=myTaskqueue.front();
                    //logStream<<"GLoabl Time Completion: "<<globalTimeToCompletion<<endl;
                    if(globalTimeToCompletion<=0)
                    {
                      //cout<<"inside"<<endl;

                      myTaskqueue.pop();
                      //cout<<"inside"<<endl;
                    }  
                    else //if   "  "  "  "   " " " " " >0, then task is not completed but interrupted
                    {

                      Task tempT=myTaskqueue.front();
                      tempT.setToCompletion(globalTimeToCompletion);
                      veryTempTaskQueue.push(tempT);
                      myTaskqueue.pop();
                      while(!myTaskqueue.empty())
                      {
                        tempT=myTaskqueue.front();
                        veryTempTaskQueue.push(tempT);
                        myTaskqueue.pop();
                      }
                    }


                    //cout<<"far"<<endl;
                    //myTaskqueue=veryTempTaskQueue;
                    //cout<<"far**"<<endl;
                    //veryTempTaskQueue=queue<Task>();
                    //              logStream<<"GLobal Time Completion: "<<globalTimeToCompletion<<endl;
                    //assign the changed taskqueue to the process queue
                    P.setProcessqueue(veryTempTaskQueue);
                    //cout<<"veryTemp size="<< veryTempTaskQueue.size()<<endl;
                    while(!veryTempTaskQueue.empty())
                    {
                      Task t=veryTempTaskQueue.front();
                      //cout<<" "<< t.getName();
                      veryTempTaskQueue.pop();
                    }
                    //cout<<"far"<<endl;
                    //Adjustment: since p is just a copy of the front process, we need to put this changed p process in the queue
                    //queue<Process> veryTempProcessQueue;
                    veryTempProcessQueue.push(P);
                    myProcessQueue.pop();
                    //cout<<"far"<<endl;
                    while(!myProcessQueue.empty())
                    {
                      Process px=myProcessQueue.front();
                      veryTempProcessQueue.push(px);
                      myProcessQueue.pop();
                    }
                    myProcessQueue=veryTempProcessQueue;
                    veryTempProcessQueue=queue<Process>();
                    /*   queue<Task> temp=myTaskqueue;
                      // logStream<<"TempQueue tasks: ";
                      for(int i=0;i<temp.size();i++)
                      { 
                        //logStream<<temp.front().getName()<<"  ";
                        temp.pop();
                      }
                    */ //logStream<<endl<<"INterrupt: myTaskqueue size= "<<myTaskqueue.size()<<endl;
                    P.setInterrupted(true);
                    //cout<<"*"<<endl;
                  }

                  
                  //add the reload queue at the end of my myprocessqueue and then sort them
                  while(!reloadProcessQueue.empty())
                  {
                    Process x=reloadProcessQueue.front();
                    myProcessQueue.push(x);
                    reloadProcessQueue.pop();
                  }
                  //sort again according to scheduling algorithm
    
                    //  if(Configfiledata.getScheduling()<=4)
                     sort(Configfiledata);
                    /* else
                    {
                    //it is RR,just put the first process at last
                    Process tempPr=myProcessQueue.front();
                    myProcessQueue.push(tempPr);
                    myProcessQueue.pop();
                    quantumChange=false;
                   }*/
                   newProcessLoaded=false;
                 //  logStream<<(double)clock() / toSeconds <<" - New Process added and Sorted"<<endl;  
                }
                else
                {
                  myProcessQueue.pop();   
                }
              }

             
              sem_post(&booleanVariables);
               
        }

        //display reload queue
        /*
        while(!reloadProcessQueue.empty())
        {
          Process P=reloadProcessQueue.front();
          cout<<"process no: "<<P.getProcessNumber()<< " size = "<<P.gettotalTask()<<endl;
          reloadTaskqueue=P.getTaskQueue();
          while(!reloadTaskqueue.empty())
          {
            Task T=reloadTaskqueue.front();
            cout<<"T.getName: "<<T.getName()<<endl;
            reloadTaskqueue.pop();
          }
          reloadProcessQueue.pop();
        }
  */
        logStream<<(double)clock() / toSeconds <<" - Simulator program starting ending "<<endl;
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
int processExecution(Process *P)
{
    //cout<<"Process Execution started:"<<endl;
    P->setStatus(1);
    int Pnum=P->getProcessNumber();
    myTaskqueue=(P->getTaskQueue());
    //cout<<"my task queue.size "<<myTaskqueue.size()<<endl;
    string S;
    ostringstream strs,strs2; 
    double initialClock=clock();
    sem_t s;
    sem_init(&s, 0, 1);
     pthread_mutex_t myMutex     = PTHREAD_MUTEX_INITIALIZER;
  //cout<<"*** PTHREAD_MUTEX_initializaer==="<<(int)count_mutex<<endl;
    pthread_mutex_lock( &myMutex ); 

   // cout<<"initial clock="<<initialClock<<endl;

   // logStream<<"NUm Process: "<< P->getTaskQueue().size()<<"_________________________________________________________________________"<<endl;
    while(!myTaskqueue.empty())
    {
        Task T = myTaskqueue.front();
        //cout<<(double)(clock())/toSeconds<<" Process:"<< Pnum <<" T.getName=="<<T.getName()<<endl;
        if("A{start}"== T.getName())
        {
            P->setStatus(2);
   		      logStream<<(double)(clock()) /toSeconds<<" - OS: preparing process "<<std::dec<<Pnum<<"\n";
        }
        else if("A{end}"== T.getName())
        {
            
            logStream<<(double)(clock()) /toSeconds<<" - OS: removing process "<<std::dec<<Pnum<<"\n";


            P->setStatus(4);			 
        }
        else if("P{run}"== T.getName())
        {
            clock_t hereClock=clock();
            //logStream<<(double)hereClock<< "Time"<<endl;
            if(P->getInterrupted()==true)
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": Resuming Processsing action"<<"\n";
            else 
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": start Processsing action"<<"\n";
            P->setStatus(3);
            //globalFullCycle=(T.getEndTime()-T.getStartTime());
            globalFullCycle=(T.getTimeToCompletion());            //cout<<"Processor .getEndtime="<<globalFullCycle<<endl;
            //logStream<<"Global FullCyle: "<< globalFullCycle<<endl;
            int x=checkFinish();

            clock_t thereClock=clock();
            //logStream<<(double)thereClock<< "Time"<<endl;
           // logStream<<"Global FullCyle: "<< globalFullCycle<< " Time to completion = "<< T.getEndTime()-(double)(thereClock-hereClock)/toSeconds<<endl;
            //logStream<<"INtern time: "<<(float)(thereClock-hereClock)/toSeconds*1000<<endl;
            int interTime=(float)(thereClock-hereClock)/toSeconds*1000;
            interTime=(int)interTime;
            //logStream<<"Intertime="<<interTime<<endl;
            if(quantumChange==true)
            {
              //cout<<"Quantum Change true"<<endl;
              globalTimeToCompletion=(T.getTimeToCompletion()-interTime);
            

              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<" Interrupted Quantum"<<"\n";
              return 1;
            }
            if(newProcessLoaded==true )
            {
              globalTimeToCompletion=(T.getTimeToCompletion()-interTime);
              
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<" Interrupted"<<"\n";

              return 1;
            }
            P->setStatus(2);
            logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": end Processsing action"<<"\n";
        }
        else if("O{monitor}"== T.getName())
        {
          clock_t hereClock=clock();
            sem_wait(&s);
            if(P->getInterrupted()==true)
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": Resuming Monitor output"<<"\n";
            else
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": Start Monitor output"<<"\n";

            P->setStatus(3);
            pthread_attr_init(&attr);
              //          cout<<"monitor t.getEndtime="<<T.getEndTime()<<endl;

            //pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
            pthread_create(&thread, &attr, ioThreadProcessing, (void *)(intptr_t)(T.getTimeToCompletion()) );
            pthread_join(thread, NULL);
            clock_t thereClock=clock();
            int interTime=(float)(thereClock-hereClock)/toSeconds*1000;
            interTime=(int)interTime;
            if(quantumChange==true)
            {

              globalTimeToCompletion=(T.getTimeToCompletion()-interTime);
              
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": Interrupted Quantum"<<"\n";
              return 1;
            }
            P->setStatus(2);
            sem_post(&s);
            logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": end Monitor output"<<"\n";
        }
        else if("I{hard drive}"== T.getName())
        {
            clock_t hereClock=clock();
            sem_wait(&s);
            if(P->getInterrupted()==true)
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": resuming hard drive Input on HDD "<<"\n";
            else
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": start hard drive Input on HDD "<<"\n";

            P->setStatus(3);
            pthread_attr_init(&attr);
             //                       cout<<"hard : getEndtime="<<T.getEndTime()<<endl;

            //pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
            pthread_create(&thread, &attr, ioThreadProcessing, (void *)(intptr_t)(T.getTimeToCompletion()) );

            pthread_join(thread, NULL);
            clock_t thereClock=clock();
            int interTime=(float)(thereClock-hereClock)/toSeconds*1000;
            interTime=(int)interTime;
            if(quantumChange==true)
            {
              globalTimeToCompletion=(T.getTimeToCompletion()-interTime);
              
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": Interrupted Quantum"<<"\n";
              return 1;
            }
            P->setStatus(2);
            sem_post(&s);
            logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": end hard drive Input"<<"\n";
        }
         else if("O{projector}"== T.getName())
        {
            //       cout<<"clock(): "<< (double)clock()<<endl;
          clock_t hereClock=clock();
            sem_wait(&s);
            if(P->getInterrupted()==true)
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": resuming projector output " <<"\n";
            else
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": start projector output"<<"\n";

            P->setStatus(3);
            pthread_attr_init(&attr);
            //cout<<"proj T.getEndTime=="<<T.getEndTime()<<endl;
            //pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
            pthread_create(&thread, &attr, ioThreadProcessing, (void *)(intptr_t)(T.getTimeToCompletion()) );

            pthread_join(thread, NULL);
            clock_t thereClock=clock();
            int interTime=(float)(thereClock-hereClock)/toSeconds*1000;
            interTime=(int)interTime;
            if(quantumChange==true)
            {
              globalTimeToCompletion=(T.getTimeToCompletion()-interTime);
              
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": Interrupted Quantum"<<"\n";
              return 1;
            }
            P->setStatus(2);
            //        cout<<"clock(): "<< (double)clock()<<endl;
            sem_post(&s);
            logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": end projector output "<<"\n";
        }
        else if("O{hard drive}"== T.getName())
        {
            //        cout<<"clock(): "<< (double)clock()<<endl;
            clock_t hereClock=clock();
            sem_wait(&s);
            if(P->getInterrupted()==true)
            logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": resuming hard drive output\n";			
            else
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": start hard drive output on HD\n";      
        
            P->setStatus(3);
            pthread_attr_init(&attr);
            //pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
            //                      cout<<" o hard getEndtime="<<T.getEndTime()<<endl;

            pthread_create(&thread, &attr, ioThreadProcessing, (void *)(intptr_t)(T.getTimeToCompletion()) );

            pthread_join(thread, NULL);
            clock_t thereClock=clock();
            int interTime=(float)(thereClock-hereClock)/toSeconds*1000;
            interTime=(int)interTime;
            if(quantumChange==true)
            {
              globalTimeToCompletion=(T.getTimeToCompletion()-interTime);
              
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": Interrupted Quantum"<<"\n";
              return 1;
            }
            P->setStatus(2);  
            //         cout<<"clock(): "<< (double)clock()<<endl;
            sem_post(&s);
            logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": end hard drive output"<<"\n";
        }
        else if("O{printer}"== T.getName())
        {
          //cout<<"clock(): "<< (double)clock()<<endl;
          clock_t hereClock=clock();
            sem_wait(&s);
            if(P->getInterrupted()==true)
            logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": resuming printer output "<<"\n";
            else
            logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": start printer output "<<"\n";

            P->setStatus(3);
            pthread_attr_init(&attr);
          //  pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
            //                         cout<<"o printer getEndtime="<<T.getEndTime()<<endl;

                        pthread_create(&thread, &attr, ioThreadProcessing, (void *)(intptr_t)(T.getTimeToCompletion()) );

            pthread_join(thread, NULL);
            clock_t thereClock=clock();
            int interTime=(float)(thereClock-hereClock)/toSeconds*1000;
            interTime=(int)interTime;
            if(quantumChange==true)
            {
              globalTimeToCompletion=(T.getTimeToCompletion()-interTime);
              
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": Interrupted Quantum"<<"\n";
              return 1;
            }
            P->setStatus(2);
              //        cout<<"clock(): "<< (double)clock()<<endl;
            sem_post(&s);
            logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": end printer output"<<"\n";               
        }
        else if("I{keyboard}"== T.getName())
        {
            //      cout<<"clock(): "<< (double)clock()<<endl;
          clock_t hereClock=clock();
            sem_wait(&s);
            if(P->getInterrupted()==true)
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": resuming keyboard Input"<<"\n";
            else
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": start keyboard Input"<<"\n";
  
            P->setStatus(3);
            pthread_attr_init(&attr);
            //                    cout<<"I keyboard getEndtime="<<T.getEndTime()<<endl;

            //pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
            pthread_create(&thread, &attr, ioThreadProcessing, (void *)(intptr_t)(T.getTimeToCompletion()) );

            pthread_join(thread, NULL);
            clock_t thereClock=clock();
            int interTime=(float)(thereClock-hereClock)/toSeconds*1000;
            interTime=(int)interTime;
            if(quantumChange==true)
            {
              globalTimeToCompletion=(T.getTimeToCompletion()-interTime);
              
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": Interrupted Quantum"<<"\n";
              return 1;
            }
            P->setStatus(2);
            //    cout<<"clock(): "<< (double)clock()<<endl;
            sem_post(&s);
            logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": end keyboard Input"<<"\n";
        }
        else if("M{allocate}"== T.getName())
        {
            //        cout<<"clock(): "<< (double)clock()<<endl;
            clock_t hereClock=clock();
            if(P->getInterrupted()==true)
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": Resume allocating memory"<<"\n";
            else
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": allocating memory"<<"\n";

            P->setStatus(3);
            //globalFullCycle=(T.getEndTime()-T.getStartTime());
            //                      cout<<"m allocate getEndtime="<<T.getEndTime()<<endl;

            globalFullCycle=(T.getTimeToCompletion());            //cout<<"Processor .getEndtime="<<globalFullCycle<<endl;
            //logStream<<"Global FullCyle: "<< globalFullCycle<<endl;
            int x=checkFinish();
            clock_t thereClock=clock();
            int interTime=(float)(thereClock-hereClock)/toSeconds*1000;
            interTime=(int)interTime;
            if(quantumChange==true)
            {
              globalTimeToCompletion=(T.getTimeToCompletion()-interTime);
              //cout<<"global TIme to COmpletion: inside: "<<globalTimeToCompletion<<endl;
              
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": Interrupted Quantum"<<"\n";
              return 1;
            }
            if(newProcessLoaded==true)
            {
              globalTimeToCompletion=(T.getTimeToCompletion()-interTime);
             

              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<" Interrupted"<<"\n";
              return 1;
            }
            P->setStatus(2);
            //      cout<<"clock(): "<< (double)clock()<<endl;

            logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": memory allocated at 0X"<<std::hex <<allocateMemory()<<"\n";
        }
        else if("M{block}"== T.getName())
        {
             //       cout<<"clock(): "<< (double)clock()<<endl;
            clock_t hereClock=clock();
            if(P->getInterrupted()==true)
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": resuming memory blocking"<<"\n";
            else
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": start memory blocking"<<"\n";

            P->setStatus(3);
            //globalFullCycle=(T.getEndTime())-T.getStartTime();
               //                     cout<<"m block getEndtime="<<T.getEndTime()<<endl;
            int x=checkFinish();
            clock_t thereClock=clock();
            int interTime=(float)(thereClock-hereClock)/toSeconds*1000;
            interTime=(int)interTime;
            if(quantumChange==true)
            {
              globalTimeToCompletion=(T.getTimeToCompletion()-interTime);
              
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": Interrupted Quantum"<<"\n";
              return 1;
            }
            if(newProcessLoaded==true)
            {
              globalTimeToCompletion=(T.getTimeToCompletion()-interTime);
              

              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<" Interrupted"<<"\n";
              return 1;
            }

            P->setStatus(2);
                 //     cout<<"clock(): "<< (double)clock()<<endl;

            logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": end memory blocking"<<"\n";
        }
        else if("I{mouse}"== T.getName())
        {
          clock_t hereClock=clock();
              //      cout<<"clock(): "<< (double)clock()<<endl;
            sem_wait(&s);
            if(P->getInterrupted()==true)
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": start mouse Input"<<"\n";
            else
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": start mouse Input"<<"\n";
             
            P->setStatus(3);
            pthread_attr_init(&attr);
                //                    cout<<"I mouse tEndtime="<<T.getEndTime()<<endl;

           // pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
            pthread_create(&thread, &attr, ioThreadProcessing, (void *)(intptr_t)(T.getTimeToCompletion()) );

            pthread_join(thread, NULL);
            clock_t thereClock=clock();
            int interTime=(float)(thereClock-hereClock)/toSeconds*1000;
            interTime=(int)interTime;
            if(quantumChange==true)
            {
              globalTimeToCompletion=(T.getTimeToCompletion()-interTime);
              
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": Interrupted Quantum"<<"\n";
              return 1;
            }
            P->setStatus(2);
                  //    cout<<"clock(): "<< (double)clock()<<endl;
            sem_post(&s);
            logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": end mouse Input"<<"\n";
        }
        else if("O{speaker}"== T.getName())
        {
          clock_t hereClock=clock();
             //       cout<<"clock(): "<< (double)clock()<<endl;
            sem_wait(&s);
            if(P->getInterrupted()==true)
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": Resuming speaker Input"<<"\n";
            else
              logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": start speaker Input"<<"\n";

            P->setStatus(3);
            pthread_attr_init(&attr);
              //                      cout<<"o speaker getEndtime="<<T.getEndTime()<<endl;

          //pthread_create(&thread, &attr, ThreadProcessing, (void *)(intptr_t)(T.getEndTime()-T.getStartTime()) );
            pthread_create(&thread, &attr, ioThreadProcessing, (void *)(intptr_t)(T.getTimeToCompletion()) );

            pthread_join(thread, NULL);
            clock_t thereClock=clock();
            int interTime=(float)(thereClock-hereClock)/toSeconds*1000;
            interTime=(int)interTime;
            if(quantumChange==true)
            {
              globalTimeToCompletion=(T.getTimeToCompletion()-interTime);
              
              logStream<<(double)(clock())/toSeconds<<" - Process "<<std::dec<<Pnum <<": Interrupted Quantum"<<"\n";
              return 1;
            }
            P->setStatus(2);
               //       cout<<"clock(): "<< (double)clock()<<endl;
            sem_post(&s);
            logStream<<(double)(clock()) /toSeconds<<" - Process "<<std::dec<<Pnum <<": end speaker Input"<<"\n";
        }
        else
        {
            cout<<"***Program terminated"<<endl;
            cout<<"Process name is missing in Meta data file"<<endl<<endl;	
            exit(0);
	      } 
   
    
    myTaskqueue.pop();		
   // cout<<"task popped";
    isTaskInterrupted=false;
    globalTimeToCompletion=0;
    /*if(newProcessLoaded==true )
     { 
        cout<<"false"<<endl;
        P->setInterrupted(true);
       return 0;
    }*/
   // cout<<"here"<<endl;

    }
    
    pthread_mutex_unlock( &myMutex );
}


//Thread runner function
void *ioThreadProcessing(void *t)
{
     int cycles=*((int*)(&t));
     if(isRR==0)
     {    
      myWait(cycles);
	   }else
     {
      for(int i=cycles;i>0;i--)
      {
        myWait(1);
        if(quantumChange==true)
        {
          //logStream<<"Quantum Time"<<endl;
          isTaskInterrupted=true;
          pthread_exit(NULL);
          return 0;
        }
      }
    }
      pthread_exit(NULL);
}

//Thread for timer
void *timerThreadProcessing(void *t)
{
  while(1)
  {
    int cycles=globalFullCycle;
    for(cycles;cycles>0;cycles--)
    {
      myWait(1);
      if(quantumChange==true)
      {
          isTaskInterrupted=true;
          pthread_exit(NULL);
          return 0;
      }
    }
    if(cycles==0) 
    {
      tBool=true;
      cycles=-1;
    }
  }
}

void *timerRoundRobinThread(void *t)
{
  myWait(50);
  while(1)
  {
    sem_wait(&booleanVariables);
    quantumChange=true;
    sem_post(&booleanVariables);
   // logStream<<(double)(clock())/toSeconds<<" quantum change true"<<endl;
     myWait(50);
  }
}


//my Sleep function to stop the execution while processing a task
void myWait(int cycles)
{
  clock_t endTime;
  endTime=clock() + cycles * toSeconds/1000;
  while(clock()<endTime);
}

int checkFinish()
{
  while(1)
  {
    //myWait(60);
    for(int i=0;i<1;i++)
    {
      myWait(1);
      
      if(quantumChange==true)
      {
        //logStream<<"Quantum Time"<<endl;
        isTaskInterrupted=true;
        return 0;
      }
      if(newProcessLoaded==true) 
      {
        //isTaskInterrupted=true;
        return 0;
      }
    }
    if(tBool==true)
    {
      tBool=false;
      break;
    }
  }
}


void sortSTR()
{
    queue<Process>tempProcessQueue=myProcessQueue;
    int numProcess=tempProcessQueue.size();
    std::vector<Process> processVector;
    
    //copying the queue to a vector
    for(int i=0;i<numProcess;i++)
    {
      processVector.push_back(tempProcessQueue.front());
      tempProcessQueue.pop();
    }

    //sorting the vector
    for(int i=0;i<numProcess-1;i++)
    {
      //cout<<"processVector[i= "<<i<<"].gettotalTask="<<processVector[i].gettotalTask()<<endl;
      for (int j=i+1;j<numProcess;j++)
      {
        //cout<<"processVector[j="<<j<<"].gettotalTask="<<processVector[j].gettotalTask()<<endl;
        if(processVector[i].getTaskQueue().size()>processVector[j].getTaskQueue().size())
        {
          std::swap(processVector[i],processVector[j]);
        }
      }
    }
    
    //writing back to main queue
    tempProcessQueue=queue<Process>();
    for(int i=0;i<numProcess;i++)
    {
      //tempProcessQueue.push(Process(processVector[i].getProcessNumber(),0.0,0.0,processVector[i].getTaskQueue(),0,processVector[i].getioTask()));
      tempProcessQueue.push(processVector[i]);
    }
    // cout<<"tempProcessQueue.size=="<< tempProcessQueue.size()<<endl;
    myProcessQueue=tempProcessQueue;
}

void sortPS()
{
   queue<Process>tempProcessQueue=myProcessQueue;
          int numProcess=tempProcessQueue.size();
          std::vector<Process> processVector;

          for(int i=0;i<numProcess;i++)
          {
            processVector.push_back(tempProcessQueue.front());
            tempProcessQueue.pop();
          }
          /*
          for(int i=0;i<numProcess;i++)
          {
            cout<<"processVector.getioTask="<<processVector[i].getioTask()<<endl;
          }*/
          //sort vector of processes
          for(int i=0;i<numProcess-1;i++)
          {
            for (int j=i+1;j<numProcess;j++)
            {
              if(processVector[i].getioTask()<processVector[j].getioTask()){
                std::swap(processVector[i],processVector[j]);
              }
            }
          }
          /*
          cout<<endl;
          for(int i=0;i<numProcess;i++)
          {
            cout<<"processVector.getioTask="<<processVector[i].getioTask()<<endl;
          }
          */
          tempProcessQueue=queue<Process>();

          for(int i=0;i<numProcess;i++)
          {
            //tempProcessQueue.push(Process(processVector[i].getProcessNumber(),0.0,0.0,processVector[i].getTaskQueue(),0,processVector[i].getioTask()));
              tempProcessQueue.push(processVector[i]);
          }

          //cout<<"tempProcessQueue.size=="<< tempProcessQueue.size()<<endl;
          myProcessQueue=tempProcessQueue;
}
void sortSJF()
{
    queue<Process>tempProcessQueue=myProcessQueue;
          int numProcess=tempProcessQueue.size();
          std::vector<Process> processVector;

          for(int i=0;i<numProcess;i++)
          {
            processVector.push_back(tempProcessQueue.front());
            tempProcessQueue.pop();
          }
          /*
          for(int i=0;i<numProcess;i++)
          {
            cout<<"processVector.getotalTask="<<processVector[i].gettotalTask()<<endl;
          }
          */
          //sort vector of processes
          
          for(int i=0;i<numProcess-1;i++)
          {
            //cout<<"processVector[i= "<<i<<"].gettotalTask="<<processVector[i].gettotalTask()<<endl;
            for (int j=i+1;j<numProcess;j++)
            {
              //cout<<"processVector[j="<<j<<"].gettotalTask="<<processVector[j].gettotalTask()<<endl;

              if(processVector[i].gettotalTask()>processVector[j].gettotalTask()){
                std::swap(processVector[i],processVector[j]);
              }
            }
          }
          
          cout<<endl;
          /*
          for(int i=0;i<numProcess;i++)
          {
            cout<<"processVector.gettotalTask="<<processVector[i].gettotalTask()<<endl;
          }
          */
          tempProcessQueue=queue<Process>();

          for(int i=0;i<numProcess;i++)
          {
            //tempProcessQueue.push(Process(processVector[i].getProcessNumber(),0.0,0.0,processVector[i].getTaskQueue(),0,processVector[i].getioTask()));
              tempProcessQueue.push(processVector[i]);
          }
         // cout<<"tempProcessQueue.size=="<< tempProcessQueue.size()<<endl;
          myProcessQueue=tempProcessQueue;
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
void myWaitTimer(int cycles)
{ 
    clock_t endTime;
    endTime=clock() + cycles * toSeconds/1000;
    while(clock()<endTime);
    //long k=cycles*1000;
    //for (long i=k;i>0;i--);
     
    //  cout<<"cycles: "<< cycles<<endl;

    //usleep(cycles*1000);
}

void sort(ConfigFile Configfiledata)
        {
        if(Configfiledata.getScheduling()==1)
        {
          //FIFO
          //dont have to do anything, default algorithm is FIFO 
        }
        else if(Configfiledata.getScheduling()==2)
        {
          sortPS();
        }
        else if(Configfiledata.getScheduling()==3)
        {
          sortSJF();
        }
        else if(Configfiledata.getScheduling()==4)
        {
          sortSTR();
        }
        else if(Configfiledata.getScheduling()==5)
        {
          isRR=1;
          
          {
            pthread_attr_init(&quantumThreadAttr);
            pthread_create(&quantumThread,&quantumThreadAttr,timerRoundRobinThread,NULL);
          }
          
        }
        else
        {
          //wrong scheduling algorithm
        }

      }