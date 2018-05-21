#include "Metadata.h"
#include <iomanip>

int count = 0;
//Store the tasks read from mdf file in the queue
queue<Task> taskQueue;
queue<Task> taskQueueReload;
queue<Task> taskQueueTemp;
//Store the processes read from mdf file in the list
list<Process> processList;
list<Process> processListReload;
//to store the loging information
ostringstream logInfoStream;
string logInfoString;
//variable to store the process number
int processnumber=0;
//variable to store total number of IO operations within a process
int totalNumofIOOps;
//variable to store total number of tasks within a process 
int totalNumofTasks; 
//varible to store total process execution time
float processTotalExecutionTime;
//Globle varible that stores the coundown time for each task and resetted when a new task appeares
float countdown_Time_Task = 0;
//config object pointer
Config * configData;
//Globle varible that checks when the timer start and when the timer should end its operation
bool timer_Run = false; // for process Timer
bool TimerRun = false; //for Quantum Timer
bool reloadMDF = false;
bool isIO = false;
bool isnewProcessReloaded = false;
bool * isnewProcessInQueue;
bool isQuantumTimeFinished = false;
//Pthreads
pthread_t thread;
pthread_t thread_Timer;
pthread_t thread_reloadMDF;
pthread_t thread_RRQuantumTime;
pthread_attr_t attr;
//Varibles that stores states of memory, printers and hard drives
long int totalSystemMem=0,curreltlyUsedMem=0;
long int memoryBlockSize=0;
int currentlyUsedProjectors=0, totalNumProjectors=0;
int currentlyUsedHD=0,totalNumHd=0;
// varible
const int size = 256;
// offset time
double offSetTime = 0.0;
// semaphore varible for projector
sem_t s_Projector;
// semaphore varible for hard drive
sem_t s_HardDrive;
//mutex object initilization to lock triggering bool
pthread_mutex_t lock_bool = PTHREAD_MUTEX_INITIALIZER;
//mutex object initilization to lock processlist
pthread_mutex_t lock_processList = PTHREAD_MUTEX_INITIALIZER;

//create reload thread that reload mdf file in every 100ms for 10 times
void CreateMDFReloadThread( Config * configPointer)
{
	configData = configPointer;
	pthread_create(&thread_reloadMDF, &attr, ReloadMdfFile, NULL);
}
//reload the mdf file in every 100ms for 10 times
void * ReloadMdfFile(void * value)
{
	for(int i = 0; i<10; i++)
	{
		WaitReloadThread(100); //start reloading mdf file 100ms after the main thread loaded mdf file
		reloadMDF = true;
		ReadDatafromMDF(configData); //reload mdf file
		setResetTriggingBool(0); //trigger the main thread that there is new process
	}
	pthread_exit(NULL); 
}
//wait reloadthread 100ms
int WaitReloadThread(int milliseconds)
{
	clock_t x_endTime;
	x_endTime = clock() + milliseconds * CLOCKS_PER_SEC/1000;
	while (clock() < x_endTime);
	return 0;		
}
// Reading the data from MDF file
void ReadDatafromMDF(Config * Configfiledata)
{
	fstream file;	string line,filename;
	vector<string> v;
	filename=Configfiledata->getmdfFilepath();
	parse(filename, '.', v);
	if (v[1].compare("mdf") != 0)
	{
		cout<<"Meta data File Extension is not correct"<<endl;
		exit(0);
	}

	file.open(filename.c_str());
	if ( file.peek() == std::ifstream::traits_type::eof() )
	{
		cout<<"Programm ended"<<endl;
		cout<<"Meta data file is empty or Missing!!"<<endl;
		exit(0);
	}

	if (file.is_open())
	{
		while ( getline (file,line) )
		{

			if (line!="Start Program Meta-Data Code:"&& line!="End Program Meta-Data Code.")
			{   
				v=parseMetaData(line, ".!,;:");
				parseProcessNcycle(v,Configfiledata);
			}
		}
	}
	else 
	{
		cout<<"Metadata file cannot be opened"<<endl;
		exit(0);
	}
	file.close();
}

