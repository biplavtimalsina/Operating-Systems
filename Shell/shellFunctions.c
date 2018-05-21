#include<shellFunctions.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<ctype.h>
#include<ncurses.h>
#include<term.h>


pid_t pidList[5]={0};

// Function to take input
int takeInput(char* str)
{
    char buf[MAXCOM];
 
   fgets(buf,MAXCOM,stdin);
   //printf("command is %s",buf);

  // add_history(buf);
   
    if (strlen(buf) != 0) {
       // add_history(buf);
        strcpy(str, buf);
     //   printf("here");
        return 0;
    } else {
        //printf("hi");
        return 1;
    }
   
}

static char* skipwhite(char* s)
{
    while (isspace(*s)) ++s;
    return s;
}

int split(char* cmd,char *args[MAXLIST], int i)
{
    
    
    if(*cmd=='\0')
    {
        printf("Error: No input");
        status=5;
    }

    //checking for adjacent whitespaces
    int j=0;
   /* printf("first value: %c \n", *(cmd));
    printf("f2 value: %c \n", *(cmd+1));
    printf("f3 value: %c \n", *(cmd+2));
    printf("f4 value: %c \n", *(cmd+3));
    */
    //printf("cmd:*%s*\n",cmd);
    while(*(cmd+j+1)!='\n')
    {
        //printf("f value: %c   f1 value: %c \n", *(cmd+j), *(cmd+j+1));
        if(isspace(*(cmd+j)) && isspace(*(cmd+j+1)))
        {
            printf("Error: Adjacent whitespaces \n");
            status=3;
            break;
        }  
        j++;
    }

    //checking for leading whitespaces
    if(isspace(*cmd))
    {
        printf("Error: Leading whitespace \n");
        status=2;
    }

    //checking for tailing whitespaces
    j=0;
    while(*(cmd+j+1)!='\n')
        j++;
    if(isspace(*(cmd+j)))
    {
        printf("Error: Tailing whitespace \n");
        status=4;
    }
    



    cmd = skipwhite(cmd);
    char* next = strchr(cmd, ' ');
 
    while(next != NULL) {
        next[0] = '\0';
        args[i] = cmd;
        ++i;
        cmd = skipwhite(next + 1);
        next = strchr(cmd, ' ');
    }
 
    if (cmd[0] != '\0') {
        args[i] = cmd;
        next = strchr(cmd, '\n');
        next[0] = '\0';
        ++i; 
    }
 
    args[i] = NULL;
    if(i>=11)
    {
        printf("Error: More than 10 seperate tokens\n ");
        status=1;
    }
    return i;
}

void process(char *inputString)
{
    //printf("Process command: %s", inputString);
    
    char* parsed[MAXLIST];
   
    int size=0;
       
         
    size= split(inputString,parsed,size);
    if(!status)
    {   
        //printf("**\n");
        /*
        for(int j=0;j<size;j++)
                printf("%s \n",parsed[j]);
        */
        //printf("*\n");
        
        int NoOfCommands=2, switchArg=0;
        char* ListofCommands[2];
        ListofCommands[0]="exit";
        ListofCommands[1]="showpid";

       // printf("Command: %s",inputString);
        
        for(int i=0;i<NoOfCommands;i++)
        {
            //printf("command= %s* and stored= %s* \n",parsed[0], ListofCommands[i]);
            if(strcmp(parsed[0],ListofCommands[i])==0)
            {
                switchArg=i+1;
               // printf("switchArg= %d",switchArg);
                break;
            }
        }

        switch(switchArg){
            case 1:
                printf("\nExit \n");
                exit(0);

            case 2:
                printf("The 5 most recent PID are:\n");
                for(int i=0;i<5;i++){
                    printf("%d \n", pidList[i]);
                }
                break;
            default:
                {
                        //handle other inbuilt commands
                        //printf("Wrong command\n");
                        pid_t pid,wpid;
                        

                        pid=fork();
                        if(pid==0)
                        {
                            //child process
                            //printf("Child Process id: %d \n", getpid());
                            //printf("Parent Process id: %d \n", getppid());
                            if(execvp(parsed[0],parsed)==-1)
                            {
                                perror("lsh");
                            }
                            exit(EXIT_FAILURE);
                            //printf("*%s:*  \n", parsed[0]); 
                            //printf("*%s:*  \n", parsed[1]);
                            //mkprintf("*%s:*  \n", parsed[2]);
                            //execvp(parsed[0],parsed);
                            /*
                            char *cmd = "ls";
                            char *argv[3];
                            argv[0] = "ls";
                            argv[1] = "-la";
                            argv[2] = NULL;

                            execvp(cmd, argv);
                            */ 
                        }
                        else if(pid<0)
                        {
                            //Error Forking
                            perror("lsh");
                        }
                        else 
                        {
                            //printf("*pid: %d",pid);
                            do{
                                wpid=waitpid(pid,&status,WUNTRACED);

                            }while(!WIFEXITED(status) && ! WIFSIGNALED(status));
                        }

                        //update pid
                        for(int i=4;i>0;i--)
                        {
                            pidList[i]=pidList[i-1];
                        }
                        pidList[0]=pid;


                        break;
                    
                }
                switchArg=100;
    }
    }
}


void displayPrompt()
{
    static int first_time=1;
    if(first_time)
    {
            const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
            write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
            first_time=0;

            
            
    }
    printf("\nprompt$ ");
}
