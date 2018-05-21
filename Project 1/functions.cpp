#include "functions.h"
const int dictSize = 256; 

void readMetadataFile(ConfigFile Configfile)
{
	fstream file,myfile;
	//cout<<"here"<<endl;
	string line,metadatafilePath,logfilepath;
	vector<string> splitted;
	metadatafilePath=Configfile.getmdfFilepath();
	logfilepath=Configfile.getlogFilePath();
	int log=Configfile.getLogged();
	//cout<<"metadatafileName="<<metadatafilePath<<endl;
	//cout<<"logfilepath="<<logfilepath<<endl;
	split(metadatafilePath,'.',splitted);
	//cout<<"Configfile.getlogFilePath="<<Configfile.getlogFilePath()<<endl;
	myfile.open(Configfile.getlogFilePath().c_str(), std::fstream::app);
	//cout<<"here"<<endl;
	if(splitted[1].compare("mdf")!=0)
	{
		cout<<"Metadata file extension not correct"<<endl;
		exit(0);
	}
	file.open(metadatafilePath.c_str());
	if ( file.peek() == std::ifstream::traits_type::eof() )
	{
   		cout<<"Program Stopped"<<endl;
	    cout<<"MDF file is empty or Missing!!"<<endl;
        exit(0);
	}
	
	if(Configfile.getLogged()==1) 
	{
		myfile<<endl;
		myfile<<"Meta-Data Metrics"<<endl;
		myfile.close();
	}
	if(Configfile.getLogged()==2)
	{
		myfile<<endl;
		myfile<<"Meta-Data Metrics"<<endl;
		myfile.close();

		cout<<endl;
		cout<<"Meta-Data Metrics"<<endl;

	}
	if(Configfile.getLogged()==3)
	{
		cout<<endl;
		cout<<"Meta-Data Metrics"<<endl;
	}


	if(file.is_open())
	{
		while(getline(file,line))
		{
			if(line!="Start Program Meta-Data Code:" && line!="End Program Meta-Data Code.")
			{
				splitted=tokenizeMyString(line,".!,;:");
				
				for(int i=0;i<splitted.size();i++)
				{
					//cout<<splitted[i]<<endl;
					//send the vector to a function and print the values to file or monitor accordingly
					operate(splitted[i], logfilepath, log, Configfile);
				}	
				//cout<<"******"<<endl;
			}

		}
	}
	file.close();

}

void split(const string& s, char c,vector<string>& v)
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
		else{
                //    v.push_back(s);
 		cout<<"Input is not correct for the Split function"<<endl;
                exit(0);                
		}
	}

void operate(string token, string logfilepath, int log, ConfigFile configfile)
{
	ofstream mdffile;
	int tempCycle=0;
	int fullCycle=0;
	//cout<<"token="<<token<<endl;
	mdffile.open(logfilepath.c_str(), std::fstream::app );
	vector<string> indParts;
	split(token,'}',indParts);
	//cout<<indParts[0]<<"***"<<indParts[1]<<endl;
	
	istringstream buffer(indParts[1]);
	buffer>>tempCycle;
	
	//cout<<"ConfigFile.getProcessor="<<configfile.getProcessor()<<endl;
	//calculating full cycle
	string tokenPart=indParts[0];
	//cout<<"tokenPart====="<<tokenPart<<endl;
	if(tempCycle<0)
	{
		cout<<"Cycle time is negative"<<endl;
		exit(0);
	}
	else if(tempCycle==0)
	{	
		//do nothing
	}
	else if(tokenPart.find("p{")!=std::string::npos || tokenPart.find("P{")!=std::string::npos)
	{
		fullCycle=configfile.getProcessor()*tempCycle;
	}
	else if(tokenPart.find("m{")!=std::string::npos || tokenPart.find("M{")!=std::string::npos)
	{
		fullCycle=configfile.getMemory()*tempCycle;
	}
	else if(tokenPart.find("hard drive")!=std::string::npos || tokenPart.find("Hard Drive")!=std::string::npos)
	{
		fullCycle=configfile.getHardDrive()*tempCycle;
	}	
	else if(tokenPart.find("projector")!=std::string::npos || tokenPart.find("Projector")!=std::string::npos)
	{
		fullCycle=configfile.getProjector()*tempCycle;
	}
	else if(tokenPart.find("keyboard")!=std::string::npos || tokenPart.find("Keyboard")!=std::string::npos)
	{
		fullCycle=configfile.getKeyboard()*tempCycle;
	}		
	else if(tokenPart.find("monitor")!=std::string::npos || tokenPart.find("Monitor")!=std::string::npos)
	{
		fullCycle=configfile.getMonitor()*tempCycle;
	}
	else if(tokenPart.find("Scanner")!=std::string::npos || tokenPart.find("scanner")!=std::string::npos)
	{
		fullCycle=configfile.getScanner()*tempCycle;
	}
	else {
		cout<<tokenPart<<endl;
		cout<<"Nothing";
	}
	if(tempCycle!=0 && configfile.getLogged()==1)
	{
		mdffile<<indParts[0]<<"}"<< indParts[1]<< " - "<<fullCycle  << " ms"<<endl;
	}
	if(configfile.getLogged()==2 && tempCycle!=0)
	{
		mdffile<<indParts[0]<<"}"<< indParts[1]<< " - "<<fullCycle  << " ms"<<endl;
		cout<<indParts[0]<<"}"<< indParts[1]<< " - "<<fullCycle  << " ms"<<endl;
	}
	if(configfile.getLogged()==3 && tempCycle!=0)
	{
		cout<<indParts[0]<<"}"<< indParts[1]<< " - "<<fullCycle  << " ms"<<endl;

	}

	
	mdffile.close();


}

vector<string> tokenizeMyString(const string &s, const string &del)
{	
        
	       static bool dict[dictSize] = { false};
	       vector<string> res;
    		for ( unsigned int i = 0; i < del.size(); ++i) {      
                dict[(unsigned char)del[i]] = true;
    		 }

    		string token("");
   		 for ( i : s) {
        		if ((unsigned char) dict[i]) {
           			 if (!token.empty()) {
       					
                                                  
					token=delUnnecessary(token);			
					res.push_back(token);
                                       
                			token.clear();
            				}           
        			}
        			else {
            		token += i;
        		}
    		}
   		 if (!token.empty()) {
      			if (token!=" ")
      			 {

			      token=delUnnecessary(token);
 
  			      res.push_back(token);
	
				}
  			  }
  			  return res;
			}
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