//parsing the process name and cycles from metadata file
void parseProcessNcycle(vector<string>& v,Config * Configfiledata)
{
	string name, cycle;
	vector<string> v1;
	int counter=0;
	for ( unsigned int x = 0; x != v.size(); ++x)
	{
		parse(v[x], '}', v1);
		name=v1[counter];
		counter++;
		cycle=v1[counter];
		counter++;
		if (cycle=="")
		{
			cout<<"Program ended"<<endl;
			cout<<"Process cycles are missing in Meta data file"<<endl<<endl;	
			exit(0);			
		}	
		assignProcessNTask(name, cycle,Configfiledata);
	}
}

//Assigning the processes in Process queue and Tasks in Task queue
void assignProcessNTask(const  string& name,const  string& cycle,Config * Configfiledata)
{
	int num;
	string c=cycle;
	if("S{begin" ==name ||"s{begin"==name)
	{

	}
	else if ("S{finish" ==name ||"s{finish" ==name)
	{

	}
	else if("A{begin"== name || "a{begin" ==name)
	{
		processnumber=processnumber+1;
		totalNumofIOOps = 0;
		totalNumofTasks = 0;
		processTotalExecutionTime = 0.0;
		if (reloadMDF)
			taskQueueReload.push(Task("A{begin}",0,0));  
		else
			taskQueue.push(Task("A{begin}",0,0));  
	}
	else if("A{finish"== name || "a{finish" ==name)
	{
		if( reloadMDF) //reload thread
		{  
			taskQueueReload.push(Task("A{finish}",0,0));  
			Process p = Process(processnumber,processTotalExecutionTime,taskQueueReload,0,totalNumofIOOps,totalNumofTasks,0);
			updateProcessList(p, 1);
			taskQueueReload=queue<Task>(); //clear task queue
		}
		else   //main thread
		{ 
			taskQueue.push(Task("A{finish}",0,0));
			processList.push_back( Process(processnumber,processTotalExecutionTime,taskQueue,0,totalNumofIOOps,totalNumofTasks,0));
			taskQueue=queue<Task>(); //clear task queue
		}
		//cout<<"Process Number "<<processnumber<<" IO "<<totalNumofIOOps<<" tasks "<<totalNumofTasks<<endl;
			
	}
	else if("P{run"== name ||"P{run" ==name)
	{
		totalNumofTasks = totalNumofTasks + 1;
		num=convertStringToNum(c);
		processTotalExecutionTime += num*Configfiledata->getProcessor ();
		if (reloadMDF)
			taskQueueReload.push(Task("P{run}",num*Configfiledata->getProcessor (),0));
		else
			taskQueue.push(Task("P{run}",num*Configfiledata->getProcessor (),0));
	}
	else if("M{allocate" == name || "m{allocate" ==name)
	{
		totalNumofTasks = totalNumofTasks + 1;
		num=convertStringToNum(c);
		processTotalExecutionTime += num*Configfiledata->getMemory ();
		if (reloadMDF)
			taskQueueReload.push(Task("M{allocate}",num*Configfiledata->getMemory () ,0));
		else
			taskQueue.push(Task("M{allocate}",num*Configfiledata->getMemory () ,0));
	}
	else if("O{monitor"== name || "o{monitor" ==name)
	{
		totalNumofTasks = totalNumofTasks + 1;
		totalNumofIOOps = totalNumofIOOps +1;
		num=convertStringToNum(c);
		processTotalExecutionTime += num*Configfiledata->getMonitor ();
		if (reloadMDF)
			taskQueueReload.push(Task("O{monitor}",num*Configfiledata-> getMonitor () ,0));
		else
			taskQueue.push(Task("O{monitor}",num*Configfiledata-> getMonitor () ,0));
	}
	else if("I{hard drive" == name || "i{hard drive" == name)
	{
		totalNumofTasks = totalNumofTasks + 1;
		totalNumofIOOps = totalNumofIOOps +1;
		num=convertStringToNum(c);
		processTotalExecutionTime += num*Configfiledata->getHardDrive();
		if (reloadMDF)
			taskQueueReload.push(Task("I{hard drive}",num*Configfiledata->getHardDrive(),0));
		else
			taskQueue.push(Task("I{hard drive}",num*Configfiledata->getHardDrive(),0));
	}
	else if("O{projector"== name || "o{projector" ==name)
	{
		totalNumofTasks = totalNumofTasks + 1;
		totalNumofIOOps = totalNumofIOOps +1;
		num=convertStringToNum(c);
		processTotalExecutionTime += num*Configfiledata->getProjector ();
		if (reloadMDF)
			taskQueueReload.push(Task("O{projector}",num*Configfiledata->getProjector (),0));
		else
			taskQueue.push(Task("O{projector}",num*Configfiledata->getProjector (),0));
	}
	else if("M{block" == name || "m{block" ==name)
	{
		totalNumofTasks = totalNumofTasks + 1;
		num=convertStringToNum(c);
		processTotalExecutionTime += num*Configfiledata-> getMemory ();
		if (reloadMDF)
			taskQueueReload.push(Task("M{block}",num*Configfiledata-> getMemory () ,0));
		else
			taskQueue.push(Task("M{block}",num*Configfiledata-> getMemory () ,0));
	}
	else if("I{keyboard" == name || "i{keyboard}" ==name)
	{
		totalNumofTasks = totalNumofTasks + 1;
		totalNumofIOOps = totalNumofIOOps +1;
		num=convertStringToNum(c);
		processTotalExecutionTime += num*Configfiledata->getKeyboard ();
		if (reloadMDF)
			taskQueueReload.push(Task("I{keyboard}",num*Configfiledata-> getKeyboard (),0));
		else
			taskQueue.push(Task("I{keyboard}",num*Configfiledata-> getKeyboard (),0));
	}
	else if("O{hard drive" == name || "o{hard drive" == name)
	{
		totalNumofTasks = totalNumofTasks + 1;
		totalNumofIOOps = totalNumofIOOps +1;
		num=convertStringToNum(c);
		processTotalExecutionTime += num*Configfiledata->getHardDrive();
		if (reloadMDF)
			taskQueueReload.push(Task("O{hard drive}",num*Configfiledata->getHardDrive(),0));
		else
			taskQueue.push(Task("O{hard drive}",num*Configfiledata->getHardDrive(),0));
	}
	else
	{
		cout<<"Program terminated"<<endl;
		cout<<"Misspelling in the process name or Process name is missing in Meta data file"<<endl<<endl;	
		exit(0);
	} 

}
//logging data from the metadata file to file or monitor based on the input from config file
void logProcessTofileoRmonitor(Config * objectConfig)
{  
	logInfoStream<<setprecision(6)<<fixed;
	int logto=objectConfig->getLogged();
	string filepath=objectConfig->getlogFilePath();
	totalSystemMem=objectConfig->getSystemMem();
	memoryBlockSize=objectConfig->getBlockSize ();
	totalNumProjectors=objectConfig->getProjectorQuantity();
	totalNumHd=objectConfig->getHDQuantity();
	string schedulingAlgo = objectConfig->getSchedulingAlgo();
        list <Process>::iterator it;
	int r = 0;
	Process p = Process(0,0.0,taskQueue,0,0,0,0);
	bool isfinish = false;
	offSetTime = (float)clock()/CLOCKS_PER_SEC;
	logInfoStream<<ReturnTime()<<" - Simulator program starting"<< " Scheduling "<<schedulingAlgo<<endl; 
	if( schedulingAlgo == "RR")
	{
		isnewProcessInQueue = & isQuantumTimeFinished; //checks quantum time
		TimerRun = true;
		pthread_create(&thread_RRQuantumTime, &attr, quantumTimeTimer, NULL); //dedicated thread that record the quantum time for RR (50ms to interrupt)
	}
	else
	{
		isnewProcessInQueue = &isnewProcessReloaded; //checks new tasks
	}

	while(!isfinish)
	{
		// executing processes based on RR scheduling algorithm
		if( schedulingAlgo == "RR")
		{
			while (!processList.empty())
			{
				r = ExecuteProcess(processList.front(),objectConfig);
				if(r)
					isQuantumTimeFinished = false;
				processList.pop_front();
				if(isnewProcessReloaded)
				{	
					if(r)
					{
					 	updateProcessList(p, 3); //if there is interrupt, put the interrupted task after the new task 
						processListReload.clear();
					}
					else
					{
						updateProcessList(p, 2); // if there is no interrupt
						processListReload.clear();
					}
					setResetTriggingBool(1); //reset triggering bool after updating the process list
					break;
				}	
			}
		}
		else //executing processes based on FIFO or SJF or PS or STR
		{
			sortProcessList(schedulingAlgo);
			while (!processList.empty())
			{
				r = ExecuteProcess(processList.front(),objectConfig);
				processList.pop_front();
				if(isnewProcessReloaded)
				{
					updateProcessList(p, 2);
					processListReload.clear();
					setResetTriggingBool(1); //reset triggering bool
					break;
				}	
			}
		}
		while (processList.empty())
		{
			isfinish = true;
			break;
		}
	}
	logInfoStream<<ReturnTime()<<" - Simulator program ending "<<endl;
	timer_Run = false;
	TimerRun = false;
	logInfoString=logInfoStream.str();
	ofstream myfile;
	if(logto == 1 || logto == 2)
	{
		myfile.open (filepath.c_str(), ios::app);
		myfile<<logInfoString<<endl;
		myfile.close();
	}
	if (logto == 3 || logto == 2)
	{
		cout<<logInfoString<<endl;
	}
}
//execute current process in the process Queue
int ExecuteProcess(Process P, Config * configfiledata)
{
	P.setProcessStatus(1);
	int Pnum=P.getProcessNumber();
	taskQueue=P.getTaskOnQueue();
	taskQueueTemp=queue<Task>();
	string S;
	int r = 0;
	float current_Time, diff_Time;
	float remaningTaskExecutionTime;
	ostringstream strs,strs2;
	//mutex object initilization for process_lock
	pthread_mutex_t lock_Process = PTHREAD_MUTEX_INITIALIZER;
	//mutex object initilization for thread_lock
	pthread_mutex_t lock_Thread = PTHREAD_MUTEX_INITIALIZER;
	// Semaphore initilization for Projector
	sem_init(&s_Projector, 0, totalNumProjectors);
	// Semaphore initilization for hard drive
	sem_init(&s_HardDrive, 0, totalNumHd);
	//lock process
        pthread_mutex_lock( &lock_Process ); 
	if( P.getProcessInterruptStatus())
	{
		logInfoStream<<ReturnTime()<<" - OS: resuming process "<<Pnum<<"\n";
	}
	while(!taskQueue.empty())
	{
		Task task = taskQueue.front();
		if( *isnewProcessInQueue) //if new Process before starting another task, return saving the process status
		{
			P.setProcessInterrupStatus(1);
			P.setProcessOnQueue(taskQueue);
			processList.push_back(P);  //push the interruped process in the process list with new updated status
 			logInfoStream<<ReturnTime()<<" - OS: interrupt process "<<Pnum<<"\n";
			return 1;
		}
		logInfoStream<<setprecision(6)<<fixed;
		if("A{begin}"== task.getTaskName())
		{
			timer_Run = true;
			P.setProcessStatus(2);
			logInfoStream<<ReturnTime()<<" - OS: preparing process "<<Pnum<<"\n";
			logInfoStream<<ReturnTime()<<" - OS: starting process "<<Pnum<<"\n";
			pthread_create(&thread_Timer, &attr, CountDown_Timer, NULL);
		}
		else if("A{finish}"== task.getTaskName())
		{
			logInfoStream<<ReturnTime()<<" - End process "<<Pnum<<"\n";
			P.setProcessStatus(4);			 
		}
		else if("P{run}"== task.getTaskName())
		{
			current_Time = (float)clock()/ CLOCKS_PER_SEC;
			if(task.getTaskInterruptStatus())
				logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": resume processsing action"<<"\n";
			else
				logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": start processsing action"<<"\n";
			P.setProcessStatus(3);
			countdown_Time_Task = task.getTaskExecutionTime();
			r = ProcessWait();
			if(r) //if there is interruption, back up process and task status, then return 1
			{
				diff_Time = ((float)clock()/ CLOCKS_PER_SEC - current_Time)*1000;
				remaningTaskExecutionTime = task.getTaskExecutionTime() - diff_Time;
				if(remaningTaskExecutionTime>0)
				{
					task.setExecutionTime ( remaningTaskExecutionTime); 
					task.setTaskInterruptStatus(1); 
					taskQueue.pop();
					taskQueueTemp.push(task);
					taskQueue = copyTaskqueue(taskQueueTemp, taskQueue); //make interruped task at the front of the task queue
					P.setProcessInterrupStatus(1);
					P.setProcessOnQueue(taskQueue); 
					processList.push_back(P);  //push the interruped process in the process list with new updated status
					logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": interrupt processing action"<<"\n";
					return 1;
				}
			}
			else
			{
				P.setProcessStatus(2);
				logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": end Processsing action"<<"\n";
			}
		}
		else if("M{allocate}"== task.getTaskName()) 
		{
			current_Time = (float)clock()/ CLOCKS_PER_SEC;
			if (task.getTaskInterruptStatus())
				logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": resume allocating memory"<<"\n";
			else
				logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": allocating memory"<<"\n";
			P.setProcessStatus(3);
			countdown_Time_Task = task.getTaskExecutionTime();
                        r = ProcessWait();
			if(r)  //if there is interruption, back up process and task status, then return 1
			{
				diff_Time = ((float)clock()/ CLOCKS_PER_SEC - current_Time)*1000;
				remaningTaskExecutionTime = task.getTaskExecutionTime() - diff_Time;
				if(remaningTaskExecutionTime>0)
				{
					task.setExecutionTime ( remaningTaskExecutionTime); 
					task.setTaskInterruptStatus(1);
					taskQueue.pop();
					taskQueueTemp.push(task);
					taskQueue = copyTaskqueue(taskQueueTemp, taskQueue); //make interruped task at the front of the task queue
					P.setProcessInterrupStatus(1); 
					P.setProcessOnQueue(taskQueue);
					processList.push_back(P);  //push the interruped process in the process list with new updated status
					logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": interrupt allocating memory"<<"\n";
					return 1;
				}
			}
			else
			{
				P.setProcessStatus(2);
				logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<":memory allocated at 0X"<<allocateSystemMemory()<<"\n";
			}
		}
		else if("O{monitor}"== task.getTaskName()) //IO tasks are unterrupted 
		{
			pthread_mutex_lock( &lock_Thread );
			isIO = true;
			logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": start monitor output"<<"\n";
			P.setProcessStatus(3);
			pthread_attr_init(&attr);
			countdown_Time_Task = task.getTaskExecutionTime();
			pthread_create(&thread, &attr, InputOutput_Thread, NULL );
			pthread_join(thread, NULL);
			P.setProcessStatus(2);
			logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": end monitor output"<<"\n";
			isIO = false;
			pthread_mutex_unlock( &lock_Thread );
		}
		else if("I{hard drive}"== task.getTaskName()) //IO tasks are unterrupted 
		{
			pthread_mutex_lock( &lock_Thread );	
			isIO = true;
			logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": start hard drive Input on HDD "<<allocateHardDrive()<<"\n";	
			P.setProcessStatus(3);
			pthread_attr_init(&attr);
			countdown_Time_Task = task.getTaskExecutionTime();
			cout<<"I hard drive is: "<<countdown_Time_Task<<"   "<<task.getTaskInterruptStatus()<<endl;
			pthread_create(&thread, &attr, InputOutput_Thread, NULL );
			pthread_join(thread, NULL);
			P.setProcessStatus(2);             
			logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": end hard drive input"<<"\n";
			isIO = false;
			pthread_mutex_unlock( &lock_Thread );  
		}
		else if("O{projector}"== task.getTaskName()) //IO tasks are unterrupted 
		{
			pthread_mutex_lock( &lock_Thread );
			isIO = true;
			logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": start projector output on PROJ "<<allocateProjectors()<<"\n";
			P.setProcessStatus(3);
			pthread_attr_init(&attr);
			countdown_Time_Task = task.getTaskExecutionTime();
			pthread_create(&thread, &attr, InputOutput_Thread, NULL );
			pthread_join(thread, NULL);
			P.setProcessStatus(2);
			logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": end projector output"<<"\n"; 
			isIO = false;     
			pthread_mutex_unlock( &lock_Thread );         
		}
		else if("M{block}"== task.getTaskName())
		{
			current_Time = (float)clock()/ CLOCKS_PER_SEC;
			if( task.getTaskInterruptStatus())
				logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": resume memory blocking"<<"\n";
			else
				logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": start memory blocking"<<"\n";
			P.setProcessStatus(3);
			pthread_attr_init(&attr);
			countdown_Time_Task = task.getTaskExecutionTime();
			r = ProcessWait();
			if(r)  //if there is interruption, back up process and task status, then return 1
			{
				diff_Time = ((float)clock()/ CLOCKS_PER_SEC - current_Time)*1000;
				remaningTaskExecutionTime = task.getTaskExecutionTime() - diff_Time;
				if(remaningTaskExecutionTime>0)
				{
					task.setExecutionTime ( remaningTaskExecutionTime); 
					task.setTaskInterruptStatus(1); 
					taskQueue.pop();
					taskQueueTemp.push(task);
					taskQueue = copyTaskqueue(taskQueueTemp, taskQueue); //make interruped task at the front of the task queue
					P.setProcessInterrupStatus(1);
					P.setProcessOnQueue(taskQueue);
					processList.push_back(P);  //push the interruped process in the process list with new update status
					logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": interrupt memory blocking"<<"\n";
					return 1;
				}
			}
			else
			{
				P.setProcessStatus(2);
				logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": end memory blocking"<<"\n";
			}
		}
		else if("I{keyboard}"== task.getTaskName()) //IO tasks are unterrupted 
		{
			pthread_mutex_lock( &lock_Thread );
			isIO = true;
			logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": start keyboard input"<<"\n";
			P.setProcessStatus(3);
			pthread_attr_init(&attr);
			countdown_Time_Task = task.getTaskExecutionTime();
			pthread_create(&thread, &attr, InputOutput_Thread, NULL );
			pthread_join(thread, NULL);
			P.setProcessStatus(2);
			logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": end keyboard input"<<"\n";
			isIO = false;
			pthread_mutex_unlock( &lock_Thread );
		}
		else if("O{hard drive}"== task.getTaskName()) //IO tasks are unterrupted 
		{
			pthread_mutex_lock( &lock_Thread );
			isIO = true;
			logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": start hard drive output on HDD "<< allocateHardDrive()<<"\n";	
			P.setProcessStatus(3);
			pthread_attr_init(&attr);
			countdown_Time_Task = task.getTaskExecutionTime();
			pthread_create(&thread, &attr, InputOutput_Thread,NULL);
			pthread_join(thread, NULL);
			P.setProcessStatus(2);
			logInfoStream<<ReturnTime()<<" - Process "<<Pnum <<": end hard drive output"<<"\n";
			isIO = false;
			pthread_mutex_unlock( &lock_Thread );
		}
		else
		{
			cout<<"Program terminated"<<endl;
			cout<<"Process name is missing in Meta data file"<<endl<<endl;	
			exit(0);
		} 
		taskQueue.pop();		
	}
	//unlock process
	pthread_mutex_unlock( &lock_Process );
	return 0;
}


// Return time for the tasks
double ReturnTime()
{
	double diff_Time, current_Time;
	current_Time = (double)clock()/ CLOCKS_PER_SEC;
	diff_Time = current_Time - offSetTime;
	return diff_Time;
}
// function running by input out thread
void *InputOutput_Thread(void *value)
{
  	ProcessWait();
   	pthread_exit(NULL);
}
// ProcessWait function checks the task completed or not in every 100 ms (i.e value of countdown_Time_Task)
int  ProcessWait()
{
	while ( countdown_Time_Task > 0)
	{
		Wait(100);//wait 100 ms
		if( !isIO && *isnewProcessInQueue )  //if new process, return to update queue
		{
			return 1;
		}
	}
	return 0;
}
// wait function based on input milliseconds
int Wait( int milliseconds)
{
	clock_t x_endTime;
	x_endTime = clock() + milliseconds * CLOCKS_PER_SEC/1000;
	while (clock() < x_endTime)
	{
		if( !isIO && *isnewProcessInQueue )  //if new process, return to update queue
		{
			return 1;
		}
	}
	return 0;	
}
// Timer function that is run by pthread_timer from process start to process end
void * CountDown_Timer(void * value)
{
	clock_t x_endTime;
	while (timer_Run)
	{	
		while ( countdown_Time_Task <= 0)
		{
			//cout<<"The counter value is: "<<countdown_Time_Task<<endl;
		}
		x_endTime = clock() + countdown_Time_Task * CLOCKS_PER_SEC/1000;
		while (clock() < x_endTime)
		{
			if( !isIO && *isnewProcessInQueue ) //if new process, reset counter
			{
				break;
			}
		}
		countdown_Time_Task = 0;
	}
	pthread_exit(NULL); 
}
// Function that is run by pthread_RRQuantumTime to record the quantum time (50ms to interrupt)
void * quantumTimeTimer(void * value)
{
	while( TimerRun)
	{
		waitQuantumTime(50); //wait 50ms
		isQuantumTimeFinished = true;	
	}
	pthread_exit(NULL); 
}
//Function that waits for 50ms quantum time
int waitQuantumTime( int milliseconds)
{
	clock_t x_endTime;
	x_endTime = clock() + milliseconds * CLOCKS_PER_SEC/1000;
	while (clock() < x_endTime);
	return 0;
}
//allocate system memory for a particular task
string allocateSystemMemory()
{
	ostringstream s_string;
        string str;
	// if memory is full then allocate memory from beginning of system memory
	if (memoryBlockSize>totalSystemMem-curreltlyUsedMem)
	{
		curreltlyUsedMem=memoryBlockSize;
		s_string<<std::hex <<(curreltlyUsedMem-memoryBlockSize);
		str=s_string.str();
		for (int i=str.length();i<8;i++)
		{
			str="0"+str; 
		}
		return str;
	}
	// allocate system memory from the first availabe address space
	s_string<<std::hex <<curreltlyUsedMem;
	str=s_string.str();
	for (int i=str.length();i<8;i++)
	{
		str="0"+str;   
	}             
	curreltlyUsedMem=curreltlyUsedMem + memoryBlockSize; //update memory used
	return str;

}
// allocate a particular Projector for a particular task
int allocateProjectors()
{
	int currentlyUsedPro = currentlyUsedProjectors;
	//semaphore lock
	//int value;
        //sem_getvalue(&s_Projector, &value);
        //cout<<value<<endl;
	sem_wait(&s_Projector);
	currentlyUsedProjectors++;
	//if all projectors are allocated then start allocating from beginneing
	if (currentlyUsedPro > (totalNumProjectors - 1))
	{
		currentlyUsedProjectors = 0;
		currentlyUsedPro = currentlyUsedProjectors;
		currentlyUsedProjectors++;
		return currentlyUsedPro;
	}
	//semaphore unlock
	sem_post(&s_Projector);
	return currentlyUsedPro;
}
// allocate a particualr HardDrive for a particular task
int allocateHardDrive()
{
	int currentlyUsedHd = currentlyUsedHD;
	//semaphore lock hard drive
	//int value;
        //sem_getvalue(&s_HardDrive, &value);
        //cout<<value<<endl;
	sem_wait(&s_HardDrive);
	currentlyUsedHD++;
	if (currentlyUsedHd > (totalNumHd - 1))
	{
		currentlyUsedHD = 0;
		currentlyUsedHd = currentlyUsedHD;
		currentlyUsedHD++;
		return currentlyUsedHd;
	}
	//semaphore unlock hard drive
	sem_post(&s_HardDrive);
	return currentlyUsedHd;
}
//Sorting processes based on the Algorithm given in the input config file
void sortProcessList(string schedulingAlgo)
{
	queue<Task> taskQueueTem;
	list<Process> processListTem;
	float totalProcessExecutionTime;
	if (schedulingAlgo == "SJF")
	{
		processList.sort(sortProcessSJF) ;
	}
	else if(schedulingAlgo == "PS")
	{
		processList.sort(sortProcessPS) ;
	}
	else if( schedulingAlgo == "STR" )
	{
		while(!processList.empty()) //before sorting, recalculate the total process execution time and update
		{
			Process p = processList.front();
			processList.pop_front();
			totalProcessExecutionTime = 0.0;
			queue<Task> taskQueueT;
			taskQueueTem = p.getTaskOnQueue();
			while(!taskQueueTem.empty())
			{
				Task T = taskQueueTem.front();
				taskQueueTem.pop();
				taskQueueT.push(T);
				totalProcessExecutionTime += T.getTaskExecutionTime ();
			}
			p.setProcessOnQueue(taskQueueT);
			p.setProcessTotalExecutionTime(totalProcessExecutionTime );
			processListTem.push_back(p);
		}
		processList = processListTem;
		processList.sort(sortProcessSTR);
	}
}
//Sort processes based on the sortest job first algorithm
bool sortProcessSJF(const Process &firstProcess,const Process &secondProcess)
{
	return firstProcess.totalNumofTasks < secondProcess.totalNumofTasks;
}
//sort processes based on the priority schedule algorithm
bool sortProcessPS(const Process &firstProcess,const Process &secondProcess)
{
	return firstProcess.totalNumofIOOps > secondProcess.totalNumofIOOps;
}
//sort processes based on the shortest time remaining schedule algorithm
bool sortProcessSTR(const Process &firstProcess,const Process &secondProcess)
{
	return firstProcess.processTotalExecutionTime < secondProcess.processTotalExecutionTime;
}
void setResetTriggingBool(int i)
{
	pthread_mutex_lock( &lock_bool ); //lock
	if( i == 0)
	{
		isnewProcessReloaded = true; //reload thread set it when it finishes reloading processes in each 100ms
	}
	else
	{
		isnewProcessReloaded = false; //executing thread reset it when it finishes updating it process list
	}
	pthread_mutex_unlock( &lock_bool ); //unlock
}
//Update the Process list
void updateProcessList(Process p, int Num)
{
	pthread_mutex_lock( &lock_processList); //lock
	if( Num == 1) //reload thread update process list
	{
		processListReload.push_back(p);
	}
	else if( Num ==2 ) // process executing thread update processlist
	{
		list <Process>::iterator itr;
		for(itr = processListReload.begin(); itr != processListReload.end(); itr++)
		{
			processList.push_back(*itr);

		}
	}
	else if( Num == 3)// if RR and the process is interrupted put that process after the new tasks in thelist
	{
		list <Process>::iterator itr;
		Process p = processList.back();
		processList.pop_back();
		for(itr = processListReload.begin(); itr != processListReload.end(); itr++)
		{
			processList.push_back(*itr);

		}
		processList.push_back(p);//put the currently running process in the back of the queue after the new coming processes
	}
	pthread_mutex_unlock( &lock_processList ); //unlock
}
//copy taskQueue to make the interrupted task at the front of the queue
queue<Task> copyTaskqueue(queue<Task> taskQueueTemp, queue<Task> taskQueue)
{
	while(!taskQueue.empty())
	{
		Task T = taskQueue.front();
		taskQueueTemp.push(T);
		taskQueue.pop();
	}
	return taskQueueTemp;
}
//convert string cycle value to integer
int convertStringToNum(const string& s)
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

//This function removes unnecessary space at the beginning and end 
string removeSpace(string &str)
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
//parse the line based on the single delimeter
void parse(const string& s, char c,vector<string>& v)
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
	else
	{
		cout<<"Input is not in correct format for parsing"<<endl;
		exit(0);                
	}
}

//parsing the strings on metadata based the multiple delimeters
vector<string> parseMetaData(const string &s, const string &sep)
{	
	static bool dict[size] = { false};
	vector<string> parse;
	for ( unsigned int i = 0; i < sep.size(); ++i) 
	{      
		dict[(unsigned char)sep[i]] = true;
	}
	string token("");
	for (i : s) 
	{
		if ((unsigned char) dict[i]) 
		{
			if (!token.empty()) 
			{
				token=removeSpace(token);			
				parse.push_back(token);
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
			token=removeSpace(token);
			parse.push_back(token);
		}
	}
	return parse;
}


